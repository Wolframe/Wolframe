//
// miscelaneous utility functions
//

#include <string>

#ifndef _MISC_UTILS_HPP_INCLUDED
#define _MISC_UTILS_HPP_INCLUDED


/// Remove .. and . from a path
std::string resolvePath( const std::string& path );

/// Check if a string is (case insensitive) a prefix for the other one (command)
int commandCmp( const char *str, const char *command );

#endif // _MISC_UTILS_HPP_INCLUDED
