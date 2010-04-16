//
// logger.cpp
//

#include "logLevel.hpp"
#include "logger.hpp"

#include <boost/log/common.hpp>
#include <boost/log/formatters.hpp>
#include <boost/log/filters.hpp>
#include <boost/log/utility/init/common_attributes.hpp>
#include <boost/algorithm/string.hpp>

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

#if !defined( _WIN32 )
static sinks::syslog::facility_t str2syslogFacility( const std::string str ) {
	std::string s = str;
	boost::trim( s );
	boost::to_upper( s );

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

src::severity_logger< LogLevel::Level > logger;

void Logger::initialize( const ApplicationConfiguration& config )
{
	// open logger to the console
	if( config.logToStderr ) {
		logging::init_log_to_console(
			std::clog,
			keywords::filter = flt::attr< LogLevel::Level >( "Severity", std::nothrow ) >= config.stderrLogLevel,
			keywords::format = fmt::format( "%1%: %2%" )
				% fmt::attr< LogLevel::Level >( "Severity", std::nothrow )
				% fmt::message( )
		);
	}

	// open logger to a logfile
	if( config.logToFile ) {
		logging::init_log_to_file(
			keywords::file_name = config.logFile,
			keywords::auto_flush = true,
			keywords::open_mode = ( std::ios_base::out | std::ios_base::app ),
			keywords::filter = flt::attr< LogLevel::Level >( "Severity", std::nothrow ) >= config.logFileLogLevel,
			keywords::format = fmt::format( "%1% %2%: %3%" )
				% fmt::date_time( "TimeStamp", std::nothrow )
				% fmt::attr< LogLevel::Level >( "Severity", std::nothrow )
				% fmt::message( )
		);
	}

#if !defined( _WIN32 )
	if( config.logToSyslog ) {
		sinks::syslog::custom_severity_mapping< LogLevel::Level > mapping( "Severity" );
		mapping[LogLevel::_SMERP_FATAL] = sinks::syslog::emergency;
		mapping[LogLevel::_SMERP_ALERT] = sinks::syslog::alert;
		mapping[LogLevel::_SMERP_CRITICAL] = sinks::syslog::critical;
		mapping[LogLevel::_SMERP_SEVERE] = sinks::syslog::critical;
		mapping[LogLevel::_SMERP_ERROR] = sinks::syslog::error;
		mapping[LogLevel::_SMERP_WARNING] = sinks::syslog::warning;
		mapping[LogLevel::_SMERP_NOTICE] = sinks::syslog::notice;
		mapping[LogLevel::_SMERP_INFO] = sinks::syslog::info;
		mapping[LogLevel::_SMERP_DEBUG] = sinks::syslog::debug;
		mapping[LogLevel::_SMERP_TRACE] = sinks::syslog::debug;
		mapping[LogLevel::_SMERP_DATA] = sinks::syslog::debug;

		logging::init_log_to_syslog(
			keywords::facility = str2syslogFacility( config.syslogFacility ),
			keywords::use_impl = sinks::syslog::native,
			keywords::custom_severity_mapping = mapping,
			keywords::filter = flt::attr< LogLevel::Level >( "Severity", nothrow ) >= config.syslogLogLevel,
			keywords::format = fmt::format( "smerpd[%1%]: %2%" )
				% fmt::attr< boost::log::aux::process::id >( "ProcessID", "%d", std::nothrow )
//				% fmt::attr< LogLevel >( "Severity", std::nothrow )
				% fmt::message( )
		);
	}
#else
	if( config.logToEventlog ) {
		sinks::event_log::custom_event_type_mapping< LogLevel::Level > mapping( "Severity" );
		mapping[LogLevel::_SMERP_FATAL] = sinks::event_log::error;
		mapping[LogLevel::_SMERP_ALERT] = sinks::event_log::error;
		mapping[LogLevel::_SMERP_CRITICAL] = sinks::event_log::error;
		mapping[LogLevel::_SMERP_SEVERE] = sinks::event_log::error;
		mapping[LogLevel::_SMERP_ERROR] = sinks::event_log::error;
		mapping[LogLevel::_SMERP_WARNING] = sinks::event_log::warning;
		mapping[LogLevel::_SMERP_NOTICE] = sinks::event_log::info;
		mapping[LogLevel::_SMERP_INFO] = sinks::event_log::info;
		mapping[LogLevel::_SMERP_DEBUG] = sinks::event_log::info;
		mapping[LogLevel::_SMERP_TRACE] = sinks::event_log::info;
		mapping[LogLevel::_SMERP_DATA] = sinks::event_log::info;

		logging::init_log_to_eventlog(
			keywords::registration = sinks::event_log::forced,
			keywords::log_name = config.eventlogLogName,
			keywords::log_source = config.eventlogSource,
			keywords::custom_event_type_mapping = mapping,
			keywords::filter = flt::attr< LogLevel::Level >( "Severity", nothrow ) >= config.eventlogLogLevel
		);
	}
#endif // !defined( _WIN32 )

	logging::add_common_attributes( );

	if( config.logToStderr )
		LOG_DEBUG << "Initialized stderr logger with level '" <<  config.stderrLogLevel << "'";
	if( config.logToFile )
		LOG_DEBUG << "Initialized file logger to '" << config.logFile <<"' with level '" <<  config.logFileLogLevel << "'";
#if !defined( _WIN32 )
	if( config.logToSyslog )
		LOG_DEBUG << "Initialized syslog logger to facility '" << config.syslogFacility
			  << "' with level '" <<  config.syslogLogLevel << "'";
#else
	if( config.logToEventlog )
		LOG_DEBUG << "Initialized eventlog logger to log with name '" << config.eventlogLogName << "'"
			  << " with log source '" <<  config.eventlogSource << "' and level '" <<  config.eventlogLogLevel << "'";
#endif // !defined( _WIN32 )
}

} // namespace _SMERP
