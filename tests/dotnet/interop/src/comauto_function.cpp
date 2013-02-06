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
	FunctionResult( const ITypeInfo* typeinfo, const VARIANT& data_)
		:types::TypeSignature( "comauto::FunctionResult", __LINE__)
	{
		std::memset( &m_data, 0, sizeof(m_data));
		m_data.vt = VT_EMPTY;
		WRAP( comauto::wrapVariantCopy( &m_data, &data_))
		m_stk.push_back( StackElem( Init, const_cast<ITypeInfo*>(typeinfo), 0));
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
		ITypeInfo* typeinfo;
		TYPEATTR* typeattr;
		VARDESC* vardesc;
		std::size_t ofs;
		unsigned short idx;

		StackElem( const StackElem& o);
		StackElem( State state_, ITypeInfo* typeinfo_, std::size_t ofs_);
		~StackElem();
	};
	std::vector<StackElem> m_stk;
	std::string m_elembuf;
	VARIANT m_data;
};
}//anonymous namespace


static void findFunctions( const comauto::TypeLib* typelib, const ITypeInfo* typeinfo, std::vector<comauto::FunctionR>& funcs, comauto::CommonLanguageRuntime* clr, const std::string& assemblyname, const std::string& classname)
{
	TYPEATTR* typeattr = 0;
	ITypeInfo* classtypeinfo = 0;
	try
	{
		unsigned short ii;
		WRAP( const_cast<ITypeInfo*>(typeinfo)->GetTypeAttr( &typeattr))
		if ((typeattr->typekind == TKIND_DISPATCH || typeattr->typekind == TKIND_RECORD) && classname.empty())
		{
			return; //no follow on toplevel interface or POD data structure declaration
		}
		if (typeattr->cFuncs)
		{
			if (classname.empty())
			{
				throw std::runtime_error( "functions found outside class context");
			}
			for (ii = 0; ii < typeattr->cFuncs; ++ii)
			{
				comauto::FunctionR func( new comauto::Function( clr, typelib, typeinfo, assemblyname, classname, ii));
				if (!comauto::isCOMInterfaceMethod( func->methodname()))
				{
					funcs.push_back( func);
				}
			}
		}
		std::string subclassname( classname);
		if (typeattr->typekind == TKIND_COCLASS)
		{
			if (!classname.empty()) throw std::runtime_error( "nested class definitions not supported");
			if (subclassname != "IUnknown")
			{
				subclassname.clear();
				subclassname.append( comauto::typestr( typeinfo));
			}
		}
		for (ii = 0; ii < typeattr->cImplTypes; ++ii)
		{
			HREFTYPE hreftype;
			WRAP( const_cast<ITypeInfo*>(typeinfo)->GetRefTypeOfImplType( ii, &hreftype))
			WRAP( const_cast<ITypeInfo*>(typeinfo)->GetRefTypeInfo( hreftype, &classtypeinfo))
			findFunctions( typelib, classtypeinfo, funcs, clr, assemblyname, subclassname);
			classtypeinfo->Release();
			classtypeinfo = 0;
		}
		const_cast<ITypeInfo*>(typeinfo)->ReleaseTypeAttr( typeattr);
		typeattr = 0;
	}
	catch (const std::runtime_error& e)
	{
		if (typeattr) const_cast<ITypeInfo*>(typeinfo)->ReleaseTypeAttr( typeattr);
		if (classtypeinfo) classtypeinfo->Release();
		throw e;
	}
}

