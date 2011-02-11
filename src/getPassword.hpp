//
// getPassword.cpp
//

#ifdef WITH_SSL

#include <string>
#include "acceptor.hpp"

namespace _SMERP {
	namespace Network {

#if !defined(_WIN32)

#include <unistd.h>
#include <libintl.h>

	std::string SSLacceptor::getPassword()
	{
		char	*pass;

		pass = getpass( gettext( "Enter your password:" ));

		return std::string( pass );
	}

#else // defined(_WIN32)

	std::string SSLacceptor::getPassword()
	{
		return "bla bla";
	}

#endif // defined(_WIN32)


} // namespace Network
} // namespace _SMERP

#endif // WITH_SSL
//
// logLevel.hpp
//

#ifndef _LOG_LEVEL_HPP_INCLUDED
#define _LOG_LEVEL_HPP_INCLUDED

#include <iostream>

namespace _SMERP {

class LogLevel {
public:
	enum Level	{
		LOGLEVEL_DATA,		/// log everything, including data
		LOGLEVEL_TRACE,		/// trace functions calls
		LOGLEVEL_DEBUG,		/// log operations
		LOGLEVEL_INFO,
		LOGLEVEL_NOTICE,
		LOGLEVEL_WARNING,
		LOGLEVEL_ERROR,
		LOGLEVEL_SEVERE,
		LOGLEVEL_CRITICAL,
		LOGLEVEL_ALERT,
		LOGLEVEL_FATAL,
		LOGLEVEL_UNDEFINED	/// log this only under special conditions
	};

	static Level str2LogLevel( const std::string s );
};


// map enum values to strings
template< typename CharT, typename TraitsT >
inline std::basic_ostream< CharT, TraitsT > &operator<< ( std::basic_ostream< CharT, TraitsT >& s, LogLevel::Level l )
{
	static const CharT *const str[] = {
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
