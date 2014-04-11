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
///\remark You have to build Wolframe with the flag WITH_TEXTWOLF=1 to make the types::String work
//	with all encodings and code pages listed in the interface. Without this flag set UTF-8 
//	will be accepted only as Ascii and UTF16BE (wchat_t) only with Ascii and Latin-1
//	characters. Other character set conversions are not available in this module.
#ifndef _Wolframe_TYPES_STRING_HPP_INCLUDED
#define _Wolframe_TYPES_STRING_HPP_INCLUDED
#include <cstdlib>
#include <string>
#include <stdexcept>
#include <boost/shared_ptr.hpp>
namespace _Wolframe {
namespace types {

///\class String
///\brief Type for representing strings in various encodings
class String
{
public:
	///\enum Encoding
	///\brief Encoding of a database string
	enum Encoding
	{
		ISO8859,	//Latin (full range only with WITH_TEXTWOLF=1, range 0..255 (code page 1) and range 0..127 (other code pages) otherwise
		UTF8,		//UTF-8 (full range only with WITH_TEXTWOLF=1, range 0..127 otherwise)
		UTF16BE,	//UTF-16BE (full range only with WITH_TEXTWOLF=1, range 0..255 otherwise)
		UTF16LE,	//UTF-16LE (only available with WITH_TEXTWOLF=1)
		UCS2BE,		//UCS-2BE (only available with WITH_TEXTWOLF=1)
		UCS2LE,		//UCS-2LE (only available with WITH_TEXTWOLF=1)
		UCS4BE,		//UCS-4BE (only available with WITH_TEXTWOLF=1)
		UCS4LE		//UCS-4LE (only available with WITH_TEXTWOLF=1)
	};

	///\brief Default constructor
	String();
	///\brief Constructor
	///\param[in] content base pointer of string
	///\param[in] contentsize size of content referenced by base pointer 'content' in bytes
	///\param[in] encoding_ encoding to the string to create and passed with 'content/contentsize'
	///\param[in] codepage_ code page in case of an ISO8859 character encoding
	String( const void* content, std::size_t contentsize, Encoding encoding_=UTF8, unsigned char codepage_=0);
	///\brief Constructor from std::string in UTF-8
	///\param[in] val string in UTF-8
	String( const std::string& val);
	///\brief Constructor from std::string in UTF-16BE (wchar_t)
	///\param[in] val string in UTF-16BE
	String( const std::wstring& val);
	///\brief Copy constructor
	///\brief o string to copy
	String( const String& o);
	///\brief Destructor
	~String();

	///\brief Translate this string into the encoding specified
	///\return a copy of this in a different encoding
	String translateEncoding( Encoding encoding_, unsigned char codepage_=0) const;

	///\brief Map encoding to sizeof element in string (not equal to char size for a variable-width encodings)
	///\return the size of one element of 'e'
	static std::size_t elementSize( Encoding e)
	{
		static std::size_t ar[] = {1,1,2,2,2,2,4,4};
		return ar[e];
	}
	///\brief Get the size of an element (not equal to char size for a variable-width encodings) of this string 
	///\return the size of one element
	std::size_t elementSize() const
	{
		return elementSize( (Encoding)m_encoding);
	}

	///\brief Map encoding to string
	///\param[in] e encoding
	///\param[in] c code page in case if and ISO 8859 encoding
	///\return the name of the encoding
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
	///\brief Get the encoding of this string as string
	///\return the name of the encoding
	const char* encodingName() const	{return encodingName((Encoding)m_encoding, m_codepage);}

	///\class EncodingAttrib
	///\brief Attributes describing an encoding completely
	struct EncodingAttrib
	{
		Encoding encoding;
		unsigned char codepage;

		EncodingAttrib( Encoding encoding_=UTF8, unsigned char codepage_=0)
			:encoding(encoding_),codepage(codepage_){}
		EncodingAttrib( const EncodingAttrib& o)
			:encoding(o.encoding),codepage(o.codepage){}
	};
	///\brief Parse the attributes of an encoding from an encoding name
	///\return the encoding attributes of name
	static EncodingAttrib getEncodingFromName( const std::string& name);

