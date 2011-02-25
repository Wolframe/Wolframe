//
// logger.cpp
//

#include "logger.hpp"
#include "logBackend.hpp"
#include "logComponent.hpp"

// no macros here, name clash with variables in syslog.h, so
// undefine them here..
// also they should not be used here because of reentrance problems
#undef LOG_DATA
#undef LOG_TRACE
#undef LOG_DEBUG
#undef LOG_INFO
#undef LOG_NOTICE
#undef LOG_WARNING
#undef LOG_ERROR
#undef LOG_SEVERE
#undef LOG_CRITICAL
#undef LOG_ALERT
#undef LOG_FATAL

#include <ostream>

#if !defined( _WIN32 )
#include <syslog.h>
#include <sys/time.h>
#endif // !defined( _WIN32 )

#if defined( _WIN32 )
#include "smerpmsg.h"
#endif // defined( _WIN32 )

#include <sstream>

#include "unused.h"

namespace _SMERP {

// ConsoleLogBackend

ConsoleLogBackend::ConsoleLogBackend( )
{
	logLevel_ = _SMERP::LogLevel::LOGLEVEL_ERROR;
}

ConsoleLogBackend::~ConsoleLogBackend( )
{
	/* nothing to do here */
}

void ConsoleLogBackend::setLevel( const LogLevel::Level level )
{
	logLevel_ = level;
}

inline void ConsoleLogBackend::log( SMERP_UNUSED const LogComponent::Component component, const LogLevel::Level level, const std::string& msg )
{
	if ( level >= logLevel_ ) {
		std::cerr << level << ": " << msg << std::endl;
		std::cerr.flush();
	}
}

void ConsoleLogBackend::reopen( )
{
	/* nothing to do here */
}

// LogfileBackend

LogfileBackend::LogfileBackend( )
{
	logLevel_ = _SMERP::LogLevel::LOGLEVEL_UNDEFINED;
	isOpen_ = false;
	// we don't open a primarily unknown logfile, wait for setFilename
}

LogfileBackend::~LogfileBackend( )
{
	if( isOpen_ ) {
		logFile_.close( );
	}
}

void LogfileBackend::setLevel( const LogLevel::Level level )
{
	logLevel_ = level;
}

void LogfileBackend::setFilename( const std::string filename )
{
	filename_ = filename;
	reopen( );
}

void LogfileBackend::reopen( )
{
	if( isOpen_ ) {
		logFile_.close( );
		isOpen_ = false;
	}

	logFile_.exceptions( logFile_.badbit | logFile_.failbit );

	try {
		logFile_.open( filename_.c_str( ), std::ios_base::app );
		isOpen_ = true;
	} catch( const std::ofstream::failure& e ) {
		//TODO: introduce system exceptions
		std::cerr << "ERROR: " << e.what( ) << std::endl;
	}
}

static inline std::string timestamp( void )
{
#if !defined( _WIN32 )
	time_t t;
	struct tm lt;
	char buf[32];

	time( &t );
	localtime_r( &t, &lt );
	strftime( buf, 32, "%b %e %X", &lt );

	return buf;
#else // !defined( _WIN32)
	SYSTEMTIME t;
	SYSTEMTIME lt;
	TCHAR buf1[16];
	TCHAR buf2[16];

	GetSystemTime( &t );
	GetLocalTime( &lt );

	(void)GetDateFormat( LOCALE_USER_DEFAULT, 0, &lt, NULL, buf1, 16 );
	(void)GetTimeFormat( LOCALE_USER_DEFAULT, 0, &lt, NULL, buf2, 16 );

	std::ostringstream oss;
	oss << buf1 << " " << buf2;
	return oss.str( );
#endif // !defined( _WIN32 )
}

inline void LogfileBackend::log( const LogComponent::Component component, const LogLevel::Level level, const std::string& msg )
{
	if( level >= logLevel_ && isOpen_ ) {
		logFile_	<< timestamp( ) << " "
				<< component << ( component == LogComponent::LOGCOMPONENT_NONE ? "" : " - " )
				<< level << ": " << msg << std::endl;
		logFile_.flush( );
	}
}

// SyslogBackend

#if !defined( _WIN32 )
SyslogBackend::SyslogBackend( )
{
	logLevel_ = _SMERP::LogLevel::LOGLEVEL_UNDEFINED;
	ident_ = "<undefined>";
	facility_ = _SMERP::SyslogFacility::_SMERP_SYSLOG_FACILITY_DAEMON;
	openlog( ident_.c_str( ), LOG_CONS | LOG_PID, facility_ );
}

SyslogBackend::~SyslogBackend( )
{
	closelog( );
}

static int levelToSyslogLevel( const LogLevel::Level level )
{
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

static int facilityToSyslogFacility( const SyslogFacility::Facility facility )
{
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

void SyslogBackend::setLevel( const LogLevel::Level level )
{
	logLevel_ = level;
}

void SyslogBackend::setFacility( const SyslogFacility::Facility facility )
{
	facility_ = facilityToSyslogFacility( facility );
	reopen( );
}

void SyslogBackend::setIdent( const std::string ident )
{
	ident_ = ident;
	reopen( );
}

inline void SyslogBackend::log( const LogComponent::Component component, const LogLevel::Level level, const std::string& msg )
{
	if ( level >= logLevel_ ) {
		std::ostringstream os;
		os	<< component << ( component == LogComponent::LOGCOMPONENT_NONE ? "" : " - " )
			<< msg;
		syslog( levelToSyslogLevel( level ), "%s", os.str( ).c_str( ) );
	}
}

void SyslogBackend::reopen( )
{
	closelog( );
	openlog( ident_.c_str( ), LOG_CONS | LOG_PID, facility_ );
}

#endif // !defined( _WIN32 )

// EventlogBackend

#if defined( _WIN32 )
EventlogBackend::EventlogBackend( )
{
	logLevel_ = _SMERP::LogLevel::LOGLEVEL_UNDEFINED;
	categoryId_ = 1 | 0x0FFF0000L; // the one category we have at the moment in the resource
	log_ = "Application";
	source_ = "<undefined>";
	eventSource_ = RegisterEventSource( NULL, source_.c_str( ) );
}

EventlogBackend::~EventlogBackend( )
{
	if( eventSource_ ) {
		(void)DeregisterEventSource( eventSource_ );
		eventSource_ = 0;
	}
}

void EventlogBackend::setLevel( const LogLevel::Level level )
{
	logLevel_ = level;
}

void EventlogBackend::setLog( const std::string log )
{
	log_ = log;
	reopen( );
}

void EventlogBackend::setSource( const std::string source )
{
	source_ = source;
	reopen( );
}

static DWORD levelToEventlogLevel( const LogLevel::Level level )
{
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
static DWORD messageIdToEventlogId( DWORD eventLogLevel )
{
	DWORD mask = 0;
	DWORD eventId;

	switch( eventLogLevel ) {
		case EVENTLOG_ERROR_TYPE:
			eventId = SMERP_EVENTID_ERROR; mask = 3; break;
		case EVENTLOG_WARNING_TYPE:
			eventId = SMERP_EVENTID_WARNING; mask = 2; break;
		case EVENTLOG_INFORMATION_TYPE:
			eventId = SMERP_EVENTID_INFO; mask = 1; break;
		default:
			eventId = SMERP_EVENTID_ERROR; mask = 3;
	}
	return( eventId | 0x0FFF0000L | ( mask << 30 ) );
}

inline void EventlogBackend::log( const LogLevel::Level level, const std::string& msg )
{
	if ( level >= logLevel_ ) {
		LPCSTR msg_arr[1];
		msg_arr[0] = (LPSTR)msg.c_str( );
		(void)ReportEvent(
			eventSource_,
			levelToEventlogLevel( level ),
			categoryId_,
			messageIdToEventlogId( level ),
			NULL, // SID of the user owning the process, not now, later..
			1, // at the moment no strings to replace, just the message itself
			0, // no binary data
			msg_arr, // array of strings to log (msg.c_str() for now)
			NULL ); // no binary data
	}
}

void EventlogBackend::reopen( )
{
	if( eventSource_ )
		(void)DeregisterEventSource( eventSource_ );
	eventSource_ = RegisterEventSource( NULL, source_.c_str( ) );
}

#endif // defined( _WIN32 )

// LogBackend

LogBackend::LogBackendImpl::LogBackendImpl( )
{
	/* logger are members, implicit initialization */
}

LogBackend::LogBackendImpl::~LogBackendImpl( )
{
	/* logger resources freed in destructors of members */
}

void LogBackend::LogBackendImpl::setConsoleLevel( const LogLevel::Level level )
{
	consoleLogger_.setLevel( level );
}

void LogBackend::LogBackendImpl::setLogfileLevel( const LogLevel::Level level )
{
	logfileLogger_.setLevel( level );
}

void LogBackend::LogBackendImpl::setLogfileName( const std::string filename )
{
	logfileLogger_.setFilename( filename );
}

#if !defined( _WIN32 )
void LogBackend::LogBackendImpl::setSyslogLevel( const LogLevel::Level level )
{
	syslogLogger_.setLevel( level );
}

void LogBackend::LogBackendImpl::setSyslogFacility( const SyslogFacility::Facility facility )
{
	syslogLogger_.setFacility( facility );
}

void LogBackend::LogBackendImpl::setSyslogIdent( const std::string ident )
{
	syslogLogger_.setIdent( ident );
}
#endif // !defined( _WIN32 )

#if defined( _WIN32 )
void LogBackend::LogBackendImpl::setEventlogLevel( const LogLevel::Level level )
{
	eventlogLogger_.setLevel( level );
}

void LogBackend::LogBackendImpl::setEventlogLog( const std::string log )
{
	eventlogLogger_.setLog( log );
}

void LogBackend::LogBackendImpl::setEventlogSource( const std::string source )
{
	eventlogLogger_.setSource( source );
}
#endif // defined( _WIN32 )

inline void LogBackend::LogBackendImpl::log( const LogComponent::Component component, const LogLevel::Level level, const std::string& msg )
{
	consoleLogger_.log( component, level, msg );
	logfileLogger_.log( component, level, msg );
#if !defined( _WIN32 )
	syslogLogger_.log( component, level, msg );
#endif // !defined( _WIN32 )
#if defined( _WIN32 )
	eventlogLogger_.log( component, level, msg );
#endif // defined( _WIN32 )
}


// Log backend PIMPL redirection
LogBackend::LogBackend() : impl_( new LogBackendImpl )	{}

LogBackend::~LogBackend()	{ delete impl_; }

void LogBackend::setConsoleLevel( const LogLevel::Level level )	{ impl_->setConsoleLevel( level ); }

void LogBackend::setLogfileLevel( const LogLevel::Level level )	{ impl_->setLogfileLevel( level ); }

void LogBackend::setLogfileName( const std::string filename )	{ impl_->setLogfileName( filename );}

#ifndef _WIN32
void LogBackend::setSyslogLevel( const LogLevel::Level level )	{ impl_->setSyslogLevel( level );}

void LogBackend::setSyslogFacility( const SyslogFacility::Facility facility )	{ impl_->setSyslogFacility( facility );}

void LogBackend::setSyslogIdent( const std::string ident )	{ impl_->setSyslogIdent( ident );}
#endif // _WIN32

#ifdef _WIN32
void LogBackend::setEventlogLevel( const LogLevel::Level level )	{ impl_->setEventlogLevel( level );}

void LogBackend::setEventlogLog( const std::string log )	{ impl_->setEventlogLog( log ); }

void LogBackend::setEventlogSource( const std::string source )	{ impl_->setEventlogSource( source ); }
#endif // _WIN32

void LogBackend::log( const LogComponent::Component component, const LogLevel::Level level, const std::string& msg )	{ impl_->log( component, level, msg ); }


// Logger

Logger::Logger( LogBackend& backend ) :	logBk_( backend )
{
}

Logger::~Logger( )
{
	logBk_.log( component_, msgLevel_, os_.str( ) );
}

std::ostringstream& Logger::Get( LogComponent::Component component, LogLevel::Level level )
{
	component_ = component;
	msgLevel_ = level;
	return os_;
}

} // namespace _SMERP
