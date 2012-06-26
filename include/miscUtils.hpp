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

///\brief Returns true, if the file with path 'path' exists
bool fileExists( const std::string& path);

///\brief Returns true, if the file with path 'path' exists and is a directory
bool directoryExists( const std::string& path);

///\brief Returns true, if the directory with path 'path' exists
bool isDirectory( const std::string& path);

///\brief Removes the the directory with path 'path'
void removeAllFiles( const std::string& path);

///\brief Get the path 'path' relative to 'refpath' if it is not absolute
///\return the canonical path
std::string getCanonicalPath( const std::string& path, const std::string& refpath);

///\brief Split the string by spaces, treating subsequent spaces as one space
void splitStringBySpaces( std::vector<std::string>& res, const std::string& inp);

}}//namespace

/// Temporary, till libboost 1.42 support goes away
#include <boost/version.hpp>
#if BOOST_VERSION < 104300
#define is_absolute is_complete
#define absolute complete
#endif

#endif // _MISC_UTILS_HPP_INCLUDED
