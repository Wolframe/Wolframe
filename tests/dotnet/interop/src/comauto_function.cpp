#include "comauto_function.hpp"
#include "comauto_utils.hpp"
#include "comauto_clr.hpp"
#include "comauto_recordinfo.hpp"
#include <iostream>
#include <sstream>

using namespace _Wolframe;

namespace
{
class FunctionResult
	:public langbind::TypedInputFilter
{
public:
	FunctionResult( const CComPtr<ITypeInfo>& typeinfo, const VARIANT& data_)
		:types::TypeSignature( "comauto::FunctionResult", __LINE__)
	{
		std::memset( &m_data, 0, sizeof(m_data));
		m_data.vt = VT_EMPTY;
		WRAP( comauto::wrapVariantCopy( &m_data, &data_))
		m_stk.push_back( StackElem( Init, typeinfo, 0));
	}

	FunctionResult( const FunctionResult& o)
		:types::TypeSignature( "comauto::FunctionResult", __LINE__)
		,m_stk(o.m_stk)
		,m_elembuf(o.m_elembuf)
	{
		std::memset( &m_data, 0, sizeof(m_data));
		m_data.vt = VT_EMPTY;
		WRAP( comauto::wrapVariantCopy( &m_data, &o.m_data))
	}

	virtual ~FunctionResult()
	{
		comauto::wrapVariantClear( &m_data);
	}

	virtual TypedInputFilter* copy() const
	{
		return new FunctionResult( *this);
	}

	virtual bool getNext( ElementType& type, Element& element);
	const VARIANT* data() const		{return &m_data;}

private:
	enum State
	{
		Init,
		VarOpen,
		VarValue,
		VarClose
	};
	struct StackElem
	{
		State state;
		CComPtr<ITypeInfo> typeinfo;
		TYPEATTR* typeattr;
		VARDESC* vardesc;
		std::size_t ofs;
		unsigned short idx;

		StackElem( const StackElem& o);
		StackElem( State state_, CComPtr<ITypeInfo> typeinfo_, std::size_t ofs_);
		~StackElem();
	};
	std::vector<StackElem> m_stk;
	std::string m_elembuf;
	VARIANT m_data;
};
}//anonymous namespace


comauto::Function::Function( comauto::CommonLanguageRuntime* clr_, ITypeInfo* typeinfo_, const std::string& assemblyname_, const std::string& classname_, unsigned short fidx)
	:m_clr(clr_)
	,m_typeinfo(typeinfo_)
	,m_funcdesc(0)
	,m_assemblyname(assemblyname_)
	,m_classname(classname_)
{
	WRAP( m_typeinfo->GetFuncDesc( fidx, &m_funcdesc))
	struct Local	//exception safe memory allocation of local variables
	{
		BSTR* pnames;
		UINT size;

		Local()	:pnames(0){}
		~Local()
		{
			if (pnames)
			{
				for (UINT ii=0; ii<size; ++ii) if (pnames[ii]) ::SysFreeString( pnames[ii]);
				delete [] pnames;
			}
		}
	};
	Local local;
	local.pnames = new BSTR[ local.size = m_funcdesc->cParams+1];
	UINT ii,nn;
	WRAP( m_typeinfo->GetNames( m_funcdesc->memid, local.pnames, m_funcdesc->cParams+1, &nn))

	m_methodname = comauto::utf8string( local.pnames[0]);
	for (ii=1; ii<nn; ++ii)
	{
		const TYPEDESC* td = &m_funcdesc->lprgelemdescParam[ii-1].tdesc;
		if (td->vt == VT_USERDEFINED)
		{
			CComPtr<ITypeInfo> rectypeinfo;
			WRAP( m_typeinfo->GetRefTypeInfo( td->hreftype, &rectypeinfo))
			Parameter param( comauto::utf8string( local.pnames[ii]), td, comauto::getRecordInfoMap( rectypeinfo));
			m_parameterlist.push_back( param);
		}
		else
		{
			m_parameterlist.push_back( Parameter( comauto::utf8string( local.pnames[ii]), td));
		}
	}
	m_returntype.typedesc = &m_funcdesc->elemdescFunc.tdesc;
	if (m_returntype.typedesc->vt == VT_USERDEFINED)
	{
		WRAP( m_typeinfo->GetRefTypeInfo( m_returntype.typedesc->hreftype, &m_returntype.typeinfo))
	}
}

