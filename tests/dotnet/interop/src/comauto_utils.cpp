#include "comauto_utils.hpp"
#include <iostream>
#include <sstream>

using namespace _Wolframe;

bool comauto::isCOMInterfaceMethod( const std::string& name)
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
std::string comauto::asciistr( BSTR str)
{
	std::wstring wsStr( str, ::SysStringLen( str));
	return std::string( wsStr.begin(), wsStr.end());
}

std::string comauto::asciistr( const TCHAR* str)
{
	std::wstring wsStr( str);
	return std::string( wsStr.begin(), wsStr.end());
}

std::wstring comauto::utf16string( const char* utf8ptr, std::size_t utf8size)
{
	std::wstring rt;
	int len = ::MultiByteToWideChar( CP_UTF8, 0, utf8ptr, utf8size, NULL, 0);
	if (len>1)
	{
		rt.resize( len);
		::MultiByteToWideChar( CP_UTF8, 0, utf8ptr, utf8size, (LPWSTR)(void*)rt.c_str(), len);
   }
   return rt;
}

std::wstring comauto::utf16string( const std::string& utf8str)
{
	return comauto::utf16string( utf8str.c_str(), utf8str.size());
}

std::string comauto::utf8string( const std::wstring& utf16str)
{
	std::string rt;
	int len = ::WideCharToMultiByte( CP_UTF8, 0, utf16str.c_str(), utf16str.size(), NULL, 0, NULL, NULL);
	if (len>0)
	{
		rt.resize( len);
		::WideCharToMultiByte( CP_UTF8, 0, utf16str.c_str(), utf16str.size(), (LPSTR)(void*)rt.c_str(), len, NULL, NULL);
   }
   return rt;
}

std::string comauto::utf8string( const BSTR& str)
{
	std::string rt;
	int strl = ::SysStringLen( str);
	int len = ::WideCharToMultiByte( CP_UTF8, 0, str, strl, NULL, 0, NULL, NULL);
	if (len>0)
	{
		rt.resize( len);
		::WideCharToMultiByte( CP_UTF8, 0, str, strl, (LPSTR)(void*)rt.c_str(), len, NULL, NULL);
   }
   return rt;
}

std::wstring comauto::tostring( VARIANT* v)
{
	VARIANT vcp;
	WRAP( ::VariantCopy( &vcp, v))
	if (v->vt != VT_BSTR)
	{
		WRAP( ::VariantChangeType( &vcp, &vcp, VARIANT_NOVALUEPROP, VT_BSTR))
	}
	std::wstring rt( vcp.bstrVal, ::SysStringLen( vcp.bstrVal));
	WRAP( ::VariantClear( &vcp))
	return rt;
}

std::wstring comauto::tostring( const _com_error& err)
{
	std::wstring rt;
	LPTSTR errstr = NULL;

	FormatMessage(
	   FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_IGNORE_INSERTS,  
	   NULL, err.Error(), MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&errstr, 0, NULL);

	if (errstr)
	{
	   rt.append( errstr);
	   ::LocalFree( errstr);
	}
	else
	{
		rt.append( err.ErrorMessage());
	}
	return rt;
}

std::string comauto::typestr( VARTYPE vt)
{
	std::string rt;
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

std::string comauto::typestr( ITypeInfo* typeinfo, const TYPEDESC* ed)
{
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
	rt.append( comauto::typestr( vt));
	return rt;
}

std::string comauto::structstring( ITypeInfo* typeinfo)
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
		out << comauto::utf8string(varname) << ":" << typestr( typeinfo, &ed.tdesc);
		typeinfo->ReleaseVarDesc( var);
		::SysFreeString( varname);
	}
	return out.str();
}

VARIANT comauto::createVariantType( bool val)
{
   VARIANT rt;
   rt.vt = VT_BOOL;
   rt.boolVal = val ? VARIANT_TRUE : VARIANT_FALSE;
   return rt;
}

template <int IntTypeSize>
struct IntegerConstructor
{
	template <typename Type>
	static VARIANT createUnsigned( Type val)
	{
		VARIANT rt;
		rt.vt = VT_UINT;
		rt.uintVal = val;
		return rt;
	}
	template <typename Type>
	static VARIANT createInteger( Type val)
	{
		VARIANT rt;
		rt.vt = VT_INT;
		rt.intVal = val;
		return rt;
	}
};

template <>
struct IntegerConstructor<2>
{
	template <typename Type>
	static VARIANT createUnsigned( Type val)
	{
		VARIANT rt;
		rt.vt = VT_UI2;
		rt.uiVal = val;
		return rt;
	}
	template <typename Type>
	static VARIANT createInteger( Type val)
	{
		VARIANT rt;
		rt.vt = VT_I2;
		rt.iVal = val;
		return rt;
	}
};

template <>
struct IntegerConstructor<4>
{
	template <typename Type>
	static VARIANT createUnsigned( Type val)
	{
		VARIANT rt;
		rt.vt = VT_I4;
		rt.lVal = val;
		return rt;
	}
	template <typename Type>
	static VARIANT createInteger( Type val)
	{
		VARIANT rt;
		rt.vt = VT_I4;
		rt.lVal = val;
		return rt;
	}
};

