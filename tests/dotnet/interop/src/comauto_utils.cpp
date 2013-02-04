#include "comauto_utils.hpp"
#include <iostream>
#include <sstream>
#pragma warning(disable:4996)

using namespace _Wolframe;

const char*& comauto::impl_V_LPSTR( const VARIANT* v)		{ return *(const char**)comauto::arithmeticTypeAddress(v); }
char*& comauto::impl_V_LPSTR( VARIANT* v)					{ return *(char**)comauto::arithmeticTypeAddress(v); }
const wchar_t*& comauto::impl_V_LPWSTR( const VARIANT* v)	{ return *(const wchar_t**)comauto::arithmeticTypeAddress(v); }
wchar_t*& comauto::impl_V_LPWSTR( VARIANT* v)				{ return *(wchar_t**)comauto::arithmeticTypeAddress(v); }

void* comauto::allocMem( std::size_t size)
{
	return ::CoTaskMemAlloc( size + 1000);
}

void comauto::freeMem( void* ptr)
{
	::CoTaskMemFree( ptr);
}

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

std::wstring comauto::utf16string( const char* utf8ptr, std::size_t utf8size)
{
	std::wstring rt;
	int len = ::MultiByteToWideChar( CP_UTF8, 0, utf8ptr, utf8size, NULL, 0);
	if (len>0)
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

std::string comauto::utf8string( LPCSTR str)
{
	std::wstring rt;
	std::size_t ii = 0;
	for (; str[ii]; ++ii) rt.push_back( (wchar_t)str[ii]);
	return comauto::utf8string( rt);
}

std::string comauto::utf8string( LPCWSTR str)
{
	return comauto::utf8string( std::wstring( str));
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

char* comauto::createLPSTR( const std::string& str)
{
	LPSTR rt;
	rt = (LPSTR)comauto::allocMem( str.size() + sizeof(char));
	if (rt == NULL) throw std::bad_alloc();
	::strcpy( rt, str.c_str());
	return rt;
}

char* comauto::createLPSTR( LPCSTR str)
{
	if (!str) return NULL;
	char* rt = (char*)comauto::allocMem( std::strlen(str) + sizeof(char));
	if (rt == NULL) throw std::bad_alloc();
	::strcpy( rt, str);
	return rt;
}

wchar_t* comauto::createLPWSTR( const std::wstring& str)
{
	wchar_t* rt = (wchar_t*)comauto::allocMem( (str.size()+1) * sizeof(wchar_t));
	if (rt == NULL) throw std::bad_alloc();
	::wcscpy( rt, str.c_str());
	return rt;
}

wchar_t* comauto::createLPWSTR( const wchar_t* str)
{
	if (!str) return NULL;
	wchar_t* rt = (wchar_t*)comauto::allocMem( (std::wcslen( str)+1) * sizeof(wchar_t));
	if (rt == NULL) throw std::bad_alloc();
	::wcscpy( rt, str);
	return rt;
}

BSTR comauto::createBSTR( LPCSTR str)
{
	std::wstring wstr = str?comauto::utf16string( str):L"";
	return ::SysAllocStringLen( wstr.c_str(), wstr.size());
}

BSTR comauto::createBSTR( LPCWSTR str)
{
	return ::SysAllocString( str?str:L"");
}

std::wstring comauto::tostring( VARIANT* v)
{
	VARIANT vcp;
	WRAP( comauto::wrapVariantCopy( &vcp, v))
	if (v->vt != VT_BSTR)
	{
		WRAP( comauto::wrapVariantChangeType( &vcp, &vcp, 0, VT_BSTR))
	}
	std::wstring rt( vcp.bstrVal, ::SysStringLen( vcp.bstrVal));
	WRAP( comauto::wrapVariantClear( &vcp))
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

std::string comauto::tostring( const EXCEPINFO& einfo)
{
	std::ostringstream rt;
	rt << "[" << std::hex << einfo.wCode << "/" << einfo.scode << "]: " << std::dec << comauto::utf8string( einfo.bstrDescription);
	return rt.str();
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

std::string comauto::typestr( const ITypeInfo* typeinfo, const TYPEDESC* ed)
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
		ITypeInfo* rectypeinfo = 0;
		TYPEATTR *recattr = 0;
		try
		{
			WRAP( const_cast<ITypeInfo*>(typeinfo)->GetRefTypeInfo( ed->hreftype, &rectypeinfo))
			rt.append( "{");
			rt.append( structstring( rectypeinfo));
			WRAP( rectypeinfo->GetTypeAttr( &recattr))
			if (recattr->typekind != TKIND_RECORD) throw std::runtime_error("Can only handle VT_USERDEFINED type of kind VT_RECORD (a structure with no methods)");
			rt.append( "}");
		}
		catch (const std::runtime_error& e)
		{
			if (rectypeinfo && recattr) rectypeinfo->ReleaseTypeAttr( recattr);
			if (rectypeinfo) rectypeinfo->Release();
			throw e;
		}
		rectypeinfo->ReleaseTypeAttr( recattr);
		rectypeinfo->Release();
		return rt;
	}
	rt.append( comauto::typestr( vt));
	return rt;
}

std::string comauto::typestr( const ITypeInfo* typeinfo)
{
	CComBSTR typeName;
	WRAP( const_cast<ITypeInfo*>(typeinfo)->GetDocumentation( MEMBERID_NIL, &typeName, NULL, NULL, NULL ))
	std::string rt( comauto::utf8string( typeName));
	::SysFreeString( typeName);
	return rt;
}

std::string comauto::structstring( const ITypeInfo* typeinfo)
{
	VARDESC* var = 0;
	TYPEATTR* typeattr = 0;
	BSTR varname = NULL;
	std::ostringstream out;

	try
	{
		WRAP( const_cast<ITypeInfo*>(typeinfo)->GetTypeAttr( &typeattr));
		for (unsigned short ii = 0; ii < typeattr->cVars; ++ii)
		{
			WRAP( const_cast<ITypeInfo*>(typeinfo)->GetVarDesc( ii, &var))
			BSTR varname;
			UINT nn;
			WRAP( const_cast<ITypeInfo*>(typeinfo)->GetNames( var->memid, &varname, 1, &nn))
			ELEMDESC ed = var->elemdescVar;
			if (ii > 0) out << ";";
			out << comauto::utf8string(varname) << ":" << typestr( typeinfo, &ed.tdesc);
			const_cast<ITypeInfo*>(typeinfo)->ReleaseVarDesc( var);
			var = 0;
			::SysFreeString( varname);
			varname = NULL;
		}
		const_cast<ITypeInfo*>(typeinfo)->ReleaseTypeAttr( typeattr);
		return out.str();
	}
	catch (const std::runtime_error& e)
	{
		if (var) const_cast<ITypeInfo*>(typeinfo)->ReleaseVarDesc( var);
		if (varname) ::SysFreeString( varname);
		if (typeattr) const_cast<ITypeInfo*>(typeinfo)->ReleaseTypeAttr( typeattr);
		throw e;
	}
}

VARIANT comauto::createVariantType( bool val)
{
   VARIANT rt;
   rt.vt = VT_BOOL;
   rt.boolVal = val ? VARIANT_TRUE : VARIANT_FALSE;
   return rt;
}

namespace {
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
}//anonymous namespace

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

VARIANT comauto::createVariantType( const char* val, std::size_t valsize, VARTYPE stringtype)
{
    VARIANT rt;
	rt.vt = stringtype;
	switch (stringtype)
	{
		case VT_BSTR:
		{
			std::wstring wstr( comauto::utf16string( val, valsize));
		    rt.bstrVal = ::SysAllocString( bstr_t( wstr.c_str()));
			if (rt.bstrVal == NULL) throw std::bad_alloc();
			break;
		}
		case VT_LPSTR:
		{
		    rt.pcVal = (LPSTR)comauto::allocMem( valsize+1);
			if (rt.pcVal == NULL) throw std::bad_alloc();
			::memcpy( rt.pcVal, val, valsize+1);
			break;
		}
		case VT_LPWSTR:
		{
			std::wstring wstr( comauto::utf16string( val, valsize));
			std::size_t mm = (wstr.size()+1) * sizeof(wchar_t);
		    rt.pcVal = (char*)(LPWSTR)comauto::allocMem( mm);
			if (rt.pcVal == NULL) throw std::bad_alloc();
			::memcpy( rt.pcVal, wstr.c_str(), mm);
			break;
		}
		default:
			throw std::logic_error( "selected unkown string type");
	}
	return rt;
}

VARIANT comauto::createVariantType( const std::string& val)
{
	return comauto::createVariantType( val.c_str(), val.size());
}

VARIANT comauto::createVariantType( const langbind::TypedInputFilter::Element& val)
{
	VARIANT rt;
	rt.vt = VT_EMPTY;
	switch (val.type)
	{
		case langbind::TypedFilterBase::Element::bool_:		rt = comauto::createVariantType( val.value.bool_); break;
		case langbind::TypedFilterBase::Element::double_:	rt = comauto::createVariantType( val.value.double_); break;
		case langbind::TypedFilterBase::Element::int_:		rt = comauto::createVariantType( val.value.int_); break;
		case langbind::TypedFilterBase::Element::uint_:		rt = comauto::createVariantType( val.value.uint_); break;
		case langbind::TypedFilterBase::Element::string_:	rt = comauto::createVariantType( val.value.string_.ptr, val.value.string_.size); break;
		case langbind::TypedFilterBase::Element::blob_:		rt = comauto::createVariantType( (const char*)val.value.blob_.ptr, val.value.blob_.size); break;
	}
	return rt;
}

VARIANT comauto::createVariantType( const langbind::TypedInputFilter::Element& val, VARTYPE dsttype)
{
	VARIANT rt;
	rt.vt = VT_EMPTY;
	switch (val.type)
	{
		case langbind::TypedFilterBase::Element::bool_:		rt = comauto::createVariantType( val.value.bool_); break;
		case langbind::TypedFilterBase::Element::double_:	rt = comauto::createVariantType( val.value.double_); break;
		case langbind::TypedFilterBase::Element::int_:		rt = comauto::createVariantType( val.value.int_); break;
		case langbind::TypedFilterBase::Element::uint_:		rt = comauto::createVariantType( val.value.uint_); break;
		case langbind::TypedFilterBase::Element::string_:	rt = comauto::createVariantType( val.value.string_.ptr, val.value.string_.size, (dsttype == VT_LPWSTR || dsttype == VT_LPSTR)?dsttype:VT_BSTR);
		case langbind::TypedFilterBase::Element::blob_:		rt = comauto::createVariantType( (const char*)val.value.blob_.ptr, val.value.blob_.size, (dsttype == VT_LPWSTR || dsttype == VT_LPSTR)?dsttype:VT_BSTR); break;
	}
	if (rt.vt != dsttype)
	{
		WRAP( comauto::wrapVariantChangeType( &rt, &rt, 0, dsttype))
	}
	return rt;
}

HRESULT comauto::wrapVariantCopy( VARIANT* pvargDest, const VARIANT* pvargSrc)
{
	std::size_t size;

	switch (pvargSrc->vt)
	{
		case VT_LPSTR:
			comauto::wrapVariantClear( pvargDest);
			size = std::strlen(V_LPSTR( pvargSrc)) + 1;
		    V_LPSTR( pvargDest) = (LPSTR)comauto::allocMem( size);
			if (V_LPSTR( pvargDest) == NULL) throw std::bad_alloc();
			::memcpy( V_LPSTR( pvargDest), V_LPSTR( pvargSrc), size);
			return S_OK;

		case VT_LPWSTR:
			comauto::wrapVariantClear( pvargDest);
			size = std::wcslen( V_LPWSTR( pvargSrc))+sizeof(wchar_t);
		    V_LPWSTR( pvargDest) = (LPWSTR)comauto::allocMem( size);
			if (V_LPWSTR( pvargDest) == NULL) throw std::bad_alloc();
			::memcpy( V_LPWSTR( pvargDest), V_LPWSTR( pvargSrc), size);
			return S_OK;

		default:
			return ::VariantCopy( pvargDest, pvargSrc);
	}
}

HRESULT comauto::wrapVariantClear( VARIANT* pvarg)
{
	switch (pvarg->vt)
	{
		case VT_LPSTR:
		    if (V_LPSTR( pvarg) != NULL) comauto::freeMem( V_LPSTR( pvarg));
			V_LPSTR( pvarg) = NULL;
			pvarg->vt = VT_EMPTY;
			return S_OK;

		case VT_LPWSTR:
		    if (V_LPWSTR( pvarg) != NULL) comauto::freeMem( V_LPWSTR( pvarg));
			V_LPWSTR( pvarg) = NULL;
			pvarg->vt = VT_EMPTY;
			return S_OK;

		default:
			return ::VariantClear( pvarg);
	}
}

HRESULT comauto::wrapVariantChangeType( VARIANT* pvargDest, const VARIANT* pvargSrc, unsigned short wFlags, VARTYPE vt)
{
	enum Conversion
	{
		C_COPY,
		C_LPSTR_LPWSTR,
		C_LPWSTR_LPSTR,
		C_BSTR_LPWSTR,
		C_BSTR_LPSTR,
		C_LPSTR_BSTR,
		C_LPWSTR_BSTR,
		C_OTHER
	};
	Conversion conversion = C_OTHER;
	switch (vt)
	{
		case VT_LPSTR:
			switch (pvargSrc->vt)
			{
				case VT_LPSTR:		conversion = C_COPY; break;
				case VT_LPWSTR:		conversion = C_LPSTR_LPWSTR; break;
				default:			conversion = C_LPSTR_BSTR; break;
			}
		case VT_LPWSTR:
			switch (pvargSrc->vt)
			{
				case VT_LPSTR:		conversion = C_LPWSTR_LPSTR; break;
				case VT_LPWSTR:		conversion = C_COPY; break;
				default:			conversion = C_LPWSTR_BSTR; break;
			}
		default:
			switch (pvargSrc->vt)
			{
				case VT_LPSTR:		conversion = C_BSTR_LPSTR; break;
				case VT_LPWSTR:		conversion = C_BSTR_LPWSTR; break;
				default:			conversion = C_OTHER; break;
			}
	}
	switch (conversion)
	{
		case C_COPY:
			if (pvargDest != pvargSrc)
			{
				comauto::wrapVariantClear( pvargDest);
				comauto::wrapVariantCopy( pvargDest, pvargSrc);
			}
			break;
		case C_LPSTR_LPWSTR:
		{
			LPSTR dest = comauto::createLPSTR( comauto::utf8string( V_LPWSTR( pvargSrc)));
			comauto::wrapVariantClear( pvargDest);
			V_LPSTR( pvargDest) = dest;
			pvargDest->vt = VT_LPSTR;
			break;
		}
		case C_LPWSTR_LPSTR:
		{
			LPWSTR dest = comauto::createLPWSTR( comauto::utf16string( V_LPSTR( pvargSrc)));
			comauto::wrapVariantClear( pvargDest);
			V_LPWSTR( pvargDest) = dest;
			pvargDest->vt = VT_LPWSTR;
			break;
		}
		case C_BSTR_LPWSTR:
		{
			BSTR dest = comauto::createBSTR( V_LPWSTR( pvargSrc));
			comauto::wrapVariantClear( pvargDest);
			V_BSTR( pvargDest) = dest;
			pvargDest->vt = VT_BSTR;
			break;
		}
		case C_BSTR_LPSTR:
		{
			BSTR dest = comauto::createBSTR( V_LPSTR( pvargSrc));
			comauto::wrapVariantClear( pvargDest);
			V_BSTR( pvargDest) = dest;
			pvargDest->vt = VT_BSTR;
			break;
		}
		case C_LPSTR_BSTR:
		{
			LPSTR dest = comauto::createLPSTR( comauto::utf8string( V_BSTR( pvargSrc)));
			comauto::wrapVariantClear( pvargDest);
			V_LPSTR( pvargDest) = dest;
			pvargDest->vt = VT_LPSTR;
			break;
		}
		case C_LPWSTR_BSTR:
		{
			if (pvargDest != pvargSrc)
			{
				comauto::wrapVariantClear( pvargDest);
				pvargDest->vt = VT_LPWSTR;
				V_LPWSTR(pvargDest) = comauto::createLPWSTR( V_LPWSTR( pvargSrc));
			}
			else
			{
				pvargDest->vt = VT_LPWSTR;
				V_LPWSTR(pvargDest) = (LPWSTR)V_BSTR(pvargDest);
			}
			break;
		}
		case C_OTHER:
			break;
	}
	if (vt != pvargDest->vt)
	{
		return ::VariantChangeType( pvargDest, pvargDest, wFlags, vt);
	}
	return S_OK;
}

HRESULT comauto::wrapVariantCopyInd( VARIANT* pvargDest, const VARIANT* pvargSrc)
{
	if ((pvargSrc->vt & VT_BYREF) == VT_BYREF)
	{
		VARTYPE vt = pvargSrc->vt - VT_BYREF;
		VARIANT vargSrc;
		vargSrc.vt = VT_EMPTY;

		switch (vt)
		{
			case VT_LPSTR:
				V_LPSTR( &vargSrc) = (LPSTR)pvargSrc->byref;
				return comauto::wrapVariantCopy( pvargDest, &vargSrc);
			case VT_LPWSTR:
				V_LPWSTR( &vargSrc) = (LPWSTR)pvargSrc->byref;
				return comauto::wrapVariantCopy( pvargDest, &vargSrc);
			default:
				return ::VariantCopyInd( pvargDest, pvargSrc);
		}
	}
	return ::VariantCopyInd( pvargDest, pvargSrc);
}


void comauto::copyVariantType( VARTYPE dsttype, void* dstfield, const langbind::TypedInputFilter::Element& val)
{
	if (dsttype == VT_BSTR)
	{
		VARIANT dstcp = comauto::createVariantType( val, dsttype);
		if (*((BSTR*)dstfield) != NULL) ::SysFreeString( *((BSTR*)dstfield));
		*((BSTR*)dstfield) = dstcp.bstrVal;
		dstcp.vt = VT_EMPTY;
	}
	else if (dsttype == VT_LPSTR)
	{
		VARIANT dstcp = comauto::createVariantType( val, dsttype);
		if (*((LPSTR*)dstfield) != NULL) comauto::freeMem( *((LPSTR*)dstfield));
		*((LPSTR*)dstfield) = V_LPSTR(&dstcp);
		dstcp.vt = VT_EMPTY;
	}
	else if (dsttype == VT_LPWSTR)
	{
		VARIANT dstcp = comauto::createVariantType( val, dsttype);
		if (*((LPWSTR*)dstfield) != NULL) comauto::freeMem( *((LPWSTR*)dstfield));
		*((LPWSTR*)dstfield) = V_LPWSTR(&dstcp);
		dstcp.vt = VT_EMPTY;
	}
	else if (comauto::isAtomicType( dsttype))
	{
		VARIANT dstcp = comauto::createVariantType( val, dsttype);
		::memcpy( dstfield, comauto::arithmeticTypeAddress( &dstcp), comauto::sizeofAtomicType( dsttype));
	}
	else
	{
		throw std::runtime_error( "cannot copy this type");
	}
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
			ar[ VT_INT] = sizeof(INT);
			ar[ VT_UINT] = sizeof(UINT);
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

bool comauto::isStringType( int vt)
{
	return vt == VT_BSTR || vt == VT_LPSTR || vt == VT_LPWSTR;
}

void* comauto::arithmeticTypeAddress( VARIANT* val)
{
	return (void*)((char*)val + 4*sizeof(short));
}

const void* comauto::arithmeticTypeAddress( const VARIANT* val)
{
	return (const void*)((const char*)val + 4*sizeof(short));
}

std::string comauto::variablename( const ITypeInfo* typeinfo, VARDESC* vardesc)
{
	std::string rt;
	BSTR vv;
	UINT nn;
	WRAP( const_cast<ITypeInfo*>(typeinfo)->GetNames( vardesc->memid, &vv, 1, &nn))
	rt = comauto::utf8string(vv);
	::SysFreeString( vv);
	return rt;
}

std::wstring comauto::variablename_utf16( const ITypeInfo* typeinfo, VARDESC* vardesc)
{
	std::wstring rt;
	BSTR vv;
	UINT nn;
	WRAP( const_cast<ITypeInfo*>(typeinfo)->GetNames( vardesc->memid, &vv, 1, &nn))
	rt = std::wstring(vv?vv:L"");
	::SysFreeString( vv);
	return rt;
}

std::string comauto::variabletype( const ITypeInfo* typeinfo, VARDESC* vardesc)
{
	std::string rt;
	ELEMDESC ed = vardesc->elemdescVar;
	rt = comauto::typestr( typeinfo, &ed.tdesc);
	return rt;
}

langbind::TypedInputFilter::Element comauto::getAtomicElement( VARTYPE vt, const void* ref, std::string& elembuf)
{
	typedef langbind::TypedInputFilter::Element Element;
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
		case VT_BSTR: element = Element( elembuf = comauto::utf8string( *(const BSTR*)ref)); break;
		case VT_LPSTR: element = Element( elembuf = comauto::utf8string( *(LPCSTR*)ref)); break;
		case VT_LPWSTR: element = Element( elembuf = comauto::utf8string( *(LPCWSTR*)ref)); break;
		default:
		{
			VARIANT elemorig;
			VARIANT elemcopy;
			try
			{
				elemorig.byref = const_cast<void*>(ref);
				elemorig.vt = vt | VT_BYREF;
				WRAP( comauto::wrapVariantCopyInd( &elemcopy, &elemorig))
				WRAP( comauto::wrapVariantChangeType( &elemcopy, &elemcopy, 0, VT_BSTR))
				element = Element( elembuf = comauto::utf8string( elemcopy.bstrVal));
			}
			catch (const std::runtime_error& e)
			{
				comauto::wrapVariantClear( &elemorig);
				comauto::wrapVariantClear( &elemcopy);
				throw e;
			}
		}
	}
	return element;
}

langbind::TypedInputFilter::Element comauto::getAtomicElement( const VARIANT& val, std::string& elembuf)
{
	return getAtomicElement( val.vt, comauto::arithmeticTypeAddress( &val), elembuf);
}

