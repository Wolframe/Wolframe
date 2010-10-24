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
		LOG_DATA,		/// log everything, including data
		LOG_TRACE,		/// trace functions calls
		LOG_DEBUG,		/// log operations
		LOG_INFO,
		LOG_NOTICE,
		LOG_WARNING,
		LOG_ERROR,
		LOG_SEVERE,
		LOG_CRITICAL,
		LOG_ALERT,
		LOG_FATAL,
		LOG_UNDEFINED	/// log this only under special conditions
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
		s << "Unknown enum used '" << static_cast< int >( l ) << "' as log level";
	}
	return s;
}

} // namespace _SMERP


#endif // _LOG_LEVEL_HPP_INCLUDED
