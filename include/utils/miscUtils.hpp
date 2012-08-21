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

///\brief Returns true, if the file with path 'path' exists
bool fileExists( const std::string& path);


///\brief Split the string, ingoring empty parts of the result (susequent split characters treated as one)
///\param[in] inp string to split
///\param[in] splitchr set of characters to split with (each of them is one separating character)
void splitString( std::vector<std::string>& res, const std::string& inp, const char* splitchr);

///\brief Parsing the next token in a UTF-8 or Isolatin-1 string that is either
// - a single quoted (') string backslash escaping returned without the delimiting quotes and escaping resolved
// - a double quoted (") string backslash escaping returned without the delimiting quotes and escaping resolved
// - an identifier consisting of alphanumeric ASCII characters and characters beyond the ASCII code page (>127)
// - an operator   '+' '-' '*' '/' '|' '&' '@' '!' '?' ':' '.' ';' ',' '#' '(' ')' '[' ']' '<' '>'
///
///\remark throws exception on error
///\return true if a token was found and parsed, false when no token was found (EOF)
///\param[out] tok token parsed
///\param[in,out] itr scanning iterator passed as start of the source to parse and returned as source position after the token parsed)
///\param[in] end iterator marking the end of the source
bool parseNextToken( std::string& tok, std::string::const_iterator& itr, std::string::const_iterator end);

}}//namespace

/// Temporary, till libboost 1.42 support goes away
#include <boost/version.hpp>
#if BOOST_VERSION < 104300
#define is_absolute is_complete
#define absolute complete
#endif

#endif // _MISC_UTILS_HPP_INCLUDED
