/************************************************************************

 Copyright (C) 2011, 2012 Project Wolframe.
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
///\param[in] path relative part of the path or a bsolute path
///\param[in] refpath absolute part of the path if 'path' is relative
///\return the canonical path
std::string getCanonicalPath( const std::string& path, const std::string& refpath);

///\brief Get the parent path of 'path' (skipping /./ as parent path unlike boost::filesystem does)
///\param[in] path the path to get the parent path from
///\param[in] level how many level (1 = parent, 2 = granparent, etc.)
///\return the parent path
std::string getParentPath( const std::string& path, unsigned int levels=1);

///\brief Returns true, if the file with path 'path' exists
bool fileExists( const std::string& path);


///\brief Split the string, ingoring empty parts of the result (susequent split characters treated as one)
///\param[in] inp string to split
///\param[in] splitchr set of characters to split with (each of them is one separating character)
void splitString( std::vector<std::string>& res, const std::string& inp, const char* splitchr);


/// \brief Normalize a string in place
/// \param[in][out] str	string to be normalized
void normalizeString( std::string& str );

/// \brief Copy the string and normalize it
/// \param[in]	str	string to be normalized
/// \returns		normalized string
std::string normalizeString_copy( const std::string& str );

/// \brief Compute a degree of similarity between strings
/// \note This is work in progress and not used yet. Most likely it will also
/// go to a different place.
/// \param[in]	str1	string to be compared
/// \param[in]	str2	reference string
/// \returns		s distance between the normalized strings (0 => strings are equal)
int stringSimilarity( const std::string& str1, const std::string& str2 );


///\class OperatorTable
///\brief Operator table structure for parseNextToken( std::string&,std::string::const_iterator&, std::string::const_iterator, );
class OperatorTable
{
public:
	OperatorTable( const char* op="");
	bool operator[]( char ch) const		{return m_ar[ (unsigned char)ch];}
private:
	bool m_ar[256];
};

///\brief Parsing the next token in a UTF-8 or Isolatin-1 string that is either
// - a single quoted (') string backslash escaping returned without the delimiting quotes and escaping resolved
// - a double quoted (") string backslash escaping returned without the delimiting quotes and escaping resolved
// - an identifier consisting of alphanumeric ASCII characters and characters beyond the ASCII code page (>127)
// - a single character operator from an operator table
///
///\remark throws exception on error
///\return the first character of the token parsed in the source if a token was found and parsed, '\0' when no token was found (EOF)
///\param[out] tok token parsed
///\param[in,out] itr scanning iterator passed as start of the source to parse and returned as source position after the token parsed)
///\param[in] end iterator marking the end of the source
///\param[in] operatorTable (optional) operator table
char parseNextToken( std::string& tok, std::string::const_iterator& itr, std::string::const_iterator end, const OperatorTable& operatorTable);
char parseNextToken( std::string& tok, std::string::const_iterator& itr, std::string::const_iterator end);


///\brief Reading the content of a source file
///\remark Throws on error
///\TODO BETTER AND SAVER IMPLEMENTATION
std::string readSourceFileContent( const std::string& filename);

}} //namespace _Wolframe::utils

/// Temporary, till libboost 1.42 support goes away
#include <boost/version.hpp>
#if BOOST_VERSION < 104300
#define is_absolute is_complete
#define absolute complete
#endif

#endif // _MISC_UTILS_HPP_INCLUDED
