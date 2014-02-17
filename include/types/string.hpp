/************************************************************************
Copyright (C) 2011 - 2013 Project Wolframe.
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
///\file types/string.hpp
///\brief Type for strings in various encodings

#ifndef _Wolframe_TYPES_STRING_HPP_INCLUDED
#define _Wolframe_TYPES_STRING_HPP_INCLUDED
#include <cstdlib>
#include <string>
#include <stdexcept>
#include <boost/shared_ptr.hpp>
namespace _Wolframe {
namespace types {

//\class String
//\brief Type for representing strings in various encodings
class String
{
public:
	//\enum Encoding
	//\brief Encoding of a database string
	enum Encoding
	{
		ISO8859,
		UTF8,
		UTF16BE,
		UTF16LE,
		UCS2BE,
		UCS2LE,
		UCS4BE,
		UCS4LE
	};

	String();
	String( const void* content, std::size_t contentsize, Encoding encoding_=UTF8, unsigned char codepage_=0);
	String( const std::string& val);
	String( const std::wstring& val);
	String( const String& o);
	~String();

	String translateEncoding( Encoding encoding_, unsigned char codepage_=0) const;

	//\brief Map encoding to sizeof element in string (not equal to char size for a variable-width encodings)
	static std::size_t elementSize( Encoding e)
	{
		static std::size_t ar[] = {1,1,2,2,2,2,4,4};
		return ar[e];
	}
	std::size_t elementSize() const
	{
		return elementSize( (Encoding)m_encoding);
	}

	//\brief Map encoding to string
	static const char* encodingName( Encoding e, unsigned char c=0)
	{
		static const char* ar[] = {"ISO8859","UTF-8","UTF-16BE","UTF-16LE","UCS-2BE","UCS-2LE","UCS-4BE","UCS-4LE"};
		static const char* cp[] = {"ISO8859-1","ISO8859-2","ISO8859-3","ISO8859-4","ISO8859-5","ISO8859-6","ISO8859-7","ISO8859-8","ISO8859-9"};
		if (e == ISO8859 && c)
		{
			if (c > 9) return 0;
			return cp[ c];
		}
		else
		{
			return ar[e];
		}
	}
	const char* encodingName() const	{return encodingName((Encoding)m_encoding, m_codepage);}

	struct EncodingAttrib
	{
		Encoding encoding;
		unsigned char codepage;

		EncodingAttrib( Encoding encoding_=UTF8, unsigned char codepage_=0)
			:encoding(encoding_),codepage(codepage_){}
		EncodingAttrib( const EncodingAttrib& o)
			:encoding(o.encoding),codepage(o.codepage){}
	};
	static EncodingAttrib getEncodingFromName( const std::string& name);

	struct EncodingClass
	{
		enum Id {NONE,UCS1,UCS2LE,UCS2BE,UCS4LE,UCS4BE,FAIL};
	};
	static EncodingClass::Id guessEncoding( const char* content, std::size_t size);

	Encoding encoding() const		{return (Encoding)m_encoding;}	//< encoding (type Encoding)
	unsigned char codepage() const		{return m_codepage;}		//< codepage for ISO8859 encodings

	std::string tostring() const;
	std::wstring towstring() const;

	std::size_t size() const		{return m_size;}
	const wchar_t* w_str() const		{if ((Encoding)m_encoding != UTF16BE) throw std::logic_error("cannot call w_str() for non UTF-16BE encoding"); return (const wchar_t*)(const void*)m_ar;}
	const char* c_str() const		{if ((Encoding)m_encoding != UTF8) throw std::logic_error("cannot call c_str() for non UTF-8 encoding"); return (const char*)m_ar;}
	const void* ptr() const			{return m_ar;}

private:
	friend class StringConst;
	enum ConstQualifier {ConstC};
	String( const ConstQualifier&, const void* content, std::size_t contentsize, Encoding encoding_=UTF8, unsigned char codepage_=0);

private:
	unsigned char m_encoding;		//< encoding (type Encoding)
	unsigned char m_codepage;		//< codepage for ISO8859 encodings
	bool m_isconst;				//< true, if the string is allocated
	std::size_t m_size;			//< number of elements in string
	unsigned char* m_ar;			//< array of elements
};


class StringConst
	:public String
{
public:
	StringConst(){}
	StringConst( const void* content, std::size_t contentsize, Encoding encoding_=UTF8, unsigned char codepage_=0)
		:String(ConstC,content,contentsize,encoding_,codepage_){}
	StringConst( const std::string& val)
		:String(ConstC,val.c_str(),val.size(),UTF8){}
	StringConst( const std::wstring& val)
		:String(ConstC,val.c_str(),val.size()*2,UTF16BE){}
	StringConst( const String& o)
		:String(ConstC,o.m_ar,o.m_size*elementSize(encoding()),encoding(),o.m_codepage){}
};

}}//namespace
#endif

