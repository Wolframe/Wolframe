#include "comauto_utils.hpp"

std::string comauto::asciistr( BSTR str)
{
	std::wstring wsStr( str, SysStringLen( str));
	return std::string( wsStr.begin(), wsStr.end());
}

std::string comauto::asciistr( const TCHAR* str)
{
	std::wstring wsStr( str);
	return std::string( wsStr.begin(), wsStr.end());
}

std::wstring comauto::utf16string( const std::string& utf8str)
{
	std::wstring rt;
	int len = ::MultiByteToWideChar( CP_UTF8, 0, utf8str.c_str(), utf8str.size(), NULL, 0);
	if (len>1)
	{
		rt.resize( len);
		::MultiByteToWideChar( CP_UTF8, 0, utf8str.c_str(), utf8str.size(), (LPWSTR)(void*)rt.c_str(), len);
   }
   return rt;
}

std::string comauto::utf8string( const std::wstring& utf16str)
{
	std::string rt;
	int len = ::WideCharToMultiByte( CP_UTF8, 0, utf16str.c_str(), utf16str.size(), NULL, 0, NULL, NULL);
	if (len>1)
	{
		rt.resize( len);
		::WideCharToMultiByte( CP_UTF8, 0, utf16str.c_str(), utf16str.size(), (LPSTR)(void*)rt.c_str(), len, NULL, NULL);
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

VARIANT comauto::createVariantType( const std::string& val)
{
    VARIANT rt;
	rt.vt = VT_BSTR;
	std::wstring wstr( comauto::utf16string( val));
    rt.bstrVal = ::SysAllocString( bstr_t( wstr.c_str()));
	if (rt.bstrVal == NULL) throw std::bad_alloc();
	return rt;
}

