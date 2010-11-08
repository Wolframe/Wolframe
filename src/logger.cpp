//
// logger.cpp
//

#define NO_LOG_MACROS
#include "logger.hpp"

#ifndef _WIN32
#include <syslog.h>
#endif // _WIN32

namespace _SMERP {

#ifndef _WIN32
	int SyslogBackend::levelToSyslogLevel( const LogLevel::Level level )	{
		switch( level )	{
			case _SMERP::LogLevel::LOGLEVEL_DATA:
			case _SMERP::LogLevel::LOGLEVEL_TRACE:
			case _SMERP::LogLevel::LOGLEVEL_DEBUG:		return LOG_DEBUG;
			case _SMERP::LogLevel::LOGLEVEL_INFO:		return LOG_INFO;
			case _SMERP::LogLevel::LOGLEVEL_NOTICE:		return LOG_NOTICE;
			case _SMERP::LogLevel::LOGLEVEL_WARNING:	return LOG_WARNING;
			case _SMERP::LogLevel::LOGLEVEL_ERROR:		return LOG_ERR;
			case _SMERP::LogLevel::LOGLEVEL_SEVERE:
			case _SMERP::LogLevel::LOGLEVEL_CRITICAL:	return LOG_CRIT;
			case _SMERP::LogLevel::LOGLEVEL_ALERT:		return LOG_ALERT;
			case _SMERP::LogLevel::LOGLEVEL_FATAL:		return LOG_EMERG;
			case _SMERP::LogLevel::LOGLEVEL_UNDEFINED:	return LOG_ERR;
		}
		return LOG_ERR;
	}

