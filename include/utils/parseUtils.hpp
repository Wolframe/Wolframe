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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
///\file utils/fileUtils.hpp
///\brief Utility functions for parsing, tokenization and positional error handling

#ifndef _WOLFRAME_PARSE_UTILS_HPP_INCLUDED
#define _WOLFRAME_PARSE_UTILS_HPP_INCLUDED
#include "utils/sourceLineInfo.hpp"
#include <string>
#include <vector>
#include <map>

namespace _Wolframe {
namespace utils {

///\class CharTable
///\brief Character table structure for parseNextToken( std::string&,std::string::const_iterator&, std::string::const_iterator, ...);
//\remark A range of characters is specified with 2 dots between the first and the last character of the range (e.g. "a..z")
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

///\class IdentifierTable
///\brief Identifier table structure for parseNextIdentifier( std::string::const_iterator&, std::string::const_iterator, ...);
class IdentifierTable
{
public:
	//\brief Constructor
	//\param[in] casesensitive_ true, if the keywords inserted are case sensitive
	//\param[in] arg NULL terminated array of identifiers to insert
	IdentifierTable( bool casesensitive_, const char** arg);

	//\brief Lookup in table
	int operator[]( const std::string&) const;
	//\brief Get the list of keywords defined as string for log messages
	std::string tostring() const;
	//\brief Get the keyword with index 'id' (starting from 1)
	const char* idstring( int id) const;

private:
	bool m_casesensitive;			//< true, if the keywords inserted are case sensitive
	std::map<std::string,int> m_tab;	//< table implementation
	const char** m_arg;			//< original set of keywors for inverse lookups
};


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

///\brief Parse the next identifier if it is in 'idtab' or goto the next token if not
///\return the index of the identifier (starting from 1) in 'idtab' or 0, if the next token does not match
///\param[in,out] si scanning iterator passed as start of the source to parse and returned as source position after the token parsed if it matches or start of the token not matching if not
///\param[in] se iterator marking the end of the source
///\param[in] idtab identifier table
//\return 0 if not found, else index of identifier in table starting with 1
int parseNextIdentifier( std::string::const_iterator& si, const std::string::const_iterator& se, const IdentifierTable& idtab);

///\brief Parse a token assignement 'identifier = token'
///\return pair with assignement first = identifier, second = token
///\param[in,out] itr scanning iterator passed as start of the source to parse and returned as source position after the expression parsed
///\param[in] end iterator marking the end of the source
///\param[in] alphaTable table with characters valid in an identifier
std::pair<std::string,std::string> parseTokenAssignement( std::string::const_iterator& itr, std::string::const_iterator end, const CharTable& alphaTable);

///\brief Parse a token assignement 'identifier = token' with the standard identifier table
///\return pair with assignement first = identifier, second = token
///\param[in,out] itr scanning iterator passed as start of the source to parse and returned as source position after the expression parsed
///\param[in] end iterator marking the end of the source
std::pair<std::string,std::string> parseTokenAssignement( std::string::const_iterator& itr, std::string::const_iterator end);

///\remark throws exception on error
///\return line parsed
///\param[in,out] itr scanning iterator passed as start of the source to parse and returned as source position after the expression parsed)
///\param[in] end iterator marking the end of the source
std::string parseNextLine( std::string::const_iterator& itr, std::string::const_iterator end);

}} //namespace _Wolframe::utils

#endif // _MISC_UTILS_HPP_INCLUDED
