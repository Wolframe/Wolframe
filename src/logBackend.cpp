//
// logger.cpp
//

#include <ostream>
#include <iostream>
#include <sstream>

#include "logger.hpp"
#include "logBackend.hpp"

// no macros here, name clash with variables in syslog.h, so
// undefine them here..
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

// our private shortcut macros
#define _LOG_DATA	_Wolframe::Logging::Logger( _Wolframe::Logging::LogBackend::instance() ).Get( _Wolframe::Logging::LogLevel::LOGLEVEL_DATA )
#define _LOG_TRACE	_Wolframe::Logging::Logger( _Wolframe::Logging::LogBackend::instance() ).Get( _Wolframe::Logging::LogLevel::LOGLEVEL_TRACE )
#define _LOG_DEBUG	_Wolframe::Logging::Logger( _Wolframe::Logging::LogBackend::instance() ).Get( _Wolframe::Logging::LogLevel::LOGLEVEL_DEBUG )
#define _LOG_INFO	_Wolframe::Logging::Logger( _Wolframe::Logging::LogBackend::instance() ).Get( _Wolframe::Logging::LogLevel::LOGLEVEL_INFO )
#define _LOG_NOTICE	_Wolframe::Logging::Logger( _Wolframe::Logging::LogBackend::instance() ).Get( _Wolframe::Logging::LogLevel::LOGLEVEL_NOTICE )
#define _LOG_WARNING	_Wolframe::Logging::Logger( _Wolframe::Logging::LogBackend::instance() ).Get( _Wolframe::Logging::LogLevel::LOGLEVEL_WARNING )
#define _LOG_ERROR	_Wolframe::Logging::Logger( _Wolframe::Logging::LogBackend::instance() ).Get( _Wolframe::Logging::LogLevel::LOGLEVEL_ERROR )
#define _LOG_SEVERE	_Wolframe::Logging::Logger( _Wolframe::Logging::LogBackend::instance() ).Get( _Wolframe::Logging::LogLevel::LOGLEVEL_SEVERE )
#define _LOG_CRITICAL	_Wolframe::Logging::Logger( _Wolframe::Logging::LogBackend::instance() ).Get( _Wolframe::Logging::LogLevel::LOGLEVEL_CRITICAL )
#define _LOG_ALERT	_Wolframe::Logging::Logger( _Wolframe::Logging::LogBackend::instance() ).Get( _Wolframe::Logging::LogLevel::LOGLEVEL_ALERT )
#define _LOG_FATAL	_Wolframe::Logging::Logger( _Wolframe::Logging::LogBackend::instance() ).Get( _Wolframe::Logging::LogLevel::LOGLEVEL_FATAL )

// for strerror, errno, FormatMessage, GetLastError
#ifdef _WIN32
#include <tchar.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <strsafe.h>
#else
#include <errno.h>
#include <cstring>
#endif

#if !defined( _WIN32 )
#include <syslog.h>
#include <sys/time.h>
#endif // !defined( _WIN32 )

#if defined( _WIN32 )
#include "wolframemsg.h"
#endif // defined( _WIN32 )

#include "unused.h"

namespace _Wolframe {
	namespace Logging {

	const LogComponent LogComponent::LogNone( LogComponent::LOGCOMPONENT_NONE );
	const LogComponent LogComponent::LogLogging( LogComponent::LOGCOMPONENT_LOGGING );
	const LogComponent LogComponent::LogNetwork( LogComponent::LOGCOMPONENT_NETWORK );
	const LogComponent LogComponent::LogAuth( LogComponent::LOGCOMPONENT_AUTH );
	const LogComponent LogComponent::LogLua( LogComponent::LOGCOMPONENT_LUA );

	const char* LogComponent::str( ) const {
		static const char *const s[] = {
			"", "Logging", "Network", "Auth", "Lua" };
		if( static_cast< size_t >( _component ) < ( sizeof( s ) / sizeof( *s ) ) ) {
			return s[_component];
		} else {
			return "";
		}
	}

	const Logger::LogStrerrorT Logger::LogStrerror = { 1 };
	const Logger::LogWinerrorT Logger::LogWinerror = { 2 };

	// map components
	Logger& operator<<( Logger& logger, LogComponent c )
	{
		logger.component_ = c;
		return logger;
	}

