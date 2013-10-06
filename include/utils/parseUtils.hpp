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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
//
// parse utility functions
//
#ifndef _WOLFRAME_PARSE_UTILS_HPP_INCLUDED
#define _WOLFRAME_PARSE_UTILS_HPP_INCLUDED
#include <string>
#include <vector>

namespace _Wolframe {
namespace utils {

///\class CharTable
///\brief Character table structure for parseNextToken( std::string&,std::string::const_iterator&, std::string::const_iterator, ...);
class CharTable
{
public:
	CharTable( const char* op="", bool isInverse=false);
	bool operator[]( char ch) const		{return m_ar[ (unsigned char)ch];}
private:
	bool m_ar[256];
};

const CharTable& identifierCharTable();
const CharTable& emptyCharTable();
const CharTable& anyCharTable();

///\brief Parsing the next token in a UTF-8 or Isolatin-1 string that is either
// - a single quoted (') string backslash escaping returned without the delimiting quotes and escaping resolved
// - a double quoted (") string backslash escaping returned without the delimiting quotes and escaping resolved
// - an identifier consisting of alphanumeric ASCII characters and characters beyond the ASCII code page (>127)
// - a single character operator from an operator table
///
///\remark throws exception on error
///\return the token as character in case of an operator, else the first character of the token parsed in the source, '\0' when no token was found (EOF)
///\param[out] tok non operator token parsed
///\param[in,out] itr scanning iterator passed as start of the source to parse and returned as source position after the token parsed)
///\param[in] end iterator marking the end of the source
///\param[in] operatorTable (optional) operator table
///\param[in] alphaTable (optional) token alphabet as character table
char parseNextToken( std::string& tok, std::string::const_iterator& itr, std::string::const_iterator end, const CharTable& operatorTable, const CharTable& alphaTable);
///\brief See utils::parseNextToken(std::string&,std::string::const_iterator&,std::string::const_iterator,const CharTable&,const CharTable&)
char parseNextToken( std::string& tok, std::string::const_iterator& itr, std::string::const_iterator end, const CharTable& operatorTable);
///\brief See utils::parseNextToken(std::string&,std::string::const_iterator&,std::string::const_iterator,const CharTable&,const CharTable&)
char parseNextToken( std::string& tok, std::string::const_iterator& itr, std::string::const_iterator end);
///\brief Skip to next token (skip white spaces)
char gotoNextToken( std::string::const_iterator& itr, std::string::const_iterator end);
///\brief Parse the rest of the line starting
///\param[in,out] si start of chunk to parse as input and first character after end of line if found or end of chunk to parse as output
///\param[in] se end of chunk to parse
///\return line parsed without end of line marker
std::string parseLine( std::string::const_iterator& si, const std::string::const_iterator& se);

///\brief Parse a token assignement 'identifier = token'
///\return pair with assignement first = identifier, second = token
std::pair<std::string,std::string> parseTokenAssignement( std::string::const_iterator& itr, std::string::const_iterator end, const CharTable& alphaTable);
std::pair<std::string,std::string> parseTokenAssignement( std::string::const_iterator& itr, std::string::const_iterator end);

///\remark throws exception on error
///\return line parsed
///\param[in,out] itr scanning iterator passed as start of the source to parse and returned as source position after the token parsed)
///\param[in] end iterator marking the end of the source
std::string parseNextLine( std::string::const_iterator& itr, std::string::const_iterator end);

///\brief Get the line/column info from a source iterator
struct LineInfo
{
	unsigned int line;
	unsigned int column;

	LineInfo()
		:line(1),column(1){}
	LineInfo( unsigned int line_, unsigned int column_)
		:line(line_),column(column_){}
	LineInfo( const LineInfo& o)
		:line(o.line),column(o.column){}

	void incrementLine()	{column=1; ++line;}
	void incrementColumn()	{++column;}
};

LineInfo getLineInfo( const std::string::const_iterator& start, const std::string::const_iterator& pos);

}} //namespace _Wolframe::utils

#endif // _MISC_UTILS_HPP_INCLUDED
