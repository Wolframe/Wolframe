#include "logger.hpp"

#include <iostream>

#include <boost/log/common.hpp>
#include <boost/log/formatters.hpp>
#include <boost/log/filters.hpp>
#include <boost/log/utility/init/common_attributes.hpp>

#include <boost/log/utility/init/to_console.hpp>
#include <boost/log/utility/init/to_file.hpp>
#ifndef _WIN32
#include <boost/log/utility/init/to_syslog.hpp>
#include <boost/log/sinks/syslog_constants.hpp>
#else
#include <boost/log/utility/init/to_eventlog.hpp>
#include <boost/log/sinks/event_log_constants.hpp>
#include <boost/log/sinks/event_log_backend.hpp>
#endif

namespace logging = boost::log;
namespace keywords = boost::log::keywords;
namespace fmt = boost::log::formatters;
namespace src = boost::log::sources;
namespace flt = boost::log::filters;
namespace sinks = boost::log::sinks;

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
	if( s == "ALWAYS" )		return Logger::_SMERP_ALWAYS;
	else if( s == "DATA" )		return Logger::_SMERP_DATA;
	else if( s == "TRACE" )		return Logger::_SMERP_TRACE;
	else if( s == "DEBUG" )		return Logger::_SMERP_DEBUG;
	else if( s == "INFO" )		return Logger::_SMERP_INFO;
	else if( s == "NOTICE" )	return Logger::_SMERP_NOTICE;
	else if( s == "WARNING" )	return Logger::_SMERP_WARNING;
	else if( s == "ERROR" )		return Logger::_SMERP_ERROR;
	else if( s == "SEVERE" )	return Logger::_SMERP_SEVERE;
	else if( s == "CRITICAL" )	return Logger::_SMERP_CRITICAL;
	else if( s == "ALERT" )		return Logger::_SMERP_ALERT;
	else if( s == "FATAL" )		return Logger::_SMERP_FATAL;
	else if( s == "NEVER" )		return Logger::_SMERP_NEVER;
	else				return Logger::_SMERP_NEVER;
}

#if !defined( _WIN32 )
static sinks::syslog::facility_t str2syslogFacility( const std::string s ) {
	if( s == "KERN" )		return sinks::syslog::kernel;
	if( s == "USER" )		return sinks::syslog::user;
	if( s == "MAIL" )		return sinks::syslog::mail;
	if( s == "DAEMON" )		return sinks::syslog::daemon;
	if( s == "AUTH" )		return sinks::syslog::security0;
	if( s == "SYSLOG" )		return sinks::syslog::syslogd;
	if( s == "LPR" )		return sinks::syslog::printer;
	if( s == "NEWS" )		return sinks::syslog::news;
	if( s == "UUCP" )		return sinks::syslog::uucp;
	if( s == "CRON" )		return sinks::syslog::clock0;
	if( s == "AUTHPRIV" )		return sinks::syslog::security1;
	if( s == "FTP" )		return sinks::syslog::ftp;
	if( s == "NTP" )		return sinks::syslog::ntp;
	if( s == "SECURITY" )		return sinks::syslog::log_audit;
	if( s == "LOCAL0" )		return sinks::syslog::local0;
	if( s == "LOCAL1" )		return sinks::syslog::local1;
	if( s == "LOCAL2" )		return sinks::syslog::local2;
	if( s == "LOCAL3" )		return sinks::syslog::local3;
	if( s == "LOCAL4" )		return sinks::syslog::local4;
	if( s == "LOCAL5" )		return sinks::syslog::local5;
	if( s == "LOCAL6" )		return sinks::syslog::local6;
	if( s == "LOCAL7" )		return sinks::syslog::local7;
	else 				return sinks::syslog::user;
}
#endif // !defined( _WIN32 )

src::severity_logger< Logger::LogLevel > logger;

