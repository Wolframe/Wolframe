#include "logger.hpp"

#include <iostream>

#include <boost/log/common.hpp>
#include <boost/log/formatters.hpp>
#include <boost/log/utility/init/common_attributes.hpp>

#include <boost/log/utility/init/to_console.hpp>

namespace logging = boost::log;
namespace keywords = boost::log::keywords;
namespace fmt = boost::log::formatters;
namespace src = boost::log::sources;

namespace _SMERP {

// map enum values to strings
template< typename CharT, typename TraitsT >
inline std::basic_ostream< CharT, TraitsT > &operator<< ( std::basic_ostream< CharT, TraitsT >& s, Logger::LogLevel l ) {
	static const char *const str[] = {
		"ALWAYS", "FATAL", "ALERT", "CRITICAL", "SEVERE", "ERROR", "WARNING", "NOTICE", "INFO",
		"DEBUG", "TRACE", "DATA", "NEVER" };
	if( static_cast< size_t >( l ) < ( sizeof( str ) / sizeof( *str ) ) ) {
		s << str[l];
	} else {
		s << "Unknown enum used '" << static_cast< int >( l ) << "'";
	}
	return s;
}

#define LOG_DEBUG      BOOST_LOG_SEV( logger, Logger::LogLevel::DEBUG )

void Logger::initialize( ) {

	// open logger to the console
	logging::init_log_to_console(
		std::clog,
		keywords::format = fmt::format( "%1% %2%: %3%" )
			% fmt::date_time( "TimeStamp", std::nothrow )
			% fmt::attr< Logger::LogLevel >( "Severity", std::nothrow )
			% fmt::message( )
	);

	logging::add_common_attributes( );

	src::severity_logger< Logger::LogLevel > logger;

	LOG_DEBUG << "Logger started";
}

} // namespace _SMERP