comauto::Function::~Function()
{
	m_typeinfo->ReleaseFuncDesc( m_funcdesc);
}

void comauto::Function::print( std::ostream& out) const
{
	if (m_classname.empty())
	{
		out << "FUNCTION " << m_methodname << "( ";
	}
	else
	{
		out << "METHOD " << m_classname << "." << m_methodname << "( ";
	}
	std::vector<Parameter>::const_iterator pi = m_parameterlist.begin(), pe = m_parameterlist.end();
	for (; pi != pe; ++pi)
	{
		if (pi != m_parameterlist.begin()) out << ", ";
		out << pi->name << " :" << comauto::typestr( m_typeinfo, pi->typedesc);
	}
	out << " ) :" << comauto::typestr( m_typeinfo, m_returntype.typedesc) << std::endl;
}

std::size_t comauto::Function::getParameterIndex( const std::string& name) const
{
	std::vector<Parameter>::const_iterator pi = m_parameterlist.begin(), pe = m_parameterlist.end();
	for (; pi != pe; ++pi)
	{
		if (pi->name == name) return pi-m_parameterlist.begin();
	}
	throw std::runtime_error( std::string("unknown parameter name '") + name + "'");
}

const comauto::Function::Parameter* comauto::Function::getParameter( std::size_t idx) const
{
	if (idx >= m_parameterlist.size()) throw std::runtime_error( "parameter index out of range");
	return &m_parameterlist.at( idx);
}

comauto::FunctionClosure::FunctionClosure( const Function* func_)
		:m_provider(0)
		,m_func(func_)
		,m_flags(serialize::Context::None)
		,m_parameter(0)
		,m_paramvalue(0)
		,m_paramidx(0xFFFF)
		,m_recdepht(0){}

comauto::FunctionClosure::~FunctionClosure()
{
	if (m_parameter)
	{
		std::size_t pi = 0, pe = m_func->nofParameter();
		for (; pi != pe; ++pi)
		{
			if (m_parameter[pi].vt == VT_USERDEFINED || m_parameter[pi].vt == VT_RECORD)
			{
				IRecordInfo* recinfo = m_parameter[pi].pRecInfo;
				if (recinfo)
				{
					recinfo->RecordDestroy( m_parameter[pi].pvRecord);
				}
			}
		}
		delete [] m_parameter;
	}
}

void comauto::FunctionClosure::init( const proc::ProcessorProvider* p, const langbind::TypedInputFilterR& i, serialize::Context::Flags f)
{
	m_provider = p;
	m_input = i;
	m_flags = f;
	std::size_t ii = 0,nn = m_func->nofParameter();
	if (m_parameter) delete [] m_parameter;
	m_parameter = 0;	//... because of exception safety (new VARIANT[] might fail)
	m_parameter = new VARIANT[ m_func->nofParameter()];
	for (; ii < nn; ++ii)
	{
		m_parameter[ii].vt = VT_EMPTY;
	}
	m_statestack.clear();
	m_statestack.push( StateStackElem( CheckParam, 0));
}

