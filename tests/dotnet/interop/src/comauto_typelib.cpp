#include "comauto_typelib.hpp"
#include "comauto_utils.hpp"
#include "comauto_record.hpp"
#include "ddl_form.hpp"
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

void comauto::TypeLib::defineFunction( ITypeInfo* typeinfo, const std::string& classname, unsigned short fidx)
{
	FunctionR func( new Function( typeinfo, classname, fidx));
	std::string funcid( func->classname() + "." + func->methodname());
	if (m_funcmap.find( funcid) != m_funcmap.end()) throw std::runtime_error( std::string("duplicate definition of function '") + funcid + "'");
	m_funcmap[ funcid] = func;
}

static IRecordInfo* getRecordInfo( ITypeInfo *typeinfo)
{
	IRecordInfo* rt = 0;
	CComObject<IRecordInfo> *crt = NULL;
	CComPtr<IRecordInfo> ptmpri;
	return rt;
}

static VARIANT getComObj( const ddl::Form& form, ITypeInfo *typeinfo, TYPEDESC *typedesc)
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
		typeinfo->ReleaseTypeAttr( recattr);
	}
	else
	{
		throw std::runtime_error("Can only handle atomic type or VT_USERDEFINED type of kind VT_RECORD (a POD structure with no methods)");
	}
}

// forward declaration for printAttributes(const char*,ITypeInfo*,TYPEATTR*,int) and printDispatchTypeInfo(ITypeInfo*,int)
static void printItem( std::ostream& out, ITypeInfo* typeinfo, int indentcnt=0);

static void printVar( std::ostream& out, ITypeInfo* typeinfo, VARDESC* var, int indentcnt=0)
{
	BSTR varname;
	UINT nn;
	WRAP( typeinfo->GetNames( var->memid, &varname, 1, &nn))
	ELEMDESC ed = var->elemdescVar;
	out << std::string(indentcnt,'\t') << "VAR " << comauto::asciistr(varname) << " :" << comauto::typestr( typeinfo, &ed.tdesc) << std::endl;
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
			comauto::Function func( typeinfo, "", ii);
			if (!comauto::isCOMInterfaceMethod( func.methodname()))
			{
				out << std::string( indentcnt+1, '\t');
				func.print( out);
			}
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
	IRecordInfo* recinfo = 0;
	try
	{
		WRAP( ::GetRecordInfoFromTypeInfo( typeinfo, &recinfo))
		if (recinfo)
		{
			std::cout << "+++++++++++++++++++++++++++++++HALLY GALLY++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
		}
	}
	catch (...)
	{
	}
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
			WRAP( ::GetRecordInfoFromTypeInfo( typeinfo, &recinfo))
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

