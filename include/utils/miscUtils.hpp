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
// miscelaneous utility functions
//
#ifndef _MISC_UTILS_HPP_INCLUDED
#define _MISC_UTILS_HPP_INCLUDED
#include <string>
#include <vector>
#include <boost/property_tree/ptree.hpp>

namespace _Wolframe {
namespace utils {

///\brief Remove .. and . from a path
///\param[in] path Path to process
std::string resolvePath( const std::string& path );

///\brief Get the extension of a file
///\param[in] path The path to the file to get the extension from
///\return The file extension including the extension marking dot '.'
std::string getFileExtension( const std::string& path);

///\brief Get the stem of a file
///\param[in] path The path to the file to get the stem from
///\return The file stem without the extension marking dot '.'
std::string getFileStem( const std::string& path);

///\brief Get the path 'path' relative to 'refpath' if it is not absolute
///\param[in] path relative part of the path or absolute path
///\param[in] refpath absolute part of the path if 'path' is relative
///\return the canonical path
std::string getCanonicalPath( const std::string& path, const std::string& refPath );

///\brief Get the parent path of 'path' (skipping /./ as parent path unlike boost::filesystem does)
///\param[in] path the path to get the parent path from
///\param[in] levels how many level (1 = parent, 2 = granparent, etc.)
///\return the parent path
std::string getParentPath( const std::string& path, unsigned int levels=1);

///\brief Returns true, if the file with path 'path' exists
bool fileExists( const std::string& path);


///\brief Split the string, ingoring empty parts of the result (susequent split characters treated as one)
///\param[out] res result of the split
///\param[in] inp string to split
///\param[in] splitchr set of characters to split with (each of them is one separating character)
void splitString( std::vector<std::string>& res, const std::string& inp, const char* splitchr);

///\brief Split the string, ingoring empty parts of the result (susequent split characters treated as one)
///\param[out] res result of the split
///\param[in] begin begin input iterator
///\param[in] end end input iterator
///\param[in] splitchr set of characters to split with (each of them is one separating character)
void splitString( std::vector<std::string>& res, std::string::const_iterator begin, std::string::const_iterator end, const char* splitchr);


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
std::pair<unsigned int,unsigned int> getLineInfo( const std::string::const_iterator& start, const std::string::const_iterator& pos);

///\brief Reading the content of a source file
///\remark Throws on error
std::string readSourceFileContent( const std::string& filename);
std::vector<std::string> readSourceFileLines( const std::string& filename);
boost::property_tree::ptree readPropertyTreeFile( const std::string& filename);

///\brief Writing a file
///\remark Throws on error
void writeFile( const std::string& filename, const std::string& content);

///\brief Get the file type as string
///\remark Very simplistic implementation for now
///\return file type ("XML","TEXT:UTF-8","TEXT:ASCII", "" (unknown))
std::string getFileType( const std::string& filename);


}} //namespace _Wolframe::utils

#endif // _MISC_UTILS_HPP_INCLUDED
