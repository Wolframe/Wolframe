//
// miscelaneous utility functions
//

#include <string>

#ifndef _MISC_UTILS_HPP_INCLUDED
#define _MISC_UTILS_HPP_INCLUDED


/// Remove .. and . from a path
std::string resolvePath( const std::string& path );

/// Temporary, till libboost 1.42 support goes away
#ifdef _WIN32
#undef ABSOLUTE
#endif
#include <boost/version.hpp>
#if BOOST_VERSION < 104300
#define IS_ABSOLUTE is_complete
#define ABSOLUTE complete
#else
#define IS_ABSOLUTE is_absolute
#define ABSOLUTE absolute
#endif

#endif // _MISC_UTILS_HPP_INCLUDED