VARIANT* comauto::FunctionClosure::initParameter( std::size_t eidx, VARIANT* val, const IRecordInfo* recinfo) const
{
	const Function::Parameter* param = m_func->getParameter( eidx);

	if (m_parameter[ eidx].vt != VT_EMPTY)
	{
		if ((param->typedesc->vt & VT_ARRAY) == VT_ARRAY || (param->typedesc->vt & VT_VECTOR) == VT_VECTOR)
		{
			//.... create new array element here
			throw std::runtime_error( "no support for arrays implemented yet");
		}
		throw std::runtime_error( std::string( "duplicate parameter '") + param->name + "'");
	}
	else
	{
		if (param->typedesc->vt == VT_USERDEFINED || param->typedesc->vt == VT_RECORD)
		{
			if (!recinfo) throw std::logic_error( "structure expected");
			val->vt = param->typedesc->vt;
			val->pRecInfo = const_cast<IRecordInfo*>(recinfo);
			val->pvRecord = val->pRecInfo->RecordCreate();
/*[-]*/std::cout << "DEBUG record created at 0x0" << std::hex << (uintptr_t)val->pvRecord << std::dec << std::endl;
			if (!val->pvRecord) throw std::logic_error( "failed to create record data structure");
			return val;
		}
		else if (comauto::isStringType( param->typedesc->vt))
		{
			if (recinfo) throw std::logic_error( "string instead of structure expected");
			std::memset( val, 0, sizeof( VARIANT));
			val->vt = param->typedesc->vt;
			return val;
		}
		else if (comauto::isAtomicType( param->typedesc->vt))
		{
			if (recinfo) throw std::logic_error( "atomic type instead of structure expected");
			val->vt = param->typedesc->vt;
			std::size_t vtsize = comauto::sizeofAtomicType( val->vt);
			::memset( comauto::arithmeticTypeAddress(val), 0, vtsize);
			return val;
		}
	}
	throw std::runtime_error( std::string( "unable to handle type of parameter '") + param->name + "'");
}

void comauto::FunctionClosure::assignValue( VARIANT* dstrec, std::size_t ofs, VARTYPE dsttype, const langbind::TypedFilterBase::Element& elem)
{
	VARIANT elemval;
	elemval.vt = VT_EMPTY;
	try
	{
		elemval = comauto::createVariantType( elem, dsttype);
		if (comauto::isAtomicType( dstrec->vt) || comauto::isStringType( dstrec->vt))
		{
			if (ofs) throw std::logic_error( "illegal operation (offset into atomic type)");
			comauto::wrapVariantCopy( dstrec, &elemval);
		}
		else if (dstrec->vt == VT_USERDEFINED || dstrec->vt == VT_RECORD) 
		{
			if (!dstrec->pvRecord) throw std::logic_error( "illegal data structure for this operation (pvRecord not defined)");
			PVOID field = (PVOID)((char*)dstrec->pvRecord + ofs);
/*[-]*/std::cout << "DEBUG copy element (value '" << elem.tostring() << "' to 0x0" << std::hex << (uintptr_t)dstrec->pvRecord << " at ofs 0x0" << ofs << std::dec << std::endl;
			copyVariantType( dsttype, field, elem);
		}
		else
		{
			throw std::runtime_error( std::string("cannot handle this structure type: ") + comauto::typestr( dstrec->vt));
		}
	}
	catch (const std::runtime_error& e)
	{
		comauto::wrapVariantClear( &elemval);
		throw e;
	}
}