	// template functions for error markers in the output stream
	// e.g. LOG_ERROR << "f() had a booboo, reason: " << Logger::LogStrerror
#ifndef _WIN32
	Logger& operator<<( Logger& logger, WOLFRAME_UNUSED Logger::LogStrerrorT t )
	{
		char errbuf[512];

#if defined( __USE_GNU )
		char *ss = strerror_r( errno, errbuf, 512 );
		logger.os_ << ss;
#else
		int res = strerror_r( errno, errbuf, 512 );
		logger.os_ << errbuf;
#endif // defined( __USE_GNU )
		logger.os_ << " (errno: " << errno << ")";

		return logger;
	}
#endif // !defined( _WIN32 )

#ifdef _WIN32
	Logger& operator<<( Logger& logger, WOLFRAME_UNUSED Logger::LogWinerrorT t )
	{
		DWORD last_error = GetLastError( );
		TCHAR errbuf[512];
		LPVOID werrbuf;
		DWORD wbuf_size;
		DWORD wres;
		
		wres = FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS |
			FORMAT_MESSAGE_MAX_WIDTH_MASK,
			NULL,			// message is from system
			last_error,		// code of last error (GetLastError)
			0,			// default language (TODO: fit to i18n of rest)
			(LPTSTR)&werrbuf,	// use LocalAlloc for the message string
			0,			// minimal allocation size
			NULL );			// no arguments to the message
			
		if( wres == 0 ) {
			StringCbCopy( errbuf, 512, _T( "No message available" ) );
		}
	
		StringCbCopy( errbuf, 512, (LPCTSTR)werrbuf );
		
		logger.os_ << errbuf;
		