	///\class EncodingClass
	///\brief Structure namespace for the enumeration of supported encoding classes
	struct EncodingClass
	{
		///\enum Id
		///\brief Encoding class for guessing encoding
		enum Id {NONE,UCS1,UCS2LE,UCS2BE,UCS4LE,UCS4BE,FAIL};
	};
	///\brief Guess the encoding from content
	///\param[in] content pointer to string
	///\param[in] contentsize length of 'content' in bytes
	///\return the encoding class that can be guessed (with 100%, no probability decision !)
	static EncodingClass::Id guessEncoding( const char* content, std::size_t size);
	///\brief Guess the encoding from content
	///\param[in] content source string to gues the encoding
	///\return the encoding class that can be guessed (with 100%, no probability decision !)
	static EncodingClass::Id guessEncoding( const std::string& content);

	///\brief Get the encoding of this string
	///\return the encoding
	Encoding encoding() const		{return (Encoding)m_encoding;}	//< encoding (type Encoding)
	///\brief Get the code page of this string in case of an ISO8859 encoding
	///\return the code page or 0
	unsigned char codepage() const		{return m_codepage;}		//< codepage for ISO8859 encodings

	///\brief Get the content of the string as std::string in UTF-8
	///\return the string
	std::string tostring() const;
	///\brief Get the content of the string as std::wstring in UTF-16BE (wchar_t)
	///\return the string
	std::wstring towstring() const;

	///\brief Get the size of the string in elements
	///\remark not equal to number of bytes and for variable length encodings not equal to number of characters
	///\return the number of elements in this string
	std::size_t size() const		{return m_size;}

	///\brief Get the base pointer of the string in case of a string in encoding UTF-16BE (wchar_t)
	///\remark With size() you get the number of wchar_t of the result of w_str()
	const wchar_t* w_str() const		{if ((Encoding)m_encoding != UTF16BE) throw std::logic_error("cannot call w_str() for non UTF-16BE encoding"); return (const wchar_t*)(const void*)m_ar;}
	///\brief Get the base pointer of the string in case of a string in encoding UTF-8
	///\remark With size() you get the number of bytes of the result of c_str()
	const char* c_str() const		{if ((Encoding)m_encoding != UTF8) throw std::logic_error("cannot call c_str() for non UTF-8 encoding"); return (const char*)m_ar;}
	///\brief Get the base pointer of the string in any encoding
	///\remark The interpretation of size depends on the encoding
	const void* ptr() const			{return m_ar;}

private:
	friend class StringConst;
	enum ConstQualifier {ConstC};
	///\brief Helper constructor for types::StringConst
	///\param[in] content base pointer of string
	///\param[in] contentsize size of content referenced by base pointer 'content' in bytes
	///\param[in] encoding_ encoding to the string to create and passed with 'content/contentsize'
	///\param[in] codepage_ code page in case of an ISO8859 character encoding
	String( const ConstQualifier&, const void* content, std::size_t contentsize, Encoding encoding_=UTF8, unsigned char codepage_=0);

private:
	unsigned char m_encoding;		//< encoding (type Encoding)
	unsigned char m_codepage;		//< codepage for ISO8859 encodings
	bool m_isconst;				//< true, if the string is allocated
	std::size_t m_size;			//< number of elements in string
	unsigned char* m_ar;			//< array of elements
};

///\class StringConst
///\brief Constant string that is not owned by the structure (caller has ownership)
///\remark This object holds only string reference. No copy is made. The life time of this has to be covered by the scope of the parameters used to construct the string
class StringConst
	:public String
{
public:
	///\brief Constructor
	StringConst(){}
	///\brief Constructor
	///\param[in] content base pointer of string
	///\param[in] contentsize size of content referenced by base pointer 'content' in bytes
	///\param[in] encoding_ encoding to the string to create and passed with 'content/contentsize'
	///\param[in] codepage_ code page in case of an ISO8859 character encoding
	StringConst( const void* content, std::size_t contentsize, Encoding encoding_=UTF8, unsigned char codepage_=0)
		:String(ConstC,content,contentsize,encoding_,codepage_){}
	///\brief Constructor from a reference to a std::string in UTF-8
	///\param[in] val reference to string in UTF-8
	StringConst( const std::string& val)
		:String(ConstC,val.c_str(),val.size(),UTF8){}
	///\brief Constructor from a reference to a std::string in UTF-16BE (wchar_t)
	///\param[in] val reference to string in UTF-16BE
	StringConst( const std::wstring& val)
		:String(ConstC,val.c_str(),val.size()*2,UTF16BE){}
	///\brief Copy constructor
	///\brief o string reference to copy
	StringConst( const String& o)
		:String(ConstC,o.m_ar,o.m_size*elementSize(encoding()),encoding(),o.m_codepage){}
};

}}//namespace
#endif