bool comauto::FunctionClosure::call()
{
	try
	{
		langbind::TypedFilterBase::ElementType elemtype;
		langbind::TypedFilterBase::Element elemvalue;

		while (!m_statestack.empty() && m_input->getNext( elemtype, elemvalue))
		{
			State state = m_statestack.top().m_state;
			switch (state)
			{
				case CheckParam:
					switch (elemtype)
					{
						case langbind::InputFilter::Attribute:
						case langbind::InputFilter::OpenTag:
						{
							if (elemvalue.type == langbind::TypedFilterBase::Element::int_)
							{
								if (elemvalue.value.int_ < 0 || (std::size_t)elemvalue.value.int_ >= m_func->nofParameter()) throw std::runtime_error( "function parameter index out of range");
								m_paramidx = (std::size_t)elemvalue.value.int_;
							}
							else if (elemvalue.type == langbind::TypedFilterBase::Element::uint_)
							{
								if ((std::size_t)elemvalue.value.uint_ >= m_func->nofParameter()) throw std::runtime_error( "function parameter index out of range");
								m_paramidx = (std::size_t)elemvalue.value.uint_;
							}
							else if (elemvalue.type == langbind::TypedFilterBase::Element::string_)
							{
								std::string paramname( elemvalue.value.string_.ptr, elemvalue.value.string_.size);
								m_paramidx = m_func->getParameterIndex( paramname);
/*[-]*/std::cout << "DEBUG select parameter " << paramname << std::endl;
							}
							else
							{
								throw std::runtime_error( "unexpected node type (function parameter name or index expected)");
							}
							const Function::Parameter* param = m_func->getParameter( m_paramidx);
							m_recdepht = 0;
							const RecordInfo* recinfo = param->recinfomap.empty()?0:param->recinfomap.at(recordInfoKey(0,0)).get();
							State followstate;
							if (elemtype==langbind::InputFilter::Attribute)
							{
								followstate = MapAttrParam;
								if (recinfo) throw std::runtime_error( "unexpected attribute selects sub structure in record");
							}
							else
							{
								followstate = MapParam;
								if (recinfo) m_recdepht++;
							}
							m_paramvalue = initParameter( m_paramidx, &m_parameter[ m_paramidx], recinfo);
							m_selectedtype = m_paramvalue->vt;
							m_statestack.push( StateStackElem( followstate, recinfo));
							break;
						}
						case langbind::InputFilter::CloseTag:
							m_statestack.pop();
							break;

						case langbind::InputFilter::Value:
							throw std::runtime_error( "unexpected content token (open element expected specifying the parameter by name or index)");
					}
					break;

				case MapAttrParam:
				case MapParam:
					switch (elemtype)
					{
						case langbind::InputFilter::OpenTag:
						case langbind::InputFilter::Attribute:
						{
							const RecordInfo* recinfo = m_statestack.top().m_recinfo;
							std::string name( elemvalue.tostring());
							comauto::RecordInfo::VariableDescriptor descr;

							if (m_statestack.top().m_state == MapAttrParam) throw std::runtime_error( "unexpected attribute name (attribute value expected)");

							if (!recinfo) throw std::runtime_error( std::string( "atomic value expected instead of identifier '") + name + "'");
/*[-]*/std::cout << "DEBUG select variable " << name << std::endl;
							if (!recinfo->getVariableDescriptor( name, descr)) throw std::runtime_error( std::string( "structure element not defined '") + name + "'");
							m_selectedtype = descr.type;
							std::size_t absofs = m_statestack.top().m_ofs + descr.ofs;
							const Function::Parameter* param = m_func->getParameter( m_paramidx);

							RecordInfoMap::const_iterator ri = param->recinfomap.find( comauto::recordInfoKey( m_recdepht, absofs));
							const RecordInfo* followrecinfo = (ri == param->recinfomap.end())?0:ri->second.get();

							State followstate;
							if (elemtype==langbind::InputFilter::Attribute)
							{
								followstate = MapAttrParam;
								if (followrecinfo) throw std::runtime_error( "unexpected attribute selects sub structure in record");
							}
							else
							{
								followstate = MapParam;
								if (followrecinfo) m_recdepht++;
							}
							m_statestack.push( StateStackElem( followstate, followrecinfo, absofs, descr.varnum));
							break;
						}

						case langbind::InputFilter::CloseTag:
							if (state == MapAttrParam) throw std::runtime_error( "unexpected close tag (attribute value expected)");
							if (m_statestack.top().m_recinfo) --m_recdepht;
							m_statestack.pop();
							break;

						case langbind::InputFilter::Value:
						{
							const RecordInfo* recinfo = m_statestack.top().m_recinfo;
							if (!recinfo)
							{
								std::size_t ofs = m_statestack.top().m_ofs;
								assignValue( m_paramvalue, ofs, m_selectedtype, elemvalue);
								break;
							}
							else if (state == MapParam)
							{
								/// ... implicit select content variable '_'
								comauto::RecordInfo::VariableDescriptor descr;
								if (!recinfo->getVariableDescriptor( "_", descr)) throw std::runtime_error( std::string( "structure element for content '_' selected but not defined"));
								m_selectedtype = descr.type;
								std::size_t absofs = m_statestack.top().m_ofs + descr.ofs;
								const Function::Parameter* param = m_func->getParameter( m_paramidx);
								RecordInfoMap::const_iterator ri = param->recinfomap.find( comauto::recordInfoKey( m_recdepht, absofs));
								const RecordInfo* followrecinfo = (ri == param->recinfomap.end())?0:ri->second.get();
								if (followrecinfo) throw std::runtime_error( std::string( "structure element for content '_' is not defined as an atomic type"));
								m_statestack.push( StateStackElem( MapAttrParam, 0, absofs, descr.varnum));
							}
							std::size_t ofs = m_statestack.top().m_ofs;
							if (!comauto::isAtomicType( m_selectedtype)) throw std::runtime_error( "atomic value assigned to non atomic type");
							assignValue( m_paramvalue, ofs, m_selectedtype, elemvalue);
							if (m_statestack.top().m_recinfo) --m_recdepht;
							m_statestack.pop();
						}
					}
					break;
			}
		}
		switch (m_input->state())
		{
			case langbind::InputFilter::Error: return false;
			case langbind::InputFilter::EndOfMessage: return false;
			case langbind::InputFilter::Open: break;
		}
		if ((m_flags & serialize::Context::ValidateAttributes) != 0)
		{
			// ... don't know yet what to do here, because attributes are not defined for .NET functions ...
		}
		if ((m_flags & serialize::Context::ValidateInitialization) != 0)
		{
			std::size_t ii = 0,nn = m_func->nofParameter();
			for (; ii < nn; ++ii)
			{
				if (m_parameter[ii].vt == VT_EMPTY)
				{
					const Function::Parameter* param = m_func->getParameter( ii);
					if ((param->typedesc->vt & VT_ARRAY) == VT_ARRAY || (param->typedesc->vt & VT_VECTOR) == VT_VECTOR) continue;

					throw std::runtime_error( std::string( "missing parameter '") + param->name + "'");
				}
			}
		}
		VARIANT resdata = m_func->clr()->call( m_func->assemblyname(), m_func->classname(), m_func->methodname(), m_func->nofParameter(), m_parameter);
		m_result.reset( new FunctionResult( m_func->getReturnType()->typeinfo, resdata));
		return true;
	}
	catch (const std::runtime_error& e)
	{
		throw std::runtime_error( errorMessage( e.what()));
	}
}