std::vector<comauto::FunctionR> comauto::loadFunctions( const comauto::TypeLib* typelib, comauto::CommonLanguageRuntime* clr, const std::string& assemblyname)
{
	std::vector<comauto::FunctionR> rt;
	ITypeInfo* typeinfo = 0;
	try
	{
		ITypeLib* tl = const_cast<ITypeLib*>( typelib->typelib());
		UINT ii = 0, nn = tl->GetTypeInfoCount();

		for (; ii < nn; ++ii)
		{
			WRAP( tl->GetTypeInfo( ii, &typeinfo))
			typelib->getRecordInfo( typeinfo);
			findFunctions( typelib, typeinfo, rt, clr, assemblyname, "");
			typeinfo->Release();
			typeinfo = 0;
		}
	}
	catch (const std::runtime_error& e)
	{
		if (typeinfo) typeinfo->Release();
		throw e;
	}
	return rt;
}


comauto::Function::Parameter::Parameter( const Parameter& o)
	:name(o.name),addrMode(o.addrMode),typedesc(o.typedesc),typeinfo(o.typeinfo)
{
	if (typeinfo) const_cast<ITypeInfo*>(typeinfo)->AddRef();
}

comauto::Function::Parameter::Parameter( const std::string& name_, const TYPEDESC* typedesc_, const ITypeInfo* typeinfo_, AddrMode addrMode_)
	:name(name_),addrMode(addrMode_),typedesc(typedesc_),typeinfo(typeinfo_)
{
	if (typeinfo) const_cast<ITypeInfo*>(typeinfo)->AddRef();
}

comauto::Function::Parameter::~Parameter()
{
	if (typeinfo) const_cast<ITypeInfo*>(typeinfo)->Release();
}

comauto::Function::ReturnType::ReturnType( const ReturnType& o)
	:typedesc(o.typedesc),typeinfo(o.typeinfo)
{
	if (typeinfo) const_cast<ITypeInfo*>(typeinfo)->AddRef();
}

comauto::Function::ReturnType::ReturnType( const TYPEDESC* typedesc_, const ITypeInfo* typeinfo_)
	:typedesc(typedesc_),typeinfo(typeinfo_)
{
	if (typeinfo) const_cast<ITypeInfo*>(typeinfo)->AddRef();
}

comauto::Function::ReturnType::ReturnType( const TYPEDESC* typedesc_)
	:typedesc(typedesc_),typeinfo(0){}

comauto::Function::ReturnType::~ReturnType()
{
	if (typeinfo) const_cast<ITypeInfo*>(typeinfo)->Release();
}


