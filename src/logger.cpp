//
// logger.cpp
//

#define NO_LOG_MACROS
#include "logger.hpp"

#if !defined( _WIN32 )
#include <syslog.h>
#endif // !defined( _WIN32 )

namespace _SMERP {

#if !defined( _WIN32 )
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

#endif // !defined( _WIN32 )

#if defined( _WIN32 )

	DWORD EventlogBackend::levelToEventlogLevel( const LogLevel::Level level )	{
		switch( level )	{
			case _SMERP::LogLevel::LOGLEVEL_DATA:
			case _SMERP::LogLevel::LOGLEVEL_TRACE:
			case _SMERP::LogLevel::LOGLEVEL_DEBUG:
			case _SMERP::LogLevel::LOGLEVEL_INFO:
			case _SMERP::LogLevel::LOGLEVEL_NOTICE:
				return EVENTLOG_INFORMATION_TYPE;
			case _SMERP::LogLevel::LOGLEVEL_WARNING:
				return EVENTLOG_WARNING_TYPE;
			case _SMERP::LogLevel::LOGLEVEL_ERROR:
			case _SMERP::LogLevel::LOGLEVEL_SEVERE:
			case _SMERP::LogLevel::LOGLEVEL_CRITICAL:
			case _SMERP::LogLevel::LOGLEVEL_ALERT:
			case _SMERP::LogLevel::LOGLEVEL_FATAL:
			case _SMERP::LogLevel::LOGLEVEL_UNDEFINED:
				return EVENTLOG_ERROR_TYPE;
		}
		return EVENTLOG_ERROR_TYPE;
	}
	
	// 00 - Success			0
	// 01 - Informational		4
	// 10 - Warning			2
	// 11 - Error			1
	DWORD EventlogBackend::messageIdToEventlogId( DWORD eventLogLevel, int messageId ) {
		DWORD mask = 0;

		switch( eventLogLevel ) {
			case EVENTLOG_ERROR_TYPE:	mask = 3; break;
			case EVENTLOG_WARNING_TYPE:	mask = 2; break;
			case EVENTLOG_INFORMATION_TYPE:	mask = 1; break;
			default:			mask = 3;
		}
		return( messageId | 0x0FFF0000L | ( mask << 30 ) );
	}


#if 0
	void EventlogBackend::registrySetString( HKEY h, TCHAR *name, TCHAR *value ) {
		RegSetValueEx( h, name, 0, REG_EXPAND_SZ, (LPBYTE)value, strlen( value ) );
	}

	void EventlogBackend::registrySetWord( HKEY h, TCHAR *name, DWORD value ) {
		RegSetValueEx( h, name, 0, REG_DWORD, (LPBYTE)&value, sizeof( DWORD ) );
	}
		void registerEventSource( );
		void registrySetString( HKEY h, TCHAR *name, TCHAR *value );
		void registrySetWord( HKEY h, TCHAR *name, DWORD value );

	void EventlogBackend::registerEventSource( )	{
	/* fiddle in the registry and register the location of the
	 * message DLL, how many categories we define and what types
	 * of events we are supporting
	 */

	eventlog_server = server;
	eventlog_log = log;
	eventlog_source = source;
	eventlog_level = level;

	register_event_source( log, source, path_to_dll, nof_categories );

	char key[256];
	HKEY h = 0;
	DWORD disposition;

	/* compose the registry key and simply overwrite the values there, we know */
	snprintf( key, 256, "SYSTEM\\CurrentControlSet\\Services\\EventLog\\%s\\%s", log, source );
	RegCreateKeyEx( HKEY_LOCAL_MACHINE, key, 0, NULL, REG_OPTION_NON_VOLATILE,
		KEY_SET_VALUE, NULL, &h, &disposition );

	/* make sure not to have hard-coded pathes here, otherwise remote
	 * event logging will not work! */
	registry_set_expandable_string( h, "EventMessageFile", "C:\\TEMP\\smerp.dll" );
	registry_set_expandable_string( h, "CategoryMessageFile", "C:\\TEMP\\smerp.dll" );
	registry_set_word( h, "TypesSupported", (DWORD)7 );
	registry_set_word( h, "CategoryCount", (DWORD)1 ); // currently we have only one category
	RegCloseKey( h );
}
#endif
	
#endif // defined( _WIN32 )
	
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

	if( config.logToStderr )
		LOG_DEBUG << "Initialized stderr logger with level '" <<  config.stderrLogLevel << "'";
	if( config.logToFile )
		LOG_DEBUG << "Initialized file logger to '" << config.logFile <<"' with level '" <<  config.logFileLogLevel << "'";
	if( config.logToSyslog )
		LOG_DEBUG << "Initialized syslog logger to facility '" << config.syslogFacility
			  << "' with level '" <<  config.syslogLogLevel << "'";
	if( config.logToEventlog )
		LOG_DEBUG << "Initialized eventlog logger to log with name '" << config.eventlogLogName << "'"
			  << " with log source '" <<  config.eventlogSource << "' and level '" <<  config.eventlogLogLevel << "'";
}

#endif

} // namespace _SMERP