langbind::TypedInputFilterR comauto::FunctionClosure::result() const
{
	return m_result;
}

std::string comauto::FunctionClosure::StateStack::variablepath() const
{
	std::string rt;
	std::vector<StateStackElem>::const_iterator si = m_impl.begin(), sn, se = m_impl.end();
	for (; si != se; ++si)
	{
		if (si->m_recinfo)
		{
			sn = si;
			++sn;
			if (sn != se && sn->m_variableidx > 0)
			{
				VARDESC* var;
				ITypeInfo* typeinfo = const_cast<ITypeInfo*>( si->m_recinfo->typeinfo());
				WRAP( typeinfo->GetVarDesc( sn->m_variableidx, &var))
				if (!rt.empty()) rt.push_back( '.');
				rt.append( comauto::variablename( typeinfo, var));
				typeinfo->ReleaseVarDesc( var);
			}
		}
	}
	return rt;
}

std::string comauto::FunctionClosure::errorMessage( const std::string& msg) const
{
	std::ostringstream rt;
	if (m_statestack.empty())
	{
		rt << "error";
	}
	else
	{
		switch (m_statestack.top().m_state)
		{
			case CheckParam:
				rt << "error mapping parameters";
				break;
			case MapParam:
			case MapAttrParam:
			{
				const comauto::Function::Parameter* param = m_func->getParameter( m_paramidx);
				std::string vp = m_statestack.variablepath();

				rt << "error mapping parameter '" << param->name << "'";
				if (!vp.empty())
				{
					rt << " (" << param->name << "." << vp << ")";
				}
			}
		}
	}
	rt << " calling function '" << m_func->classname() << "." << m_func->methodname() << "': " << msg;
	return rt.str();
}


