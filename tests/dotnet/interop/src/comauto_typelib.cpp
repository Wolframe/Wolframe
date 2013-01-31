#include "comauto_typelib.hpp"
#include "comauto_utils.hpp"
#include "comauto_function.hpp"
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

using namespace _Wolframe;

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

// forward declaration for printAttributes(const char*,ITypeInfo*,TYPEATTR*,int)
static void printItem( std::ostream& out, ITypeInfo* typeinfo, int indentcnt=0);

static void printVar( std::ostream& out, ITypeInfo* typeinfo, VARDESC* var, int indentcnt=0)
{
	std::string name = comauto::variablename( typeinfo, var);
	std::string type = comauto::variabletype( typeinfo, var);
	out << std::string(indentcnt,'\t') << "VAR " << name << " :" << type << std::endl;
}

static void printAttributes( std::ostream& out, const char* title, ITypeInfo* typeinfo, TYPEATTR* typeattr, int indentcnt=0)
{
	BSTR name;
	WRAP( typeinfo->GetDocumentation( MEMBERID_NIL, &name, NULL, NULL, NULL))
	std::string namestr( comauto::utf8string(name));
	if (namestr != "IUnknown")
	{
		out << std::string( indentcnt, '\t') << title << " " << namestr << std::endl;
		unsigned short ii;

		for (ii = 0; ii < typeattr->cImplTypes; ++ii)
		{
			HREFTYPE hreftype;
			ITypeInfo* classtypeinfo;
			WRAP( typeinfo->GetRefTypeOfImplType( ii, &hreftype))
			WRAP( typeinfo->GetRefTypeInfo( hreftype, &classtypeinfo))
			printItem( out, classtypeinfo, indentcnt+1);
			classtypeinfo->Release();
		}
		for (ii = 0; ii < typeattr->cFuncs; ++ii)
		{
			comauto::Function func( 0, typeinfo, "", "", ii);
			if (!comauto::isCOMInterfaceMethod( func.methodname()))
			{
				out << std::string( indentcnt+1, '\t');
				func.print( out);
			}
		}
		for (ii = 0; ii < typeattr->cVars; ++ii)
		{
			VARDESC* var;
			WRAP( typeinfo->GetVarDesc( ii, &var))
			printVar( out, typeinfo, var, indentcnt+1);
			typeinfo->ReleaseVarDesc( var);
		}
	}
	::SysFreeString( name);
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
		printItem( out, typeinfo, indentcnt);
		typeinfo->Release();
	}
}

void comauto::TypeLib::print( std::ostream& out) const
{
	printItem( out, m_typelib);
}

static void findFunctions( ITypeInfo* typeinfo, std::vector<comauto::FunctionR>& funcs, comauto::CommonLanguageRuntime* clr, const std::string& assemblyname, const std::string& classname)
{
	TYPEATTR* typeattr;
	unsigned short ii;
	WRAP( typeinfo->GetTypeAttr( &typeattr))
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
			comauto::FunctionR func( new comauto::Function( clr, typeinfo, assemblyname, classname, ii));
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
		BSTR name;
		WRAP( typeinfo->GetDocumentation( MEMBERID_NIL, &name, NULL, NULL, NULL))
		if (subclassname != "IUnknown")
		{
			subclassname.clear();
			subclassname.append( comauto::utf8string(name));
		}
		::SysFreeString( name);
	}
	for (ii = 0; ii < typeattr->cImplTypes; ++ii)
	{
		HREFTYPE hreftype;
		ITypeInfo* classtypeinfo;
		WRAP( typeinfo->GetRefTypeOfImplType( ii, &hreftype))
		WRAP( typeinfo->GetRefTypeInfo( hreftype, &classtypeinfo))
		findFunctions( classtypeinfo, funcs, clr, assemblyname, subclassname);
		classtypeinfo->Release();
	}
	typeinfo->ReleaseTypeAttr( typeattr);
}


std::vector<comauto::FunctionR> comauto::TypeLib::loadFunctions( comauto::CommonLanguageRuntime* clr, const std::string& assemblyname)
{
	std::vector<comauto::FunctionR> rt;
	ITypeInfo* typeinfo;

	for (UINT ii = 0; ii < m_typelib->GetTypeInfoCount(); ++ii)
	{
		WRAP( m_typelib->GetTypeInfo( ii, &typeinfo))
		findFunctions( typeinfo, rt, clr, assemblyname, "");
		typeinfo->Release();
	}
	return rt;
}