	int SyslogBackend::facilityToSyslogFacility( const SyslogFacility::Facility facility )	{
		switch( facility ) {
			case _SMERP::SyslogFacility::_SMERP_SYSLOG_FACILITY_KERN:	return LOG_KERN;
			case _SMERP::SyslogFacility::_SMERP_SYSLOG_FACILITY_USER:	return LOG_USER;
			case _SMERP::SyslogFacility::_SMERP_SYSLOG_FACILITY_MAIL:	return LOG_MAIL;
			case _SMERP::SyslogFacility::_SMERP_SYSLOG_FACILITY_DAEMON:	return LOG_DAEMON;
			case _SMERP::SyslogFacility::_SMERP_SYSLOG_FACILITY_AUTH:	return LOG_AUTH;
			case _SMERP::SyslogFacility::_SMERP_SYSLOG_FACILITY_SYSLOG:	return LOG_SYSLOG;
			case _SMERP::SyslogFacility::_SMERP_SYSLOG_FACILITY_LPR:	return LOG_LPR;
			case _SMERP::SyslogFacility::_SMERP_SYSLOG_FACILITY_NEWS:	return LOG_NEWS;
			case _SMERP::SyslogFacility::_SMERP_SYSLOG_FACILITY_UUCP:	return LOG_UUCP;
			case _SMERP::SyslogFacility::_SMERP_SYSLOG_FACILITY_CRON:	return LOG_CRON;
#if defined LOG_AUTHPRIV
			case _SMERP::SyslogFacility::_SMERP_SYSLOG_FACILITY_AUTHPRIV:	return LOG_AUTHPRIV;
#else
			case _SMERP::SyslogFacility::_SMERP_SYSLOG_FACILITY_AUTHPRIV:	return LOG_AUTH;
#endif
#if defined LOG_FTP
			case _SMERP::SyslogFacility::_SMERP_SYSLOG_FACILITY_FTP:	return LOG_FTP;
#else
			case _SMERP::SyslogFacility::_SMERP_SYSLOG_FACILITY_FTP:	return LOG_DAEMON;
#endif
#if defined LOG_NTP
			case _SMERP::SyslogFacility::_SMERP_SYSLOG_FACILITY_NTP:	return LOG_NTP;
#else
			case _SMERP::SyslogFacility::_SMERP_SYSLOG_FACILITY_NTP:	return LOG_DAEMON;
#endif
#if defined LOG_SECURITY
			case _SMERP::SyslogFacility::_SMERP_SYSLOG_FACILITY_SECURITY:	return LOG_SECURITY;
#else
			case _SMERP::SyslogFacility::_SMERP_SYSLOG_FACILITY_SECURITY:	return LOG_AUTH;
#endif
#if defined LOG_CONSOLE
			case _SMERP::SyslogFacility::_SMERP_SYSLOG_FACILITY_CONSOLE:	return LOG_CONSOLE;
#else
			case _SMERP::SyslogFacility::_SMERP_SYSLOG_FACILITY_CONSOLE:	return LOG_DAEMON;
#endif
#if defined LOG_AUDIT
			case _SMERP::SyslogFacility::_SMERP_SYSLOG_FACILITY_AUDIT:	return LOG_AUDIT;
#else
			case _SMERP::SyslogFacility::_SMERP_SYSLOG_FACILITY_AUDIT:	return LOG_AUTH;
#endif
			case _SMERP::SyslogFacility::_SMERP_SYSLOG_FACILITY_LOCAL0:	return LOG_LOCAL0;
			case _SMERP::SyslogFacility::_SMERP_SYSLOG_FACILITY_LOCAL1:	return LOG_LOCAL1;
			case _SMERP::SyslogFacility::_SMERP_SYSLOG_FACILITY_LOCAL2:	return LOG_LOCAL2;
			case _SMERP::SyslogFacility::_SMERP_SYSLOG_FACILITY_LOCAL3:	return LOG_LOCAL3;
			case _SMERP::SyslogFacility::_SMERP_SYSLOG_FACILITY_LOCAL4:	return LOG_LOCAL4;
			case _SMERP::SyslogFacility::_SMERP_SYSLOG_FACILITY_LOCAL5:	return LOG_LOCAL5;
			case _SMERP::SyslogFacility::_SMERP_SYSLOG_FACILITY_LOCAL6:	return LOG_LOCAL6;
			case _SMERP::SyslogFacility::_SMERP_SYSLOG_FACILITY_LOCAL7:	return LOG_LOCAL7;	
			case _SMERP::SyslogFacility::_SMERP_SYSLOG_FACILITY_UNDEFINED:	return LOG_DAEMON;		
		}
		return LOG_DAEMON;
	}

#endif // _WIN32

#if 0

void Logger::initialize( const ApplicationConfiguration& config )
{

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

	if( config.logToEventlog ) {
		sinks::event_log::custom_event_type_mapping< LogLevel::Level > mapping( "Severity" );
		mapping[LogLevel::LOGLEVEL_FATAL] = sinks::event_log::error;
		mapping[LogLevel::LOGLEVEL_ALERT] = sinks::event_log::error;
		mapping[LogLevel::LOGLEVEL_CRITICAL] = sinks::event_log::error;
		mapping[LogLevel::LOGLEVEL_SEVERE] = sinks::event_log::error;
		mapping[LogLevel::LOGLEVEL_ERROR] = sinks::event_log::error;
		mapping[LogLevel::LOGLEVEL_WARNING] = sinks::event_log::warning;
		mapping[LogLevel::LOGLEVEL_NOTICE] = sinks::event_log::info;
		mapping[LogLevel::LOGLEVEL_INFO] = sinks::event_log::info;
		mapping[LogLevel::LOGLEVEL_DEBUG] = sinks::event_log::info;
		mapping[LogLevel::LOGLEVEL_TRACE] = sinks::event_log::info;
		mapping[LogLevel::LOGLEVEL_DATA] = sinks::event_log::info;

		logging::init_log_to_eventlog(
			keywords::registration = sinks::event_log::forced,
			keywords::log_name = config.eventlogLogName,
			keywords::log_source = config.eventlogSource,
			keywords::custom_event_type_mapping = mapping,
			keywords::filter = flt::attr< LogLevel::Level >( "Severity", nothrow ) >= config.eventlogLogLevel
		);
	}

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

#endif

} // namespace _SMERP