FunctionResult::StackElem::StackElem( const StackElem& o)
	:state(o.state),typeinfo(o.typeinfo),typeattr(0),vardesc(0),ofs(o.ofs),idx(o.idx)
{
	if (typeinfo)
	{
		WRAP( typeinfo->GetTypeAttr( &typeattr))
		if (o.vardesc)
		{
			WRAP( typeinfo->GetVarDesc( idx, &vardesc))
		}
	}
}

FunctionResult::StackElem::StackElem( State state_, CComPtr<ITypeInfo> typeinfo_, std::size_t ofs_)
	:state(state_),typeinfo(typeinfo_),typeattr(0),vardesc(0),ofs(ofs_),idx(0)
{
	if (typeinfo) WRAP( typeinfo->GetTypeAttr( &typeattr))
}

FunctionResult::StackElem::~StackElem()
{
	if (typeinfo)
	{
		if (typeattr) typeinfo->ReleaseTypeAttr( typeattr);
		if (vardesc) typeinfo->ReleaseVarDesc( vardesc);
	}
}

bool FunctionResult::getNext( ElementType& type, Element& element)
{
AGAIN:
	if (m_stk.empty()) return false;
	State state = m_stk.back().state;
	StackElem& cur = m_stk.back();	//< REMARK: 'cur' only valid till next m_stk.push_back()/pop_back(). Check that push/pop return or goto AGAIN !
	switch (state)
	{
		case Init:
			if (comauto::isAtomicType( m_data.vt) || comauto::isStringType( m_data.vt))
			{
				element = comauto::getAtomicElement( m_data, m_elembuf);
				type = Value;
				m_stk.pop_back();
				return true;
			}
			else if (!cur.typeinfo)
			{
				throw std::runtime_error( std::string( "cannot convert result type '") + comauto::typestr( m_data.vt)  + "'");
			}
			cur.state = VarOpen;
			/*no break here!*/

		case VarOpen:
		{
			if (!cur.typeinfo) throw std::logic_error( "illegal state in comauto::FunctionResult::getNext");

			if (cur.idx < cur.typeattr->cVars)
			{
				cur.typeinfo->GetVarDesc( cur.idx, &cur.vardesc);
				element = Element( m_elembuf = comauto::variablename( cur.typeinfo, cur.vardesc));
				type = OpenTag;
				cur.state = VarValue;
				return true;
			}
			else
			{
				m_stk.pop_back();
				goto AGAIN;
			}
			break;
		}
		case VarValue:
		{
			VARDESC* vardesc = cur.vardesc;
			VARTYPE vt = vardesc->elemdescVar.tdesc.vt;
			if (comauto::isAtomicType( vt) || comauto::isStringType( vt))
			{
				element = comauto::getAtomicElement( vt, (char*)m_data.pvRecord + cur.ofs + vardesc->oInst, m_elembuf);
				type = Value;
				cur.state = VarClose;
				return true;
			}
			else if (vt == VT_USERDEFINED)
			{
				CComPtr<ITypeInfo> rectypeinfo;
				WRAP( cur.typeinfo->GetRefTypeInfo( vardesc->elemdescVar.tdesc.hreftype, &rectypeinfo))
				std::size_t recofs = cur.ofs + vardesc->oInst;
				cur.state = VarClose;
				m_stk.push_back( StackElem( VarOpen, rectypeinfo, recofs));
				goto AGAIN;
			}
			break;
		}
		case VarClose:
		{
			cur.typeinfo->ReleaseVarDesc( cur.vardesc);
			cur.vardesc = 0;
			cur.idx++;
			cur.state = VarOpen;
			element = Element();
			type = CloseTag;
			return true;
		}
	}
	return false;
}

