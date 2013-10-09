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
///\file langbind/charsetEncodings.hpp
///\brief Interface for character set encodings for modules that do support only one or some of them

#ifndef _Wolframe_LANGBIND_CHARSET_ENDOCDING_HPP_INCLUDED
#define _Wolframe_LANGBIND_CHARSET_ENDOCDING_HPP_INCLUDED
#include <string>
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace langbind {

struct CharsetEncodingStruct
{
	virtual ~CharsetEncodingStruct(){}
	virtual void convertToUTF8( std::string& dest, const char* content, std::size_t contentsize) const=0;
	virtual void convertFromUTF8( std::string& dest, const char* content, std::size_t contentsize) const=0;
	virtual void printEOLN( std::string& dest) const=0;
};
typedef boost::shared_ptr<CharsetEncodingStruct> CharsetEncoding;

CharsetEncoding getCharsetEncoding( const std::string& name);

std::string convertStringCharsetToUTF8( const CharsetEncoding& encoding, const std::string& content);
std::string convertStringUTF8ToCharset( const CharsetEncoding& encoding, const std::string& content);

struct CharsetClass
{
	//\brief Enumeration of recorgnized character set classes (UTF and UCS cannot be distinguished)
	enum Id {NONE,UCS1,UCS2LE,UCS2BE,UCS4LE,UCS4BE,FAIL};

	///\brief Guess the character set of a source file containing ASCII operators
	///\remark Does not distinguish between UTF and UCS
	///\return A class or FAIL for error or NONE if charset class can not be recognized
	static CharsetClass::Id guess( const char* content, std::size_t size);
};

}}
#endif