template <>
struct IntegerConstructor<8>
{
	template <typename Type>
	static VARIANT createUnsigned( Type val)
	{
		VARIANT rt;
		rt.vt = VT_I8;
		rt.hVal.QuadPart = val;
		return rt;
	}
	template <typename Type>
	static VARIANT createInteger( Type val)
	{
		VARIANT rt;
		rt.vt = VT_I8;
		rt.uhVal.QuadPart = val;
		return rt;
	}
};

VARIANT comauto::createVariantType( int val)
{
	return IntegerConstructor<sizeof(val)>::createInteger<int>( val);
}

VARIANT comauto::createVariantType( unsigned int val)
{
	VARIANT rt;
	rt.vt = VT_UINT;
    rt.uintVal = val;
	return rt;
}

VARIANT comauto::createVariantType( short val)
{
	VARIANT rt;
	rt.vt = VT_I2;
    rt.iVal = val;
	return rt;
}

VARIANT comauto::createVariantType( unsigned short val)
{
	VARIANT rt;
	rt.vt = VT_UI2;
    rt.uiVal = val;
	return rt;
}

VARIANT comauto::createVariantType( long val)
{
	VARIANT rt;
	rt.vt = VT_I8;
    rt.lVal = val;
	return rt;
}

VARIANT comauto::createVariantType( unsigned long val)
{
	VARIANT rt;
	rt.vt = VT_UI4;
    rt.ulVal = val;
	return rt;
}

VARIANT comauto::createVariantType( float val)
{
	VARIANT rt;
	rt.vt = VT_R4;
    rt.fltVal = val;
	return rt;
}

VARIANT comauto::createVariantType( double val)
{
	VARIANT rt;
	rt.vt = VT_R8;
    rt.dblVal = val;
	return rt;
}

VARIANT comauto::createVariantType( const std::wstring& val)
{
    VARIANT rt;
	rt.vt = VT_BSTR;
    rt.bstrVal = ::SysAllocString( bstr_t( val.c_str()));
	if (rt.bstrVal == NULL) throw std::bad_alloc();
	return rt;
}

VARIANT comauto::createVariantType( const char* val, std::size_t valsize)
{
    VARIANT rt;
	rt.vt = VT_BSTR;
	std::wstring wstr( comauto::utf16string( val, valsize));
    rt.bstrVal = ::SysAllocString( bstr_t( wstr.c_str()));
	if (rt.bstrVal == NULL) throw std::bad_alloc();
	return rt;
}

VARIANT comauto::createVariantType( const std::string& val)
{
	return comauto::createVariantType( val.c_str(), val.size());
}

unsigned char comauto::sizeofAtomicType( int vt)
{
	struct AtomicTypes
	{
		unsigned char ar[80];
		AtomicTypes()
		{
			std::memset( ar, 0, sizeof(ar));
			ar[ VT_I2] = 2;
			ar[ VT_I4] = 4;
			ar[ VT_R4] = 4;
			ar[ VT_R8] = 8;
			ar[ VT_CY] = sizeof(CY);
			ar[ VT_DATE] = sizeof(DATE);
			ar[ VT_BOOL] = sizeof(BOOL);
			ar[ VT_DECIMAL] = sizeof(DECIMAL);
			ar[ VT_I1] = 1;
			ar[ VT_UI1] = 1;
			ar[ VT_UI2] = 2;
			ar[ VT_UI4] = 4;
			ar[ VT_I8] = 8;
			ar[ VT_UI8] = 8;
			ar[ VT_INT] = sizeof(int);
			ar[ VT_UINT] = sizeof(unsigned int);
			ar[ VT_HRESULT] = sizeof(HRESULT);
		}

		char operator[]( int i) const
		{
			if (i >= 0 && i <= sizeof(ar)) return ar[i];
			return 0;
		}
	};
	static AtomicTypes at;
	return at[vt];
}

bool comauto::isAtomicType( int vt)
{
	return comauto::sizeofAtomicType(vt)!=0;
}

void* comauto::arithmeticTypeAddress( VARIANT* val)
{
	return (void*)((char*)val + 4*sizeof(short));
}

const void* comauto::arithmeticTypeAddress( const VARIANT* val)
{
	return (const void*)((const char*)val + 4*sizeof(short));
}

std::string comauto::variablename( ITypeInfo* typeinfo, VARDESC* vardesc)
{
	std::string rt;
	BSTR vv;
	UINT nn;
	WRAP( typeinfo->GetNames( vardesc->memid, &vv, 1, &nn))
	rt = comauto::utf8string(vv);
	::SysFreeString( vv);
	return rt;
}

std::string comauto::variabletype( ITypeInfo* typeinfo, VARDESC* vardesc)
{
	std::string rt;
	ELEMDESC ed = vardesc->elemdescVar;
	rt = comauto::typestr( typeinfo, &ed.tdesc);
	return rt;
}