		return logger;
	}
#endif // defined( _WIN32 )

// ConsoleLogBackend

ConsoleLogBackend::ConsoleLogBackend( )
{
	logLevel_ = LogLevel::LOGLEVEL_ERROR;
}

ConsoleLogBackend::~ConsoleLogBackend( )
{
	/* nothing to do here */
}

void ConsoleLogBackend::setLevel( const LogLevel::Level level )
{
	logLevel_ = level;
}

inline void ConsoleLogBackend::log( WOLFRAME_UNUSED const LogComponent component, const LogLevel::Level level, const std::string& msg )
{
	if ( level >= logLevel_ ) {
		std::cerr << level << ": " << msg << std::endl;
		std::cerr.flush( );
	}
}

void ConsoleLogBackend::reopen( )
{
	/* nothing to do here */
}

// LogfileBackend

LogfileBackend::LogfileBackend( )
{
	logLevel_ = LogLevel::LOGLEVEL_UNDEFINED;
	isOpen_ = false;
	// we can't open the logfile here, wait for setFilename
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
	} catch( const std::ofstream::failure& ) {
		isOpen_ = false;
		_LOG_CRITICAL	<< _Wolframe::Logging::LogComponent::LogLogging
				<< "Can't open logfile '" << filename_ << "'";
		// TODO: e.what() displays "basic_ios::clear" always, how to get
		// decent error messages here? I fear the C++ standard doesn't
		// help here..
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

inline void LogfileBackend::log( const LogComponent component, const LogLevel::Level level, const std::string& msg )
{
	if( level >= logLevel_ && isOpen_ ) {
		logFile_	<< timestamp( ) << " "
				<< component.str( )
				<< ( component == LogComponent::LogNone ? "" : " - " )
				<< level << ": " << msg << std::endl;
		logFile_.flush( );
	}
}

// SyslogBackend

#if !defined( _WIN32 )
SyslogBackend::SyslogBackend( )
{
	logLevel_ = LogLevel::LOGLEVEL_UNDEFINED;
	ident_ = "<undefined>";
	facility_ = SyslogFacility::WOLFRAME_SYSLOG_FACILITY_DAEMON;
	openlog( ident_.c_str( ), LOG_CONS | LOG_PID, facility_ );
}

SyslogBackend::~SyslogBackend( )
{
	closelog( );
}

static int levelToSyslogLevel( const LogLevel::Level level )
{
	switch( level )	{
		case LogLevel::LOGLEVEL_DATA:
		case LogLevel::LOGLEVEL_TRACE:
		case LogLevel::LOGLEVEL_DEBUG:		return LOG_DEBUG;
		case LogLevel::LOGLEVEL_INFO:		return LOG_INFO;
		case LogLevel::LOGLEVEL_NOTICE:		return LOG_NOTICE;
		case LogLevel::LOGLEVEL_WARNING:	return LOG_WARNING;
		case LogLevel::LOGLEVEL_ERROR:		return LOG_ERR;
		case LogLevel::LOGLEVEL_SEVERE:
		case LogLevel::LOGLEVEL_CRITICAL:	return LOG_CRIT;
		case LogLevel::LOGLEVEL_ALERT:		return LOG_ALERT;
		case LogLevel::LOGLEVEL_FATAL:		return LOG_EMERG;
		case LogLevel::LOGLEVEL_UNDEFINED:	return LOG_ERR;
	}
	return LOG_ERR;
}

static int facilityToSyslogFacility( const SyslogFacility::Facility facility )
{
	switch( facility ) {
		case SyslogFacility::WOLFRAME_SYSLOG_FACILITY_KERN:	return LOG_KERN;
		case SyslogFacility::WOLFRAME_SYSLOG_FACILITY_USER:	return LOG_USER;
		case SyslogFacility::WOLFRAME_SYSLOG_FACILITY_MAIL:	return LOG_MAIL;
		case SyslogFacility::WOLFRAME_SYSLOG_FACILITY_DAEMON:	return LOG_DAEMON;
		case SyslogFacility::WOLFRAME_SYSLOG_FACILITY_AUTH:	return LOG_AUTH;
		case SyslogFacility::WOLFRAME_SYSLOG_FACILITY_SYSLOG:	return LOG_SYSLOG;
		case SyslogFacility::WOLFRAME_SYSLOG_FACILITY_LPR:	return LOG_LPR;
		case SyslogFacility::WOLFRAME_SYSLOG_FACILITY_NEWS:	return LOG_NEWS;
		case SyslogFacility::WOLFRAME_SYSLOG_FACILITY_UUCP:	return LOG_UUCP;
		case SyslogFacility::WOLFRAME_SYSLOG_FACILITY_CRON:	return LOG_CRON;
#if defined LOG_AUTHPRIV
		case SyslogFacility::WOLFRAME_SYSLOG_FACILITY_AUTHPRIV:	return LOG_AUTHPRIV;
#else
		case SyslogFacility::WOLFRAME_SYSLOG_FACILITY_AUTHPRIV:	return LOG_AUTH;
#endif
#if defined LOG_FTP
		case SyslogFacility::WOLFRAME_SYSLOG_FACILITY_FTP:	return LOG_FTP;
#else
		case SyslogFacility::WOLFRAME_SYSLOG_FACILITY_FTP:	return LOG_DAEMON;
#endif
#if defined LOG_NTP
		case SyslogFacility::WOLFRAME_SYSLOG_FACILITY_NTP:	return LOG_NTP;
#else
		case SyslogFacility::WOLFRAME_SYSLOG_FACILITY_NTP:	return LOG_DAEMON;
#endif
#if defined LOG_SECURITY
		case SyslogFacility::WOLFRAME_SYSLOG_FACILITY_SECURITY:	return LOG_SECURITY;
#else
		case SyslogFacility::WOLFRAME_SYSLOG_FACILITY_SECURITY:	return LOG_AUTH;
#endif
#if defined LOG_CONSOLE
		case SyslogFacility::WOLFRAME_SYSLOG_FACILITY_CONSOLE:	return LOG_CONSOLE;
#else
		case SyslogFacility::WOLFRAME_SYSLOG_FACILITY_CONSOLE:	return LOG_DAEMON;
#endif
#if defined LOG_AUDIT
		case SyslogFacility::WOLFRAME_SYSLOG_FACILITY_AUDIT:	return LOG_AUDIT;
#else
		case SyslogFacility::WOLFRAME_SYSLOG_FACILITY_AUDIT:	return LOG_AUTH;
#endif
		case SyslogFacility::WOLFRAME_SYSLOG_FACILITY_LOCAL0:	return LOG_LOCAL0;
		case SyslogFacility::WOLFRAME_SYSLOG_FACILITY_LOCAL1:	return LOG_LOCAL1;
		case SyslogFacility::WOLFRAME_SYSLOG_FACILITY_LOCAL2:	return LOG_LOCAL2;
		case SyslogFacility::WOLFRAME_SYSLOG_FACILITY_LOCAL3:	return LOG_LOCAL3;
		case SyslogFacility::WOLFRAME_SYSLOG_FACILITY_LOCAL4:	return LOG_LOCAL4;
		case SyslogFacility::WOLFRAME_SYSLOG_FACILITY_LOCAL5:	return LOG_LOCAL5;
		case SyslogFacility::WOLFRAME_SYSLOG_FACILITY_LOCAL6:	return LOG_LOCAL6;
		case SyslogFacility::WOLFRAME_SYSLOG_FACILITY_LOCAL7:	return LOG_LOCAL7;
		case SyslogFacility::WOLFRAME_SYSLOG_FACILITY_UNDEFINED:	return LOG_DAEMON;
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

inline void SyslogBackend::log( const LogComponent component, const LogLevel::Level level, const std::string& msg )
{
	if ( level >= logLevel_ ) {
		std::ostringstream os;
		os	<< component.str( )
			<< ( component == LogComponent::LogNone ? "" : " - " )
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
	logLevel_ = LogLevel::LOGLEVEL_UNDEFINED;
	// the one category we have at the moment in the resource
	// was '1 | 0x0FFF0000' before, why?!
	log_ = "Application";
	source_ = "<undefined>";
	eventSource_ = RegisterEventSource( NULL, source_.c_str( ) );
	sid_ = NULL;
}

EventlogBackend::~EventlogBackend( )
{
	if( sid_ != NULL ) {
		free( sid_ );
		sid_ = NULL;
	}

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

static WORD levelToEventlogLevel( const LogLevel::Level level )
{
	switch( level )	{
		case LogLevel::LOGLEVEL_DATA:
		case LogLevel::LOGLEVEL_TRACE:
		case LogLevel::LOGLEVEL_DEBUG:
		case LogLevel::LOGLEVEL_INFO:
		case LogLevel::LOGLEVEL_NOTICE:
			return EVENTLOG_INFORMATION_TYPE;
		case LogLevel::LOGLEVEL_WARNING:
			return EVENTLOG_WARNING_TYPE;
		case LogLevel::LOGLEVEL_ERROR:
		case LogLevel::LOGLEVEL_SEVERE:
		case LogLevel::LOGLEVEL_CRITICAL:
		case LogLevel::LOGLEVEL_ALERT:
		case LogLevel::LOGLEVEL_FATAL:
		case LogLevel::LOGLEVEL_UNDEFINED:
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
			eventId = WOLFRAME_EVENTID_ERROR; mask = 3; break;
		case EVENTLOG_WARNING_TYPE:
			eventId = WOLFRAME_EVENTID_WARNING; mask = 2; break;
		case EVENTLOG_INFORMATION_TYPE:
			eventId = WOLFRAME_EVENTID_INFO; mask = 1; break;
		default:
			eventId = WOLFRAME_EVENTID_ERROR; mask = 3;
	}
	return( eventId | 0x0FFF0000L | ( mask << 30 ) );
}

static WORD logComponentToCategoryId( const LogComponent c )
{
	switch( c.component( ) ) {
		case LogComponent::LOGCOMPONENT_NONE:		return WOLFRAME_CATEGORY_NONE;
		case LogComponent::LOGCOMPONENT_LOGGING:	return WOLFRAME_CATEGORY_LOGGING;
		case LogComponent::LOGCOMPONENT_NETWORK:	return WOLFRAME_CATEGORY_NETWORK;
		case LogComponent::LOGCOMPONENT_AUTH:		return WOLFRAME_CATEGORY_AUTH;
		case LogComponent::LOGCOMPONENT_LUA:		return WOLFRAME_CATEGORY_LUA;
		default:									return WOLFRAME_CATEGORY_NONE;
	};
}

inline void EventlogBackend::log( WOLFRAME_UNUSED const LogComponent component, const LogLevel::Level level, const std::string& msg )
{
	if ( level >= logLevel_ ) {
		LPCSTR msg_arr[1];
		msg_arr[0] = (LPSTR)msg.c_str( );
		BOOL res = ReportEvent(
			eventSource_,
			levelToEventlogLevel( level ),
			logComponentToCategoryId( component ),
			messageIdToEventlogId( level ),
			sid_, // SID of the user owning the process, not now, later..
			1, // at the moment no strings to replace, just the message itself
			0, // no binary data
			msg_arr, // array of strings to log (msg.c_str() for now)
			NULL ); // no binary data
		if( !res ) {
			_LOG_CRITICAL	<< _Wolframe::Logging::LogComponent::LogLogging
					<< "Can't report event to event log: "
					<< Logger::LogWinerror;
		}
	}
}

void EventlogBackend::reopen( )

{
// deferring calculation of SID for logging, avoid reentrance trouble in
// the constructor
	if( !sid_ )
		calculateSid( );

	if( eventSource_ )
		(void)DeregisterEventSource( eventSource_ );
	eventSource_ = RegisterEventSource( NULL, source_.c_str( ) );
}

void EventlogBackend::calculateSid( )
{
// get the token for the current process token 'tokenProcess' with proper rights,
// process is only a pseudoToken (which we also don't have to close)
	HANDLE tokenProcess = NULL;
	HANDLE process = GetCurrentProcess( );
	if( process == NULL ) {
		_LOG_CRITICAL	<< _Wolframe::Logging::LogComponent::LogLogging
				<< "Unable to get current process handle (GetCurrentProcess): "
				<< Logger::LogWinerror;
		return;
	}
	if( !OpenProcessToken( process, TOKEN_QUERY, &tokenProcess ) ) {
		_LOG_CRITICAL	<< _Wolframe::Logging::LogComponent::LogLogging
				<< "Unable to get process token of current process (OpenProcessToken): "
				<< Logger::LogWinerror;
		return;
	}

// get the required size to store the user token and allocate memory for it */
	DWORD tokenUserSize = 0;
	(void)GetTokenInformation( tokenProcess, TokenUser, NULL, 0, &tokenUserSize );
	PTOKEN_USER tokenUser = (PTOKEN_USER)malloc( tokenUserSize * sizeof( BYTE ) );
	if( tokenUser == NULL ) {
		_LOG_CRITICAL	<< _Wolframe::Logging::LogComponent::LogLogging
				<< "Unable to get memory to store user token (malloc)";
		CloseHandle( tokenProcess );
		return;
	}

// get the user token
	if( !GetTokenInformation( tokenProcess, TokenUser, (LPVOID)tokenUser, tokenUserSize, &tokenUserSize ) ) {
		_LOG_CRITICAL	<< _Wolframe::Logging::LogComponent::LogLogging
				<< "Unable to get user token (GetTokenInformation): "
				<< Logger::LogWinerror;
		free( tokenUser );
		CloseHandle( tokenProcess );
		return;
	}

// make a local copy of the sid because it and return that one
	PSID tokenSid = (PSID)tokenUser->User.Sid;
	DWORD sidSize = GetLengthSid( tokenSid );
	sid_ = (PSID)malloc( sidSize );
	if( sid_ != NULL ) {
		if( !CopySid( sidSize, sid_, tokenSid ) ) {
			_LOG_CRITICAL	<< _Wolframe::Logging::LogComponent::LogLogging
					<< "Unable to make a copy of the current SID (CopySid): "
					<< Logger::LogWinerror;
			free( tokenSid );
			free( tokenUser );
			CloseHandle( tokenProcess );
			return;
		}
	} else {
		_LOG_CRITICAL	<< _Wolframe::Logging::LogComponent::LogLogging
				<< "Unable to get memory to store copy of user token (malloc)";
		free( tokenSid );
		free( tokenUser );
		CloseHandle( tokenProcess );
		return;
	}

// the process handle is a pseudo handle which doesn't need closing,
// all others have to be freed
	free( tokenSid );
	free( tokenUser );
	CloseHandle( tokenProcess );
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

inline void LogBackend::LogBackendImpl::log( const LogComponent component, const LogLevel::Level level, const std::string& msg )
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

void LogBackend::log( const LogComponent component, const LogLevel::Level level, const std::string& msg )	{ impl_->log( component, level, msg ); }


// Logger

Logger::Logger( LogBackend& backend ) :	logBk_( backend )
{
}

Logger::~Logger( )
{
	logBk_.log( component_, msgLevel_, os_.str( ) );
}

Logger& Logger::Get( LogLevel::Level level )
{
	component_ = LogComponent::LogNone;
	msgLevel_ = level;
	return *this;
}

	} // namespace Logging
} // namespace _Wolframe