void Logger::initialize( const ApplicationConfiguration& config )
{
	// open logger to the console
	if( config.logToStderr ) {
		logging::init_log_to_console(
			std::clog,
			keywords::filter = flt::attr< LogLevel >( "Severity", std::nothrow ) >= Logger::str2LogLevel( config.stderrLogLevel ),
			keywords::format = fmt::format( "%1%: %2%" )
				% fmt::attr< LogLevel >( "Severity", std::nothrow )
				% fmt::message( )
		);

		LOG_DEBUG << "Initialized stderr logger with level '" <<  config.stderrLogLevel << "'";
	}

	// open logger to a logfile
	if( config.logToFile ) {
		logging::init_log_to_file(
			keywords::file_name = config.logFile,
			keywords::open_mode = ( std::ios_base::out | std::ios_base::app ),
			keywords::filter = flt::attr< LogLevel >( "Severity", std::nothrow ) >= Logger::str2LogLevel( config.logFileLogLevel ),
			keywords::format = fmt::format( "%1% %2%: %3%" )
				% fmt::date_time( "TimeStamp", std::nothrow )
				% fmt::attr< LogLevel >( "Severity", std::nothrow )
				% fmt::message( )
		);

		LOG_DEBUG << "Initialized file logger to '" << config.logFile <<"' with level " <<  config.logFileLogLevel << "'";
	}

#if !defined( _WIN32 )
	if( config.logToSyslog ) {
		sinks::syslog::custom_severity_mapping< LogLevel > mapping( "Severity" );
		mapping[Logger::_SMERP_FATAL] = sinks::syslog::emergency;
		mapping[Logger::_SMERP_ALERT] = sinks::syslog::alert;
		mapping[Logger::_SMERP_CRITICAL] = sinks::syslog::critical;
		mapping[Logger::_SMERP_SEVERE] = sinks::syslog::critical;
		mapping[Logger::_SMERP_ERROR] = sinks::syslog::error;
		mapping[Logger::_SMERP_WARNING] = sinks::syslog::warning;
		mapping[Logger::_SMERP_NOTICE] = sinks::syslog::notice;
		mapping[Logger::_SMERP_INFO] = sinks::syslog::info;
		mapping[Logger::_SMERP_DEBUG] = sinks::syslog::debug;
		mapping[Logger::_SMERP_TRACE] = sinks::syslog::debug;
		mapping[Logger::_SMERP_DATA] = sinks::syslog::debug;

		logging::init_log_to_syslog(
			keywords::facility = str2syslogFacility( config.syslogFacility ),
			keywords::use_impl = sinks::syslog::native,
			keywords::custom_severity_mapping = mapping,
			keywords::filter = flt::attr< LogLevel >( "Severity", nothrow ) >= Logger::str2LogLevel( config.syslogLogLevel ),
			keywords::format = fmt::format( "%1%: %2%" )
				% fmt::attr< LogLevel >( "Severity", std::nothrow )
				% fmt::message( )
		);

		LOG_DEBUG << "Initialized syslog logger to facility '" << config.syslogFacility
		          << "' with level '" <<  config.syslogLogLevel << "'";
	}
#else
	if( config.logToEventlog ) {
		sinks::event_log::custom_event_type_mapping< LogLevel > mapping( "Severity" );
		mapping[Logger::_SMERP_FATAL] = sinks::event_log::error;
		mapping[Logger::_SMERP_ALERT] = sinks::event_log::error;
		mapping[Logger::_SMERP_CRITICAL] = sinks::event_log::error;
		mapping[Logger::_SMERP_SEVERE] = sinks::event_log::error;
		mapping[Logger::_SMERP_ERROR] = sinks::event_log::error;
		mapping[Logger::_SMERP_WARNING] = sinks::event_log::warning;
		mapping[Logger::_SMERP_NOTICE] = sinks::event_log::info;
		mapping[Logger::_SMERP_INFO] = sinks::event_log::info;
		mapping[Logger::_SMERP_DEBUG] = sinks::event_log::info;
		mapping[Logger::_SMERP_TRACE] = sinks::event_log::info;
		mapping[Logger::_SMERP_DATA] = sinks::event_log::info;

		logging::init_log_to_eventlog(
			keywords::registration = sinks::event_log::forced,
			keywords::log_name = config.eventlogLogName,
			keywords::log_source = config.eventlogSource,
			keywords::custom_event_type_mapping = mapping,
			keywords::filter = flt::attr< LogLevel >( "Severity", nothrow ) >= Logger::str2LogLevel( config.eventlogLogLevel )
		);
	}
#endif // !defined( _WIN32 )

	logging::add_common_attributes( );
}

} // namespace _SMERP
