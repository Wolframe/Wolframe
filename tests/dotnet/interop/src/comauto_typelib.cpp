#include "comauto_typelib.hpp"
#include "comauto_utils.hpp"
#include "comauto_record.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <cstring>
#include <oaidl.h>
#include <comdef.h>
#include <atlbase.h>
#include <atlcom.h>


comauto::TypeLib::TypeLib( const std::string& file)
	:m_typelib(0)
{
	try
	{
		std::wstring ws_type_lib_file = utf16string( file);
		WRAP( ::LoadTypeLibEx( ws_type_lib_file.c_str(), REGKIND_NONE, &m_typelib))
	}
	catch (const std::runtime_error& e)
	{
		if (m_typelib) m_typelib->Release();
		throw e;
	}

}

comauto::TypeLib::~TypeLib()
{
	if (m_typelib) m_typelib->Release();
}


// forward declaration for typestr(ITypeInfo*,TYPEDESC*)
static std::string structstring( ITypeInfo* typeinfo);

class Form
{
public:
	typedef std::map<std::string,Form> Map;

	Form( const std::string& v)
		:m_value(v){}
	Form( const Form& o)
		:m_map(o.m_map),m_value(o.m_value){}
	Form(){}
	Form& operator()( const std::string& name, const Form& value)
	{
		m_map[ name] = value;
		return *this;
	}
	const std::string& operator*()
	{
		return m_value;
	}
	const std::string& value()
	{
		return m_value;
	}
	const wchar_t* wcvalue() const
	{
		return std::wstring( m_value.begin(), m_value.end()).c_str();
	}
	const Form& operator[]( const std::string& name) const
	{
		Map::const_iterator fi = m_map.find( name);
		if (fi == m_map.end()) throw std::runtime_error( std::string("element '") + name + "' not found");
		return fi->second;
	}
	Form& operator[]( const std::string& name)
	{
		Map::iterator fi = m_map.find( name);
		if (fi == m_map.end()) throw std::runtime_error( std::string("element '") + name + "' not found");
		return fi->second;
	}
	Map::const_iterator begin() const	{return m_map.begin();}
	Map::const_iterator end() const		{return m_map.end();}
	Map::iterator begin()				{return m_map.begin();}
	Map::iterator end()					{return m_map.end();}

	bool atomic() const
	{
		return m_map.empty();
	}
	bool empty() const
	{
		return m_map.empty() && m_value.empty();
	}

private:
	Map m_map;
	std::string m_value;
};

static IRecordInfo* getRecordInfo( ITypeInfo *typeinfo)
{
	IRecordInfo* rt = 0;
	CComObject<IRecordInfo> *crt = NULL;
	CComPtr<IRecordInfo> ptmpri;
	//WRAP( CComObject<IRecordInfo>::CreateInstance ( &crt))
	//WRAP( crt->QueryInterface( &rt))
	return rt;
}

static VARIANT getComObj( const Form& form, ITypeInfo *typeinfo, TYPEDESC *typedesc)
{
	VARIANT rt;
	if (form.empty())
	{
		VariantClear(&rt);
		if (typedesc->vt != VT_BSTR)
		{
			WRAP( ::VariantChangeType( &rt, &rt, 0, typedesc->vt))
		}
		return rt;
	}
	else if (form.atomic())
	{
		rt.vt = VT_BSTR;
		rt.bstrVal = ::SysAllocString( form.wcvalue());
		if (typedesc->vt != VT_BSTR)
		{
			WRAP( ::VariantChangeType( &rt, &rt, 0, typedesc->vt))
		}
		return rt;
	}
	else if (typedesc->vt == VT_USERDEFINED)
	{
		ITypeInfo* rectypeinfo;
		//IRecordInfo* recinfo;
		TYPEATTR *recattr;
		WRAP( typeinfo->GetRefTypeInfo( typedesc->hreftype, &rectypeinfo))
		WRAP( rectypeinfo->GetTypeAttr( &recattr))
		if (recattr->typekind != TKIND_RECORD) throw std::runtime_error("Can only handle VT_USERDEFINED type of kind VT_RECORD (a POD structure with no methods)");
		rt.vt = VT_RECORD;
		rt.pvRecord = 0;
	}
	else
	{
		throw std::runtime_error("Can only handle atomic type or VT_USERDEFINED type of kind VT_RECORD (a POD structure with no methods)");
	}
}

