//
// miscelaneous utility functions
//

#include <string>

#ifndef _MISC_UTILS_HPP_INCLUDED
#define _MISC_UTILS_HPP_INCLUDED


/// Remove .. and . from a path
std::string resolvePath( const std::string& path );


namespace _SMERP {

	/// String to boolean functions
	bool isBoolean( const std::string& val );
	bool isTrue( const std::string& val );
	bool isFalse( const std::string& val );

} // namespace _SMERP

#endif // _MISC_UTILS_HPP_INCLUDED
