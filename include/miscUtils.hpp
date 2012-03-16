//
// miscelaneous utility functions
//

#include <string>

#ifndef _MISC_UTILS_HPP_INCLUDED
#define _MISC_UTILS_HPP_INCLUDED


/// Remove .. and . from a path
std::string resolvePath( const std::string& path );

/// Temporary, till libboost 1.42 support goes away
#include <boost/version.hpp>
#if BOOST_VERSION < 104300
#define is_absolute is_complete
#define absolute complete
#endif

#endif // _MISC_UTILS_HPP_INCLUDED