static std::string typestr( ITypeInfo* typeinfo, TYPEDESC* ed) {
	std::string rt;
	VARTYPE vt = ed->vt;
	if ((vt & VT_BYREF) == VT_BYREF)
	{
		rt.append( "-> ");
		vt -= VT_BYREF;
	}
	if ((vt & VT_ARRAY) == VT_ARRAY)
	{
		rt.append( "[] ");
		vt -= VT_ARRAY;
	}
	if ((vt & VT_VECTOR) == VT_VECTOR)
	{
		rt.append( "[] ");
		vt -= VT_VECTOR;
	}
	if (vt == VT_PTR)
	{
		rt.append( "^");
		rt.append( typestr( typeinfo, ed->lptdesc));
		return rt;
	}
	if (vt == VT_SAFEARRAY)
	{
		rt.append( "[] ");
		rt.append( typestr( typeinfo, ed->lptdesc));
		return rt;
	}
	if (vt == VT_USERDEFINED)
	{
		ITypeInfo* rectypeinfo;
		TYPEATTR *recattr;
		WRAP( typeinfo->GetRefTypeInfo( ed->hreftype, &rectypeinfo))
		rt.append( "{");
		rt.append( structstring( rectypeinfo));
		WRAP( rectypeinfo->GetTypeAttr( &recattr))
		if (recattr->typekind != TKIND_RECORD) throw std::runtime_error("Can only handle VT_USERDEFINED type of kind VT_RECORD (a structure with no methods)");
		rt.append( "}");
		rectypeinfo->Release();
		return rt;
	}
	switch (vt)
	{
		case VT_EMPTY:	rt.append( "VT_EMPTY"); break;
		case VT_VOID:	rt.append( "VT_VOID"); break;
		case VT_NULL:	rt.append( "VT_NULL"); break;
		case VT_INT:	rt.append( "VT_INT"); break;
		case VT_I1:		rt.append( "VT_I1"); break;
		case VT_I2:		rt.append( "VT_I2"); break;
		case VT_I4:		rt.append( "VT_I4"); break;
		case VT_UINT:	rt.append( "VT_UINT"); break;
		case VT_UI1:	rt.append( "VT_UI1"); break;
		case VT_UI2:	rt.append( "VT_UI2"); break;
		case VT_UI4:	rt.append( "VT_UI4"); break;
		case VT_R4:		rt.append( "VT_R4"); break;
		case VT_R8:		rt.append( "VT_R8"); break;
		case VT_CY:		rt.append( "VT_CY"); break;
		case VT_DATE:	rt.append( "VT_DATE"); break;
		case VT_BSTR:	rt.append( "VT_BSTR"); break;
		case VT_DISPATCH:rt.append( "VT_DISPATCH"); break;
		case VT_ERROR:	rt.append( "VT_ERROR"); break;
		case VT_BOOL:	rt.append( "VT_BOOL"); break;
		case VT_VARIANT:rt.append( "VT_VARIANT"); break;
		case VT_DECIMAL:rt.append( "VT_DECIMAL"); break;
		case VT_RECORD:	rt.append( "VT_RECORD"); break;
		case VT_UNKNOWN:rt.append( "VT_UNKNOWN"); break;
		case VT_HRESULT:rt.append( "VT_HRESULT"); break;
		case VT_CARRAY:	rt.append( "VT_CARRAY"); break;
		case VT_LPSTR:	rt.append( "VT_LPSTR"); break;
		case VT_LPWSTR:	rt.append( "VT_LPWSTR"); break;
		case VT_BLOB:	rt.append( "VT_BLOB"); break;
		case VT_STREAM:	rt.append( "VT_STREAM"); break;
		case VT_STORAGE:	rt.append( "VT_STORAGE"); break;
		case VT_STREAMED_OBJECT:rt.append( "VT_STREAMED_OBJECT"); break;
		case VT_STORED_OBJECT:rt.append( "VT_STORED_OBJECT"); break;
		case VT_BLOB_OBJECT:rt.append( "VT_BLOB_OBJECT"); break;
		case VT_CF:		rt.append( "VT_CF"); break;
		case VT_CLSID:	rt.append( "VT_CLSID"); break;
		default:		rt.append( "[Unknown]"); break;
	}
	return rt;
}

