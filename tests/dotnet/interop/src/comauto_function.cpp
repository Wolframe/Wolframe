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
	FunctionResult( const comauto::TypeLib* typelib_, const ITypeInfo* typeinfo_, VARIANT data_, serialize::Context::Flags flags_)
		:types::TypeSignature( "comauto::FunctionResult", __LINE__)
		,m_typelib(typelib_)
		,m_flags(flags_)
	{
		m_stk.push_back( StackElem( "", m_typelib->getRecordInfo(typeinfo_), const_cast<ITypeInfo*>(typeinfo_), data_));
	}

	FunctionResult( const FunctionResult& o)
		:types::TypeSignature( "comauto::FunctionResult", __LINE__)
		,m_stk(o.m_stk)
		,m_elembuf(o.m_elembuf)
		,m_typelib(o.m_typelib)
	{}

	virtual ~FunctionResult()
	{}

	virtual TypedInputFilter* copy() const
	{
		return new FunctionResult( *this);
	}

	virtual bool getNext( ElementType& type, types::VariantConst& element);

private:
	enum State
	{
		VarOpen,
		VarClose
	};
	struct StackElem
	{
		State state;
		ITypeInfo* typeinfo;
		const IRecordInfo* recinfo;
		TYPEATTR* typeattr;
		VARIANT data;
		std::string name;
		std::size_t idx;

		StackElem( const StackElem& o);
		StackElem( const std::string& name_, const IRecordInfo* recinfo_, ITypeInfo* typeinfo_, VARIANT data_);
		~StackElem();
	};
	std::vector<StackElem> m_stk;
	std::string m_elembuf;
	const comauto::TypeLib* m_typelib;
	serialize::Context::Flags m_flags;
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
		,m_paramidx(null_paramidx){}

static void clearArrayParam( std::map<std::size_t,std::vector<VARIANT> >& ap)
{
	std::map<std::size_t,std::vector<VARIANT> >::iterator ai = ap.begin(), ae = ap.end();
	for (; ai != ae; ++ai)
	{
		std::vector<VARIANT>::iterator vi = ai->second.begin(), ve = ai->second.end();
		for (; vi != ve; ++vi) comauto::wrapVariantClear( &*vi);
	}
	ap.clear();
}

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
	clearArrayParam( m_arrayparam);
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
	clearArrayParam( m_arrayparam);
}