comauto::Function::Function( comauto::CommonLanguageRuntime* clr_, const comauto::TypeLib* typelib_, const ITypeInfo* typeinfo_, const std::string& assemblyname_, const std::string& classname_, unsigned short fidx)
	:m_clr(clr_)
	,m_typelib(typelib_)
	,m_typeinfo(typeinfo_)
	,m_funcdesc(0)
	,m_assemblyname(assemblyname_)
	,m_classname(classname_)
{
	if (m_typeinfo) const_cast<ITypeInfo*>(m_typeinfo)->AddRef();
	WRAP( const_cast<ITypeInfo*>(m_typeinfo)->GetFuncDesc( fidx, &m_funcdesc))
	struct Local	//exception safe memory allocation of local variables
	{
		BSTR* pnames;
		UINT size;
		ITypeInfo* rectypeinfo;

		Local()	:pnames(0),rectypeinfo(0){}
		~Local()
		{
			if (pnames)
			{
				for (UINT ii=0; ii<size; ++ii) if (pnames[ii]) ::SysFreeString( pnames[ii]);
				delete [] pnames;
			}
			if (rectypeinfo) rectypeinfo->Release();
		}
	};
	Local local;
	local.pnames = new BSTR[ local.size = m_funcdesc->cParams+1];
	UINT ii,nn;
	WRAP( const_cast<ITypeInfo*>(m_typeinfo)->GetNames( m_funcdesc->memid, local.pnames, m_funcdesc->cParams+1, &nn))

	m_methodname = comauto::utf8string( local.pnames[0]);
/*[-]*/if (m_methodname == "Sum")
/*[-]*/{
/*[-]*/	std::cout << "HALLY GALLY" << std::endl;
/*[-]*/}
	for (ii=1; ii<nn; ++ii)
	{
		const TYPEDESC* td = &m_funcdesc->lprgelemdescParam[ii-1].tdesc;
		if (td->vt == VT_USERDEFINED)
		{
			if (local.rectypeinfo) local.rectypeinfo->Release();
			local.rectypeinfo = 0;
			WRAP( const_cast<ITypeInfo*>(m_typeinfo)->GetRefTypeInfo( td->hreftype, &local.rectypeinfo))
			Parameter param( comauto::utf8string( local.pnames[ii]), td, local.rectypeinfo, Parameter::Value);
			m_parameterlist.push_back( param);
		}
		else if (td->vt == VT_SAFEARRAY)
		{
			if (local.rectypeinfo) local.rectypeinfo->Release();
			local.rectypeinfo = 0;
			if (!comauto::isAtomicType(td->lptdesc->vt) && !comauto::isStringType(td->lptdesc->vt))
			{
				WRAP( const_cast<ITypeInfo*>(m_typeinfo)->GetRefTypeInfo( td->lptdesc->hreftype, &local.rectypeinfo))
			}
			Parameter param( comauto::utf8string( local.pnames[ii]), td->lptdesc, local.rectypeinfo, Parameter::Safearray);
			m_parameterlist.push_back( param);
		}
		else
		{
			m_parameterlist.push_back( Parameter( comauto::utf8string( local.pnames[ii]), td, 0, Parameter::Value));
		}
	}
	if (m_funcdesc->elemdescFunc.tdesc.vt == VT_USERDEFINED)
	{
		if (local.rectypeinfo) local.rectypeinfo->Release();
		local.rectypeinfo = 0;
		WRAP( const_cast<ITypeInfo*>(m_typeinfo)->GetRefTypeInfo( m_funcdesc->elemdescFunc.tdesc.hreftype, &local.rectypeinfo))
		m_returntype = ReturnType( &m_funcdesc->elemdescFunc.tdesc, local.rectypeinfo);
	}
	else
	{
		m_returntype = ReturnType( &m_funcdesc->elemdescFunc.tdesc);
	}
}

