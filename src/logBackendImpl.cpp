/************************************************************************

 Copyright (C) 2011 Project Wolframe.
 All rights reserved.

 This file is part of Project Wolframe.

 Commercial Usage
    Licensees holding valid Project Wolframe Commercial licenses may
    use this file in accordance with the Project Wolframe
    Commercial License Agreement provided with the Software or,
    alternatively, in accordance with the terms contained
    in a written agreement between the licensee and Project Wolframe.

 GNU General Public License Usage
    Alternatively, you can redistribute this file and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Wolframe is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/

///
/// \file logBackend.cpp
/// \brief implementation of logging backends
///

#include <ostream>
#include <iostream>
#include <sstream>

#include "logger.hpp"
#include "logBackendImpl.hpp"
#include "logBackendConsole.hpp"

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

inline void EventlogBackend::log( const LogComponent component, const LogLevel::Level level, const std::string& msg )
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

	} // namespace Logging
} // namespace _Wolframe