bool comauto::FunctionClosure::call()
{
AGAIN:
	try
	{
		langbind::FilterBase::ElementType elemtype;
		types::VariantConst elemvalue;

		if (m_paramclosure.get())
		{
			//... we are in a parameter initilization
			VARIANT paramvalue;
			paramvalue.vt = VT_EMPTY;
			try
			{
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
						m_arrayparam[ m_paramidx].push_back( paramvalue);
						paramvalue.vt = VT_EMPTY;
						break;
					}
				}
				m_paramidx = null_paramidx;
				m_paramclosure.reset();
			}
			catch (const std::runtime_error& e)
			{
				comauto::wrapVariantClear( &paramvalue);
				throw e;
			}
		}
		while (m_input->getNext( elemtype, elemvalue))
		{
			switch (elemtype)
			{
				case langbind::InputFilter::Attribute:
				case langbind::InputFilter::OpenTag:
				{
					if (elemvalue.type() == types::Variant::Int)
					{
						__int64 val = elemvalue.data().value.Int;
						if (val < 0 || (std::size_t)val >= m_func->nofParameter()) throw std::runtime_error( "function parameter index out of range");
						m_paramidx = (std::size_t)val;
					}
					else if (elemvalue.type() == types::Variant::UInt)
					{
						unsigned __int64 val = elemvalue.data().value.UInt;
						if ((std::size_t)val >= m_func->nofParameter()) throw std::runtime_error( "function parameter index out of range");
						m_paramidx = (std::size_t)val;
					}
					else if (elemvalue.type() == types::Variant::String)
					{
						std::string paramname( elemvalue.charptr(), elemvalue.charsize());
						m_paramidx = m_func->getParameterIndex( paramname);
					}
					else
					{
						throw std::runtime_error( "unexpected node type (function parameter name or index expected)");
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
		// create all array parameters:
		std::map<std::size_t,std::vector<VARIANT> >::iterator pi = m_arrayparam.begin(), pe = m_arrayparam.end();
		for (; pi != pe; ++pi)
		{
			const Function::Parameter* param = m_func->getParameter( pi->first);
			const IRecordInfo* recinfo = m_func->typelib()->getRecordInfo( param->typeinfo);
			m_param[ pi->first] = comauto::createVariantArray( pi->second[0].vt, recinfo, pi->second);
		}

		// function signature validation:
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
							const IRecordInfo* recinfo = m_func->typelib()->getRecordInfo( param->typeinfo);
							m_param[ ii] = comauto::createVariantArray( param->typedesc->vt, recinfo);
						}
						else if (param->typeinfo)
						{
							const IRecordInfo* recinfo = m_func->typelib()->getRecordInfo( param->typeinfo);
							if (!recinfo) throw std::runtime_error( std::string( "default initialization of parameter '") + param->name + "' failed (record info unknown)");
							m_param[ ii] = comauto::createVariantArray( param->typedesc->vt, recinfo);
						}
						else
						{
							throw std::runtime_error( std::string( "default initialization of parameter '") + param->name + "' failed");
						}
						break;
					case Function::Parameter::Value:
						if (comauto::isAtomicType( param->typedesc->vt))
						{
							m_param[ ii] = comauto::createVariantType( types::Variant(), param->typedesc->vt);
						}
						else if (param->typeinfo)
						{
							const IRecordInfo* recinfo = m_func->typelib()->getRecordInfo( param->typeinfo);
							if (!recinfo) throw std::runtime_error( std::string( "default initialization of parameter '") + param->name + "' failed (record info unknown)");
							m_param[ ii].pvRecord = const_cast<IRecordInfo*>(recinfo)->RecordCreate();
							m_param[ ii].pRecInfo = const_cast<IRecordInfo*>(recinfo);
							m_param[ ii].vt = param->typedesc->vt;
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
		m_result.reset( new FunctionResult( m_func->typelib(), m_func->getReturnType()->typeinfo, resdata, m_flags));
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
	:name(o.name),state(o.state),recinfo(o.recinfo),typeinfo(o.typeinfo),typeattr(o.typeattr),idx(o.idx)
{
	try
	{
		data.vt = VT_EMPTY;
		WRAP( comauto::wrapVariantCopy( &data, &o.data))
		if (typeinfo)
		{
			typeinfo->AddRef();
			WRAP( typeinfo->GetTypeAttr( &typeattr))
		}
	}
	catch (const std::runtime_error& e)
	{
		comauto::wrapVariantClear( &data);
		if (typeinfo)
		{
			if (typeattr) typeinfo->ReleaseTypeAttr( typeattr);
			typeinfo->Release();
		}
		throw e;
	}
}

FunctionResult::StackElem::StackElem( const std::string& name_, const IRecordInfo* recinfo_, ITypeInfo* typeinfo_, VARIANT data_)
	:name(name_),state(VarOpen),recinfo(recinfo_),typeinfo(typeinfo_),typeattr(0),data(data_),idx(0)
{
	try
	{
		if (typeinfo)
		{
			typeinfo->AddRef();
			WRAP( typeinfo->GetTypeAttr( &typeattr))
		}
	}
	catch (const std::runtime_error& e)
	{
		if (typeinfo)
		{
			if (typeattr) typeinfo->ReleaseTypeAttr( typeattr);
			typeinfo->Release();
		}
		throw e;
	}
}

FunctionResult::StackElem::~StackElem()
{
	comauto::wrapVariantClear( &data);
	if (typeinfo)
	{
		if (typeattr) typeinfo->ReleaseTypeAttr( typeattr);
		typeinfo->Release();
	}
}

bool FunctionResult::getNext( ElementType& type, types::VariantConst& element)
{
AGAIN:
	VARIANT data;
	data.vt = VT_EMPTY;
	VARDESC* vardesc = 0;
	ITypeInfo* reftypeinfo = 0;
	try
	{
		if (m_stk.empty()) return false;
		StackElem& cur = m_stk.back();	//< REMARK: 'cur' only valid till next m_stk.push_back()/pop_back(). Check that push/pop return or goto AGAIN !
		switch (cur.state)
		{
			case VarOpen:
			{
				if ((cur.data.vt & VT_ARRAY) == VT_ARRAY)
				{
					if (1 != cur.data.parray->cDims)
					{
						throw std::runtime_error( "cannont handle multi dimensional arrays");
					}
					if (cur.idx >= cur.data.parray->rgsabound->cElements)
					{
						m_stk.pop_back();
						goto AGAIN;
					}
					cur.state = VarClose;
					if (((int)m_flags & serialize::Context::SerializeWithIndices) != 0 || cur.name.empty())
					{
						element = types::VariantConst( cur.idx+1);
					}
					else
					{
						element = types::VariantConst( m_elembuf = cur.name);
					}
					type = OpenTag;
					LONG idx = cur.idx++;
					VARTYPE elemvt = cur.data.vt - VT_ARRAY;

					if (comauto::isAtomicType( elemvt))
					{
						data.vt = elemvt;
						WRAP( ::SafeArrayGetElement( cur.data.parray, &idx, const_cast<void*>(comauto::arithmeticTypeAddress( &data))))
						cur.state = VarClose;
						m_stk.push_back( StackElem( "", 0, 0, data));
						std::memset( &data, 0, sizeof(data));
						data.vt = VT_EMPTY;
					}
					else if (comauto::isStringType( elemvt))
					{
						std::memset( &data, 0, sizeof(data));
						data.vt = elemvt;
						switch (elemvt)
						{
							case VT_LPSTR:
								WRAP( ::SafeArrayGetElement( cur.data.parray, &idx, &V_LPSTR( const_cast<VARIANT*>(&data))))
								break;
							case VT_LPWSTR:
								WRAP( ::SafeArrayGetElement( cur.data.parray, &idx, &V_LPWSTR( const_cast<VARIANT*>(&data))))
								break;
							case VT_BSTR:
								WRAP( ::SafeArrayGetElement( cur.data.parray, &idx, V_BSTR( const_cast<VARIANT*>(&data))))
								break;
							default:
								throw std::logic_error("internal: unknown string type");
						}
						cur.state = VarClose;
						m_stk.push_back( StackElem( "", 0, 0, data));
						std::memset( &data, 0, sizeof(data));
						data.vt = VT_EMPTY;
					}
					else if (elemvt == VT_RECORD)
					{
						std::memset( &data, 0, sizeof(data));
						data.vt = elemvt;
						WRAP( ::SafeArrayGetRecordInfo( cur.data.parray, &data.pRecInfo))
						if (!data.pRecInfo) throw std::runtime_error( "cannot iterate on result structure without record info");
						data.pvRecord = data.pRecInfo->RecordCreate();
						WRAP( ::SafeArrayGetElement( cur.data.parray, &idx, const_cast<void*>(data.pvRecord)))
						data.pRecInfo->GetTypeInfo( &reftypeinfo);
						cur.state = VarClose;
						m_stk.push_back( StackElem( "", data.pRecInfo, reftypeinfo, data));
						std::memset( &data, 0, sizeof(data));
						data.vt = VT_EMPTY;
						reftypeinfo->Release();
						reftypeinfo = 0;
					}
					else
					{
						throw std::runtime_error(std::string("cannot handle this array element type in result: '") + comauto::typestr(elemvt));
					}
					return true;
				}
				else if (comauto::isAtomicType( cur.data.vt) || comauto::isStringType( cur.data.vt))
				{
					element = comauto::getAtomicElement( cur.data, m_elembuf);
					type = Value;
					m_stk.pop_back();
					return true;
				}
				else if (cur.data.vt == VT_RECORD)
				{
					LONG idx = cur.idx++;
					cur.state = VarClose;

					if (idx >= cur.typeattr->cVars)
					{
						m_stk.pop_back();
						goto AGAIN;
					}
					WRAP( cur.typeinfo->GetVarDesc( idx, &vardesc))
					std::wstring varname( comauto::variablename_utf16( cur.typeinfo, vardesc));
					cur.typeinfo->ReleaseVarDesc( vardesc);
					vardesc = 0;

					const_cast<IRecordInfo*>(cur.recinfo)->GetField( cur.data.pvRecord, varname.c_str(), &data);
					if ((data.vt & VT_ARRAY) == VT_ARRAY)
					{
						bool rt = false;
						std::string elemname;
						if (((int)m_flags & serialize::Context::SerializeWithIndices) != 0)
						{
							type = OpenTag;
							element = types::VariantConst( m_elembuf = comauto::utf8string( varname));
							cur.state = VarClose;
							rt = true;
						}
						else
						{
							elemname = comauto::utf8string(varname);
							cur.state = VarOpen;
						}
						m_stk.push_back( StackElem( elemname, 0, 0, data));
						std::memset( &data, 0, sizeof(data));
						data.vt = VT_EMPTY;
						if (rt) return true;
						goto AGAIN;
					}
					else if (data.vt == VT_RECORD)
					{
						if (!data.pRecInfo || !data.pvRecord) 
						{
							throw std::runtime_error("cannot iterate through structure without record info");
						}
						WRAP( data.pRecInfo->GetTypeInfo( &reftypeinfo));
						m_stk.push_back( StackElem( "", data.pRecInfo, reftypeinfo, data));
						reftypeinfo->Release();
						reftypeinfo = 0;
						type = OpenTag;
						element = types::VariantConst( m_elembuf = comauto::utf8string( varname));
					}
					else if (comauto::isAtomicType( data.vt) || comauto::isStringType( data.vt))
					{
						m_stk.push_back( StackElem( "", 0, 0, data));
						type = OpenTag;
						element = types::VariantConst( m_elembuf = comauto::utf8string( varname));
					}
					else
					{
						throw std::runtime_error( std::string( "cannot handle this type in result structure '") + comauto::typestr(data.vt) + "'");
					}
					std::memset( &data, 0, sizeof(data));
					data.vt = VT_EMPTY;
					return true;
				}
				else
				{
					throw std::runtime_error( std::string("cannot handle this type of result '") + comauto::typestr(cur.data.vt) + "'");
				}
			}
			case VarClose:
			{
				cur.state = VarOpen;
				element = types::VariantConst();
				type = CloseTag;
				return true;
			}
		}
	}
	catch (const std::runtime_error& e)
	{
		comauto::wrapVariantClear( &data);
		if (vardesc) m_stk.back().typeinfo->ReleaseVarDesc( vardesc);
		if (reftypeinfo) reftypeinfo->Release();
		throw e;
	}
	return false;
}

