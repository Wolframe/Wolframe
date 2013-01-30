#include "comauto_function.hpp"
#include "comauto_utils.hpp"
#include "comauto_clr.hpp"
#include "comauto_recordinfo.hpp"
#include <iostream>
#include <sstream>

using namespace _Wolframe;

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
			m_parameterlist.push_back( Parameter( comauto::utf8string( local.pnames[ii]), td, comauto::getRecordInfoMap( rectypeinfo)));
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
		,m_parameter(0){}

comauto::FunctionClosure::~FunctionClosure()
{
	if (m_parameter)
	{
		std::size_t pi = 0, pe = m_func->nofParameter();
		for (; pi != pe; ++pi)
		{
			if (m_parameter[pi].vt == VT_USERDEFINED)
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
			//.... create new Array element here
			throw std::runtime_error( "no support for arrays implemented yet");
		}
		throw std::runtime_error( std::string( "duplicate parameter '") + param->name + "'");
	}
	else
	{
		if (param->typedesc->vt == VT_USERDEFINED)
		{
			if (!recinfo) throw std::logic_error( "structure expected");
			val->vt = param->typedesc->vt;
			val->pRecInfo = const_cast<IRecordInfo*>(recinfo);
			val->pvRecord = val->pRecInfo->RecordCreate();
			return val;
		}
		else if (param->typedesc->vt == VT_BSTR)
		{
			if (recinfo) throw std::logic_error( "string instead of structure expected");
			val->vt = param->typedesc->vt;
			val->bstrVal = NULL;
			return val;
		}
		else if (comauto::isAtomicType(param->typedesc->vt))
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
		switch (elem.type)
		{
			case langbind::TypedFilterBase::Element::bool_:		elemval = comauto::createVariantType( elem.value.bool_); break;
			case langbind::TypedFilterBase::Element::double_:	elemval = comauto::createVariantType( elem.value.double_); break;
			case langbind::TypedFilterBase::Element::int_:		elemval = comauto::createVariantType( elem.value.int_); break;
			case langbind::TypedFilterBase::Element::uint_:		elemval = comauto::createVariantType( elem.value.uint_); break;
			case langbind::TypedFilterBase::Element::string_:	elemval = comauto::createVariantType( elem.value.string_.ptr, elem.value.string_.size); break;
			case langbind::TypedFilterBase::Element::blob_:		elemval = comauto::createVariantType( (const char*)elem.value.blob_.ptr, elem.value.blob_.size); break;
		}
		WRAP( ::VariantChangeType( &elemval, &elemval, VARIANT_NOVALUEPROP, dsttype))
		if (comauto::isAtomicType( dstrec->vt))
		{
			if (ofs) throw std::logic_error( "illegal operation (offset into atomic type)");
			::VariantCopy( dstrec, &elemval);
		}
		else if (dstrec->vt == VT_USERDEFINED || dstrec->vt == VT_RECORD) 
		{
			PVOID field = (PVOID)((char*)dstrec->pvRecord + ofs);
			if (dsttype == VT_BSTR)
			{
				if (*((BSTR*)field) != NULL) ::SysFreeString( *((BSTR*)field));
				*((BSTR*)field) = ::SysAllocString( elemval.bstrVal);
			}
			else
			{
				::memcpy( field, comauto::arithmeticTypeAddress( &elemval), comauto::sizeofAtomicType( dsttype));
			}
		}
		else
		{
			throw std::runtime_error( std::string("cannot handle this structure type: ") + comauto::typestr( dstrec->vt));
		}
	}
	catch (const std::runtime_error& e)
	{
		::VariantClear( &elemval);
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
								m_paramidx = m_func->getParameterIndex( std::string( elemvalue.value.string_.ptr, elemvalue.value.string_.size));
							}
							else
							{
								throw std::runtime_error( "unexpected node type (function parameter name or index expected)");
							}
							const Function::Parameter* param = m_func->getParameter( m_paramidx);
							State followstate = (elemtype==langbind::InputFilter::Attribute)?MapAttrParam:MapParam;
							const RecordInfo* recinfo = param->recinfomap.empty()?0:param->recinfomap.at(0).get();
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
							if (!recinfo->getVariableDescriptor( name, descr)) throw std::runtime_error( std::string( "structure element not defined '") + name + "'");
							m_selectedtype = descr.type;
							State followstate = (elemtype==langbind::InputFilter::Attribute)?MapAttrParam:MapParam;
							std::size_t absofs = m_statestack.top().m_ofs + descr.ofs;
							const Function::Parameter* param = m_func->getParameter( m_paramidx);
							const RecordInfo* followrecinfo = param->recinfomap.at( absofs).get();
							m_statestack.push( StateStackElem( followstate, followrecinfo, absofs, descr.varnum));
							break;
						}

						case langbind::InputFilter::CloseTag:
							if (state == MapAttrParam) throw std::runtime_error( "unexpected close tag (attribute value expected)");
							m_statestack.pop();
							break;

						case langbind::InputFilter::Value:
						{
							const RecordInfo* recinfo = m_statestack.top().m_recinfo;

							if (!recinfo)
							{
								assignValue( m_paramvalue, 0, m_selectedtype, elemvalue);
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
								const RecordInfo* followrecinfo = param->recinfomap.at(absofs).get();
								if (followrecinfo) throw std::runtime_error( std::string( "structure element for content '_' is not defined as an atomic type"));
								m_statestack.push( StateStackElem( MapAttrParam, 0, absofs, descr.varnum));
							}
							std::size_t ofs = m_statestack.top().m_ofs;
							if (!comauto::isAtomicType( m_selectedtype)) throw std::runtime_error( "atomic value assigned to non atomic type");
							assignValue( m_paramvalue, ofs, m_selectedtype, elemvalue);
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
		FunctionResult* resptr;
		m_result.reset( resptr = new FunctionResult( m_func->getReturnType()->typeinfo));

		*resptr->data() = m_func->clr()->call( m_func->assemblyname(), m_func->classname(), m_func->methodname(), m_func->nofParameter(), m_parameter);
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
			if (sn->m_variableidx > 0)
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


comauto::FunctionResult::StackElem::StackElem( const StackElem& o)
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

comauto::FunctionResult::StackElem::StackElem( State state_, CComPtr<ITypeInfo> typeinfo_, std::size_t ofs_)
	:state(state_),typeinfo(typeinfo_),typeattr(0),ofs(ofs_),idx(0)
{
	if (typeinfo) WRAP( typeinfo->GetTypeAttr( &typeattr))
}

comauto::FunctionResult::StackElem::~StackElem()
{
	if (typeinfo)
	{
		if (typeattr) typeinfo->ReleaseTypeAttr( typeattr);
		if (vardesc) typeinfo->ReleaseVarDesc( vardesc);
	}
}

static comauto::FunctionResult::Element getAtomicElement( VARTYPE vt, const void* ref, std::string& elembuf)
{
	typedef comauto::FunctionResult::Element Element;
	Element element;
	switch (vt)
	{
		case VT_I1:  element.type = Element::int_; element.value.int_ = *(const SHORT*)ref; break;
		case VT_I2:  element.type = Element::int_; element.value.int_ = *(const SHORT*)ref; break;
		case VT_I4:  element.type = Element::int_; element.value.int_ = *(const LONG*)ref; break;
		case VT_I8:  element.type = Element::int_; element.value.int_ = boost::lexical_cast<int>(*(const LONGLONG*)ref); break;
		case VT_INT: element.type = Element::int_; element.value.int_ = *(const INT*)ref; break;
		case VT_UI1:  element.type = Element::uint_; element.value.uint_ = *(const USHORT*)ref; break;
		case VT_UI2:  element.type = Element::uint_; element.value.uint_ = *(const USHORT*)ref; break;
		case VT_UI4:  element.type = Element::uint_; element.value.uint_ = *(const ULONG*)ref; break;
		case VT_UI8:  element.type = Element::uint_; element.value.uint_ = boost::lexical_cast<unsigned int>(*(const ULONGLONG*)ref); break;
		case VT_UINT: element.type = Element::uint_; element.value.uint_ = *(const UINT*)ref; break;
		case VT_R4:  element.type = Element::double_; element.value.double_ = *(const FLOAT*)ref; break;
		case VT_R8:  element.type = Element::double_; element.value.double_ = *(const DOUBLE*)ref; break;
		case VT_BOOL: element.type = Element::bool_; element.value.bool_ = *(const VARIANT_BOOL*)ref != VARIANT_FALSE; break;
		case VT_BSTR: 
			element.type = Element::string_; 
			elembuf = comauto::utf8string( *(const BSTR*)ref);
			element.type = Element::string_;
			element.value.string_.ptr = elembuf.c_str();
			element.value.string_.size = elembuf.size();
		default:
		{
			VARIANT elemorig;
			VARIANT elemcopy;
			try
			{
				elemorig.byref = const_cast<void*>(ref);
				elemorig.vt = vt | VT_BYREF;
				WRAP( ::VariantCopyInd( &elemcopy, &elemorig))
				WRAP( ::VariantChangeType( &elemcopy, &elemcopy, VARIANT_NOVALUEPROP, VT_BSTR))
				element.type = Element::string_; 
				elembuf = comauto::utf8string( elemcopy.bstrVal);
				element.type = Element::string_;
				element.value.string_.ptr = elembuf.c_str();
				element.value.string_.size = elembuf.size();
			}
			catch (const std::runtime_error& e)
			{
				::VariantClear( &elemorig);
				::VariantClear( &elemcopy);
				throw e;
			}
		}
	}
	return element;
}

static comauto::FunctionResult::Element getAtomicElement( const VARIANT& val, std::string& elembuf)
{
	return getAtomicElement( val.vt, comauto::arithmeticTypeAddress( &val), elembuf);
}

bool comauto::FunctionResult::getNext( ElementType& type, Element& element)
{
AGAIN:
	if (m_stk.empty()) return false;
	State state = m_stk.back().state;
	switch (state)
	{
		case Init:
			if (comauto::isAtomicType( m_data.vt ))
			{
				element = getAtomicElement( m_data, m_elembuf);
				type = Value;
				m_stk.pop_back();
				return true;
			}
			else if (m_data.vt == VT_BSTR)
			{
				element = getAtomicElement( m_data.vt, (char*)m_data.bstrVal, m_elembuf);
				type = Value;
				m_stk.pop_back();
				return true;
			}
			else if (!m_stk.back().typeinfo)
			{
				throw std::runtime_error( std::string( "cannot convert result type '") + comauto::typestr( m_data.vt)  + "'");
			}
			m_stk.back().state = VarOpen;
			/*no break here!*/

		case VarOpen:
			if (!m_stk.back().typeinfo) throw std::logic_error( "illegal state in comauto::FunctionResult::getNext");

			if (m_stk.back().idx < m_stk.back().typeattr->cVars)
			{
				m_stk.back().typeinfo->GetVarDesc( m_stk.back().idx, &m_stk.back().vardesc);
				element = Element( comauto::variablename( m_stk.back().typeinfo, m_stk.back().vardesc));
				type = OpenTag;
				m_stk.back().state = VarValue;
				return true;
			}
			else
			{
				m_stk.pop_back();
				goto AGAIN;
			}
			break;

		case VarValue:
		{
			VARDESC* vardesc = m_stk.back().vardesc;
			VARTYPE vt = vardesc->elemdescVar.tdesc.vt;
			if (comauto::isAtomicType( vt) || vt == VT_BSTR)
			{
				element = getAtomicElement( vt, (char*)m_data.pvRecord + m_stk.back().ofs, m_elembuf);
				type = Value;
				m_stk.back().state = VarClose;
				return true;
			}
			else if (vt == VT_USERDEFINED)
			{
				CComPtr<ITypeInfo> rectypeinfo;
				WRAP( m_stk.back().typeinfo->GetRefTypeInfo( vardesc->elemdescVar.tdesc.hreftype, &rectypeinfo))
				std::size_t recofs = m_stk.back().ofs + vardesc->oInst;
				m_stk.back().state = VarClose;
				m_stk.push_back( StackElem( VarOpen, rectypeinfo, recofs));
				goto AGAIN;
			}
			break;
		}
		case VarClose:
		{
			m_stk.back().typeinfo->ReleaseVarDesc( m_stk.back().vardesc);
			m_stk.back().vardesc = 0;
			m_stk.back().idx++;
			m_stk.back().state = VarOpen;
			element = Element();
			type = CloseTag;
			return true;
		}
	}
	return false;
}

