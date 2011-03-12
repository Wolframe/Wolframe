//
// logLevel.hpp
//

#ifndef _LOG_LEVEL_HPP_INCLUDED
#define _LOG_LEVEL_HPP_INCLUDED

#include <iostream>

namespace _Wolframe {
	namespace Logging {

	class LogLevel {
	public:
		enum Level	{
			LOGLEVEL_DATA,		/// log everything, including data
			LOGLEVEL_TRACE,		/// trace functions calls
			LOGLEVEL_DEBUG,		/// log operations
			LOGLEVEL_INFO,		/// information message
			LOGLEVEL_NOTICE,	/// normal but significant condition
			LOGLEVEL_WARNING,	/// warning condition
			LOGLEVEL_ERROR,		/// normal error condition
			LOGLEVEL_SEVERE,	/// severe error condition
			LOGLEVEL_CRITICAL,	/// critical error condition
			LOGLEVEL_ALERT,		/// action must be taken immediately
			LOGLEVEL_FATAL,		/// system is unusable
			LOGLEVEL_UNDEFINED	/// log this only under special conditions
		};

		static Level str2LogLevel( const std::string str );
	};


	// map enum values to strings
	template< typename CharT, typename TraitsT >
	inline std::basic_ostream< CharT, TraitsT > &operator<< ( std::basic_ostream< CharT, TraitsT >& s,
								  LogLevel::Level l )
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

	} // namespace Logging
} // namespace _Wolframe


#endif // _LOG_LEVEL_HPP_INCLUDED
