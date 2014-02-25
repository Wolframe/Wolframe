/************************************************************************
Copyright (C) 2011 - 2014 Project Wolframe.
All rights reserved.

This file is part of Project Wolframe.

Commercial Usage
Licensees holding valid Project Wolframe Commercial licenses may
use this file in accordance with the Project Wolframe
Commercial License Agreement provided with the Software or,
alternatively, in accordance with the terms contained
in a written agreement between the licensee and Project Wolframe.

GNU General Public License Usage
Alternatively, you can redistribute this file and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Wolframe is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
#ifndef _Wolframe_COM_AUTOMATION_UTILS_INCLUDED
#define _Wolframe_COM_AUTOMATION_UTILS_INCLUDED
#include "filter/typedfilter.hpp"
#include <cstring>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <oaidl.h>
#include <comdef.h>
#include <atlbase.h>
#include <atlcom.h>

namespace _Wolframe {
namespace comauto {

void* allocMem( std::size_t size);
void freeMem( void* ptr);

std::wstring utf16string( const std::string& utf8str);
std::wstring utf16string( const char* utf8ptr, std::size_t utf8size);
std::string utf8string( const std::wstring& utf16str);
std::string utf8string( LPCSTR str);
std::string utf8string( LPCWSTR str);
char* createLPSTR( const std::string& str);
char* createLPSTR( LPCSTR str);
wchar_t* createLPWSTR( const std::wstring& str);
wchar_t* createLPWSTR( LPCWSTR str);
BSTR createBSTR( LPCSTR str);
BSTR createBSTR( LPCWSTR str);
std::string utf8string( const BSTR& str);
std::wstring tostring( VARIANT* v);
std::wstring tostring( const _com_error& err);
std::string tostring( const EXCEPINFO& einfo);
std::string typestr( VARTYPE vt);
std::string typestr( const ITypeInfo* typeinfo, const TYPEDESC* ed);
std::string typestr( const ITypeInfo* typeinfo);
std::string structstring( const ITypeInfo* typeinfo);
std::string variablename( const ITypeInfo* typeinfo, VARDESC* vardesc);
std::wstring variablename_utf16( const ITypeInfo* typeinfo, VARDESC* vardesc);
std::string variabletype( const ITypeInfo* typeinfo, VARDESC* vardesc);

bool isCOMInterfaceMethod( const std::string& name);

VARIANT createVariantType();
VARIANT createVariantType( bool val);
VARIANT createVariantType( int val);
VARIANT createVariantType( unsigned int val);
VARIANT createVariantType( short val);
VARIANT createVariantType( unsigned short val);
VARIANT createVariantType( signed __int64 val);
VARIANT createVariantType( unsigned __int64 val);
VARIANT createVariantType( float val);
VARIANT createVariantType( double val);
VARIANT createVariantType( const std::string& val, VARTYPE stringtype=VT_BSTR);
VARIANT createVariantType( const char* val, std::size_t valsize, VARTYPE stringtype=VT_BSTR);
VARIANT createVariantType( const std::wstring& val);
VARIANT createVariantType( const types::Variant& val);
VARIANT createVariantType( const types::Variant& val, VARTYPE dsttype);
VARIANT createVariantArray( VARTYPE vt, const IRecordInfo* recinfo, const std::vector<VARIANT>& ar=std::vector<VARIANT>());
void copyVariantType( VARTYPE dsttype, void* dstfield, const types::Variant& val);
HRESULT wrapVariantCopy( VARIANT* pvargDest, const VARIANT* pvargSrc);
HRESULT wrapVariantCopyInd( VARIANT* pvargDest, const VARIANT* pvargSrc);
HRESULT wrapVariantClear( VARIANT* pvarg);
HRESULT wrapVariantChangeType( VARIANT* pvargDest, const VARIANT* pvargSrc, unsigned short wFlags, VARTYPE vt);

unsigned char sizeofAtomicType( int vt);
bool isAtomicType( int vt);
bool isStringType( int vt);
const void* arithmeticTypeAddress( const VARIANT* val);
void* arithmeticTypeAddress( VARIANT* val);

types::VariantConst getAtomicElement( VARTYPE vt, const void* ref, std::string& elembuf);
types::VariantConst getAtomicElement( const VARIANT& val, std::string& elembuf);

#define WRAP(SYSCALL){\
	static const char* call = "" #SYSCALL;\
	HRESULT hr = SYSCALL;\
	if (hr != S_OK)\
	{\
		const char* endcallname = strchr( call, '(');\
		std::string callname( call, endcallname?(endcallname-call):strlen(call));\
		_com_error error(hr);\
		std::ostringstream errcode; errcode << std::hex << "0x" << hr; \
		throw std::runtime_error( std::string("Error calling ") + callname + ": '" + comauto::utf8string(error.ErrorMessage()) + "' [" + errcode.str() + "]");\
	}}

const char*& impl_V_LPSTR( const VARIANT* v);
char*& impl_V_LPSTR( VARIANT* v);
const wchar_t*& impl_V_LPWSTR( const VARIANT* v);
wchar_t*& impl_V_LPWSTR( VARIANT* v);
const BSTR& impl_V_BSTR( const VARIANT* v);
BSTR& impl_V_BSTR( VARIANT* v);

#ifndef V_LPSTR
#define V_LPSTR(V) comauto::impl_V_LPSTR(V)
#endif
#ifndef V_LPWSTR
#define V_LPWSTR(V) comauto::impl_V_LPWSTR(V)
#endif
#ifndef V_BSTR
#define V_BSTR(V) comauto::impl_V_BSTR(V)
#endif

}}
#endif
