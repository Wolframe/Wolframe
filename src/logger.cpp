#include "logger.hpp"

#include <iostream>

#include <boost/log/common.hpp>
#include <boost/log/formatters.hpp>
#include <boost/log/filters.hpp>
#include <boost/log/utility/init/common_attributes.hpp>

#include <boost/log/utility/init/to_console.hpp>
#include <boost/log/utility/init/to_file.hpp>

namespace logging = boost::log;
namespace keywords = boost::log::keywords;
namespace fmt = boost::log::formatters;
namespace src = boost::log::sources;
namespace flt = boost::log::filters;

namespace _SMERP {

// map enum values to strings
template< typename CharT, typename TraitsT >
inline std::basic_ostream< CharT, TraitsT > &operator<< ( std::basic_ostream< CharT, TraitsT >& s, Logger::LogLevel l )
{
	static const char *const str[] = {
		"ALWAYS", "DATA", "TRACE", "DEBUG", "INFO", "NOTICE",
		"WARNING", "ERROR", "SEVERE", "CRITICAL", "ALERT", "FATAL", "NEVER" };
	if( static_cast< size_t >( l ) < ( sizeof( str ) / sizeof( *str ) ) ) {
		s << str[l];
	} else {
		s << "Unknown enum used '" << static_cast< int >( l ) << "'";
	}
	return s;
}

Logger::LogLevel Logger::str2LogLevel( const std::string s ) {
	if( s == "ALWAYS" )		return Logger::ALWAYS;
	else if( s == "DATA" )		return Logger::DATA;
	else if( s == "TRACE" )		return Logger::TRACE;
	else if( s == "DEBUG" )		return Logger::DEBUG;
	else if( s == "INFO" )		return Logger::INFO;
	else if( s == "NOTICE" )	return Logger::NOTICE;
	else if( s == "WARNING" )	return Logger::WARNING;
	else if( s == "ERROR" )		return Logger::ERROR;
	else if( s == "SEVERE" )	return Logger::SEVERE;
	else if( s == "CRITICAL" )	return Logger::CRITICAL;
	else if( s == "ALERT" )		return Logger::ALERT;
	else if( s == "FATAL" )		return Logger::FATAL;
	else if( s == "NEVER" )		return Logger::NEVER;
	else return Logger::NEVER;
}

src::severity_logger< Logger::LogLevel > logger;

void Logger::initialize( const ApplicationConfiguration& config )
{
	// open logger to the console
	logging::init_log_to_console(
		std::clog,
		keywords::filter = flt::attr< LogLevel >( "Severity", std::nothrow ) >= Logger::str2LogLevel( config.stderrLogLevel ),
		keywords::format = fmt::format( "%1% %2%: %3%" )
			% fmt::date_time( "TimeStamp", std::nothrow )
			% fmt::attr< LogLevel >( "Severity", std::nothrow )
			% fmt::message( )
	);

	// open logger to a logfile
	if( config.logFile != std::string( ) ) {
		logging::init_log_to_file(
			keywords::file_name = config.logFile,
			keywords::open_mode = ( std::ios_base::out | std::ios_base::app ),
			keywords::filter = flt::attr< LogLevel >( "Severity", std::nothrow ) >= Logger::str2LogLevel( config.logFileLogLevel ),
			keywords::format = fmt::format( "%1% %2%: %3%" )
				% fmt::date_time( "TimeStamp", std::nothrow )
				% fmt::attr< LogLevel >( "Severity", std::nothrow )
				% fmt::message( )
		);
	}

	logging::add_common_attributes( );

	LOG_DEBUG << "Logger started";
	
}

} // namespace _SMERP