static std::string structstring( ITypeInfo* typeinfo)
{
	std::ostringstream out;
	TYPEATTR* typeattr;
	WRAP( typeinfo->GetTypeAttr( &typeattr));
	for (unsigned short ii = 0; ii < typeattr->cVars; ++ii)
	{
		VARDESC* var;
		WRAP( typeinfo->GetVarDesc( ii, &var))
		BSTR varname;
		UINT nn;
		WRAP( typeinfo->GetNames( var->memid, &varname, 1, &nn))
		ELEMDESC ed = var->elemdescVar;
		if (ii > 0) out << ";";
		out << comauto::asciistr(varname) << ":" << typestr( typeinfo, &ed.tdesc);
		typeinfo->ReleaseVarDesc( var);
		::SysFreeString( varname);
	}
	return out.str();
}

// forward declaration for printAttributes(const char*,ITypeInfo*,TYPEATTR*,int) and printDispatchTypeInfo(ITypeInfo*,int)
static void printItem( std::ostream& out, ITypeInfo* typeinfo, int indentcnt=0);

static bool isCOMInterfaceMethod( const std::string& name)
{
	if (std::strcmp( name.c_str(), "AddRef") == 0) return true;
	if (std::strcmp( name.c_str(), "Release") == 0) return true;
	if (std::strcmp( name.c_str(), "GetTypeInfo") == 0) return true;
	if (std::strcmp( name.c_str(), "GetTypeInfoCount") == 0) return true;
	if (std::strcmp( name.c_str(), "GetIDsOfNames") == 0) return true;
	if (std::strcmp( name.c_str(), "Invoke") == 0) return true;
	if (std::strcmp( name.c_str(), "QueryInterface") == 0) return true;
	if (std::strcmp( name.c_str(), "ToString") == 0) return true;
	if (std::strcmp( name.c_str(), "Equals") == 0) return true;
	if (std::strcmp( name.c_str(), "GetType") == 0) return true;
	if (std::strcmp( name.c_str(), "GetHashCode") == 0) return true;
	return false;
}

static void printFunc( std::ostream& out, ITypeInfo* typeinfo, FUNCDESC* func, int indentcnt=0)
{
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
	local.pnames = new BSTR[ local.size = func->cParams+1];
	UINT ii,nn;
	WRAP( typeinfo->GetNames( func->memid, local.pnames, func->cParams+1, &nn))
	std::string funcname = comauto::asciistr(local.pnames[0]);
	if (!isCOMInterfaceMethod( funcname))
	{
		out << std::string(indentcnt,'\t') << "FUNCTION " << funcname << "( ";
		for (ii=1; ii<nn; ++ii)
		{
			if (ii>1) out << ", ";
			out << comauto::asciistr(local.pnames[ii]) << " :" << typestr( typeinfo, &func->lprgelemdescParam[ii-1].tdesc);
		}
		out << " ) :" << typestr( typeinfo, &func->elemdescFunc.tdesc) << std::endl;
	}
}

static void printVar( std::ostream& out, ITypeInfo* typeinfo, VARDESC* var, int indentcnt=0)
{
	BSTR varname;
	UINT nn;
	WRAP( typeinfo->GetNames( var->memid, &varname, 1, &nn))
	ELEMDESC ed = var->elemdescVar;
	out << std::string(indentcnt,'\t') << "VAR " << comauto::asciistr(varname) << " :" << typestr( typeinfo, &ed.tdesc) << std::endl;
	::SysFreeString( varname);
}

