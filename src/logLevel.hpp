//
// logLevel.hpp
//

#ifndef _LOG_LEVEL_HPP_INCLUDED
#define _LOG_LEVEL_HPP_INCLUDED

#include <string>
#include <iostream>

namespace _SMERP {

class LogLevel {
public:
	enum Level	{
		_SMERP_DATA,		/// log everything, including data
		_SMERP_TRACE,		/// trace functions calls
		_SMERP_DEBUG,		/// log operations
		_SMERP_INFO,
		_SMERP_NOTICE,
		_SMERP_WARNING,
		_SMERP_ERROR,
		_SMERP_SEVERE,
		_SMERP_CRITICAL,
		_SMERP_ALERT,
		_SMERP_FATAL,
		_SMERP_UNDEFINED	/// log this only under special conditions
	};

	static Level str2LogLevel( const std::string s );
};


// map enum values to strings
template< typename CharT, typename TraitsT >
inline std::basic_ostream< CharT, TraitsT > &operator<< ( std::basic_ostream< CharT, TraitsT >& s, LogLevel::Level l )
{
	static const char *const str[] = {
		"DATA", "TRACE", "DEBUG", "INFO", "NOTICE", "WARNING",
		"ERROR", "SEVERE", "CRITICAL", "ALERT", "FATAL" };
	if( static_cast< size_t >( l ) < ( sizeof( str ) / sizeof( *str ) ) ) {
		s << str[l];
	} else {
		s << "Unknown enum used '" << static_cast< int >( l ) << "'";
	}
	return s;
}

} // namespace _SMERP


#endif // _LOG_LEVEL_HPP_INCLUDED