comauto::Function::~Function()
{
	if (m_typeinfo)
	{
		const_cast<ITypeInfo*>(m_typeinfo)->ReleaseFuncDesc( m_funcdesc);
		const_cast<ITypeInfo*>(m_typeinfo)->Release();
	}
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
		out << pi->name << " :" << ((pi->addrMode==Parameter::Safearray)?"[] ":"") << comauto::typestr( m_typeinfo, const_cast<TYPEDESC*>(pi->typedesc));
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
		,m_param(0)
		,m_paramidx(null_paramidx)
		,m_paramarray_idx(0){}

comauto::FunctionClosure::~FunctionClosure()
{
	if (m_param)
	{
		std::size_t pi = 0, pe = m_func->nofParameter();
		for (; pi != pe; ++pi)
		{
			comauto::wrapVariantClear( &m_param[pi]);
		}
		delete [] m_param;
	}
	std::size_t ii,nn;
	for (ii=0,nn=m_paramarray.size(); ii<nn;++ii)
	{
		comauto::wrapVariantClear( &m_paramarray[ii]);
	}
}

void comauto::FunctionClosure::init( const proc::ProcessorProvider* p, const langbind::TypedInputFilterR& i, serialize::Context::Flags f)
{
	m_provider = p;
	m_input = i;
	m_flags = f;
	std::size_t ii = 0,nn = m_func->nofParameter();
	if (m_param) delete [] m_param;
	m_param = 0;	//... because of exception safety (new VARIANT[] might fail)
	m_param = new VARIANT[ m_func->nofParameter()];
	for (; ii < nn; ++ii)
	{
		m_param[ii].vt = VT_EMPTY;
	}
	m_paramidx = 0;
	m_paramarray_idx = 0;
	for (ii=0,nn=m_paramarray.size(); ii<nn;++ii)
	{
		comauto::wrapVariantClear( &m_paramarray[ii]);
	}
	m_paramarray.clear();
}


bool comauto::FunctionClosure::call()
{
AGAIN:
	try
	{
		langbind::TypedFilterBase::ElementType elemtype;
		langbind::TypedFilterBase::Element elemvalue;

		if (m_paramclosure.get())
		{
			//... we are in a parameter initilization
			VARIANT paramvalue;
			paramvalue.vt = VT_EMPTY;
			if (!m_paramclosure->call( paramvalue))
			{
				if (m_input->state() == langbind::InputFilter::Open) throw std::runtime_error( "unexpected end of input");
				return false;
			}
			const Function::Parameter* paramdescr = m_func->getParameter( m_paramidx);
			if (m_param[ m_paramidx].vt != VT_EMPTY) throw std::runtime_error( std::string("duplicate definition of parameter '") + paramdescr->name + "'");

			switch (paramdescr->addrMode)
			{
				case Function::Parameter::Value:
				{
					m_param[ m_paramidx] = paramvalue;
					break;
				}
				case Function::Parameter::Safearray:
				{
					m_paramarray_idx = m_paramidx;
					m_paramarray.push_back( paramvalue);
					break;
				}
			}
			m_paramidx = null_paramidx;
			m_paramclosure.reset();
		}
		while (m_input->getNext( elemtype, elemvalue))
		{
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
					}
					else
					{
						throw std::runtime_error( "unexpected node type (function parameter name or index expected)");
					}
					if (!m_paramarray.empty() && m_paramidx != m_paramarray_idx)
					{
						// ... we were filling an parameter that is an array into a buffer and encounter another element. we create the array from the buffered elements.
						m_param[ m_paramarray_idx] = comauto::createVariantArray( m_paramarray[0].vt, m_paramarray);
						m_paramarray.clear();
						m_paramarray_idx = null_paramidx;
					}

					const Function::Parameter* param = m_func->getParameter( m_paramidx);

					if (elemtype==langbind::InputFilter::Attribute)
					{
						if (param->typeinfo)
						{
							throw std::runtime_error( "atomic parameter expected if passed as attribute");
						}
						else
						{
							m_paramclosure.reset( new TypeLib::AssignmentClosure( m_func->typelib(), m_input, param->typedesc->vt, true));
						}
					}
					else
					{
						if (param->typeinfo)
						{
							m_paramclosure.reset( new TypeLib::AssignmentClosure( m_func->typelib(), m_input, param->typeinfo));
						}
						else
						{
							m_paramclosure.reset( new TypeLib::AssignmentClosure( m_func->typelib(), m_input, param->typedesc->vt, false));
						}
					}
					goto AGAIN;
				}
				case langbind::InputFilter::CloseTag:
					throw std::runtime_error( "unexpected close tag (tags not balanced)");
					break;

				case langbind::InputFilter::Value:
					throw std::runtime_error( "unexpected content token (expected a parameter name or index)");
			}
			break;
		}
		switch (m_input->state())
		{
			case langbind::InputFilter::Error: return false;
			case langbind::InputFilter::EndOfMessage: return false;
			case langbind::InputFilter::Open: break;
		}
		if (!m_paramarray.empty())
		{
			// ... there are some array elements buffered. we create the array from the buffered elements.
			m_param[ m_paramarray_idx] = comauto::createVariantArray( m_paramarray[0].vt, m_paramarray);
			m_paramarray.clear();
			m_paramarray_idx = null_paramidx;
		}
		if ((m_flags & serialize::Context::ValidateAttributes) != 0)
		{
			// ... don't know yet what to do here, because attributes are not defined for .NET functions ...
		}
		std::size_t ii = 0,nn = m_func->nofParameter();
		for (; ii < nn; ++ii)
		{
			if (m_param[ii].vt == VT_EMPTY)
			{
				const Function::Parameter* param = m_func->getParameter( ii);

				if ((m_flags & serialize::Context::ValidateInitialization) != 0)
				{
					throw std::runtime_error( std::string( "missing parameter '") + param->name + "'");
				}
				switch (param->addrMode)
				{
					case Function::Parameter::Safearray:
						if (comauto::isAtomicType( param->typedesc->vt))
						{
							m_param[ m_paramarray_idx] = comauto::createVariantArray( param->typedesc->vt);
						}
						else if (param->typeinfo)
						{
							const IRecordInfo* recinfo = m_func->typelib()->getRecordInfo( param->typeinfo);
							if (!recinfo) throw std::runtime_error( std::string( "default initialization of parameter '") + param->name + "' failed (record info unknown)");
							m_param[ m_paramarray_idx] = comauto::createVariantArray( param->typedesc->vt);
						}
						else
						{
							throw std::runtime_error( std::string( "default initialization of parameter '") + param->name + "' failed");
						}
						break;
					case Function::Parameter::Value:
						if (comauto::isAtomicType( param->typedesc->vt))
						{
							m_param[ii] = comauto::createVariantType( langbind::TypedInputFilter::Element(), param->typedesc->vt);
						}
						else if (param->typeinfo)
						{
							const IRecordInfo* recinfo = m_func->typelib()->getRecordInfo( param->typeinfo);
							if (!recinfo) throw std::runtime_error( std::string( "default initialization of parameter '") + param->name + "' failed (record info unknown)");
							m_param[ii].pvRecord = const_cast<IRecordInfo*>(recinfo)->RecordCreate();
							m_param[ii].pRecInfo = const_cast<IRecordInfo*>(recinfo);
							m_param[ii].vt = param->typedesc->vt;
						}
						else
						{
							throw std::runtime_error( std::string( "default initialization of parameter '") + param->name + "' failed");
						}
						break;
				}
			}
		}
		VARIANT resdata = m_func->clr()->call( m_func->assemblyname(), m_func->classname(), m_func->methodname(), m_func->nofParameter(), m_param);
		m_result.reset( new FunctionResult( m_func->getReturnType()->typeinfo, resdata));
		return true;
	}
	catch (const std::runtime_error& e)
	{
		std::string funcname = m_func->classname() + "." +  m_func->methodname();
		if (m_paramclosure.get())
		{
			const Function::Parameter* paramdescr = m_func->getParameter( m_paramidx);
			throw std::runtime_error( std::string("error calling function '") + funcname + " in parameter '" + paramdescr->name + "' (" + m_paramclosure->variablepath() + "): " + e.what());
		}
		else
		{
			throw std::runtime_error( std::string("error calling function '") + funcname + "': " + e.what());
		}
	}
}

langbind::TypedInputFilterR comauto::FunctionClosure::result() const
{
	return m_result;
}

FunctionResult::StackElem::StackElem( const StackElem& o)
	:state(o.state),typeinfo(o.typeinfo),typeattr(0),vardesc(0),ofs(o.ofs),idx(o.idx)
{
	if (typeinfo)
	{
		typeinfo->AddRef();
		WRAP( typeinfo->GetTypeAttr( &typeattr))
		if (o.vardesc)
		{
			WRAP( typeinfo->GetVarDesc( idx, &vardesc))
		}
	}
}

FunctionResult::StackElem::StackElem( State state_, ITypeInfo* typeinfo_, std::size_t ofs_)
	:state(state_),typeinfo(typeinfo_),typeattr(0),vardesc(0),ofs(ofs_),idx(0)
{
	if (typeinfo)
	{
		typeinfo->AddRef();
		WRAP( typeinfo->GetTypeAttr( &typeattr))
	}
}

FunctionResult::StackElem::~StackElem()
{
	if (typeinfo)
	{
		if (typeattr) typeinfo->ReleaseTypeAttr( typeattr);
		if (vardesc) typeinfo->ReleaseVarDesc( vardesc);
		typeinfo->Release();
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