static void printAttributes( std::ostream& out, const char* title, ITypeInfo* typeinfo, TYPEATTR* typeattr, int indentcnt=0)
{
	BSTR name;
	WRAP( typeinfo->GetDocumentation( MEMBERID_NIL, &name, NULL, NULL, NULL))
	if (comauto::asciistr(name) != "IUnknown")
	{
		out << std::string( indentcnt, '\t') << title << " " << comauto::asciistr(name) << std::endl;
		unsigned short ii;

		for (ii = 0; ii < typeattr->cImplTypes; ++ii)
		{
			// interface
			BSTR interfaceName;
			HREFTYPE hreftype;
			ITypeInfo* classtypeinfo;
			WRAP( typeinfo->GetRefTypeOfImplType( ii, &hreftype))
			WRAP( typeinfo->GetRefTypeInfo( hreftype, &classtypeinfo))
			WRAP( classtypeinfo->GetDocumentation( MEMBERID_NIL, &interfaceName, NULL, NULL, NULL))
			printItem( out, classtypeinfo, indentcnt+1);
			classtypeinfo->Release();
		}
		for (ii = 0; ii < typeattr->cFuncs; ++ii)
		{
			// function
			FUNCDESC* func;
			WRAP( typeinfo->GetFuncDesc( ii, &func))
			printFunc( out, typeinfo, func, indentcnt+1);
			typeinfo->ReleaseFuncDesc( func);
		}
		for (ii = 0; ii < typeattr->cVars; ++ii)
		{
			// variable
			VARDESC* var;
			WRAP( typeinfo->GetVarDesc( ii, &var))
			printVar( out, typeinfo, var, indentcnt+1);
			typeinfo->ReleaseVarDesc( var);
		}
	}
	::SysFreeString( name);
}

static void printDispatchTypeInfo( std::ostream& out, ITypeInfo* typeinfo, int indentcnt=0)
{
	HREFTYPE hreftype_dispatch;
	ITypeInfo* dispatchtypeinfo;
	HRESULT hr = typeinfo->GetRefTypeOfImplType( -1, &hreftype_dispatch);
	if (hr == TYPE_E_ELEMENTNOTFOUND) return;
	if (hr != S_OK) throw std::logic_error( "invalid system call to GetRefTypeOfImplType");
	WRAP( typeinfo->GetRefTypeInfo( hreftype_dispatch, &dispatchtypeinfo))
	printItem( out, dispatchtypeinfo, indentcnt+1);
	dispatchtypeinfo->Release();
}

static void printItem( std::ostream& out, ITypeInfo* typeinfo, int indentcnt)
{
	std::string indent( indentcnt, '\t');
	TYPEATTR* typeattr;
	WRAP( typeinfo->GetTypeAttr( &typeattr))

	switch (typeattr->typekind)
	{
		case TKIND_ENUM:
		{
			out << indent << "ENUM " << std::endl;
			break;
		}
		case TKIND_RECORD:
		{
			printAttributes( out, "RECORD", typeinfo, typeattr, indentcnt);
			break;
		}
		case TKIND_MODULE:
		{
			out << indent << "MODULE " << std::endl;
			break;
		}
		case TKIND_INTERFACE:
		{
			printAttributes( out, "INTERFACE", typeinfo, typeattr, indentcnt);
			break;
		}
		case TKIND_DISPATCH:
		{
			printAttributes( out, "DISPATCH", typeinfo, typeattr, indentcnt);
			//printDispatchTypeInfo( typeinfo, indentcnt);
			break;
		}
		case TKIND_ALIAS:
		{
			out << indent << "ALIAS" << std::endl;
			ITypeInfo* aliastypeinfo;
			WRAP( typeinfo->GetRefTypeInfo( typeattr->tdescAlias.hreftype, &aliastypeinfo))
			printItem( out, aliastypeinfo, indentcnt+1);
			aliastypeinfo->Release();
			break;
		}
		case TKIND_UNION:
		{
			out << indent << "UNION " << std::endl;
			break;
		}
		case TKIND_COCLASS:
		{
			printAttributes( out, "COCLASS", typeinfo, typeattr, indentcnt);
			//printDispatchTypeInfo( typeinfo, indentcnt);
			break;
		}
	}
	typeinfo->ReleaseTypeAttr( typeattr);
}

static void printItem( std::ostream& out, ITypeLib* typelib, int indentcnt=0)
{
	ITypeInfo* typeinfo;

	for (UINT ii = 0; ii < typelib->GetTypeInfoCount(); ++ii)
	{
		WRAP( typelib->GetTypeInfo( ii, &typeinfo))
		printItem( out, typeinfo);
		typeinfo->Release();
	}
}

void comauto::TypeLib::print( std::ostream& out) const
{
	printItem( out, m_typelib);
}

