/************************************************************************

 Copyright (C) 2011 - 2014 Project Wolframe.
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
/// \file logBackendEventlog.cpp
/// \brief implementation of logging to the Windows event logger
///

#include <tchar.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <ostream>
#include <iostream>
#include <sstream>

#include "logBackendEventlog.hpp"

#include "wolframemsg.h"

#include "logger-v1.hpp" // TODO: just for LOG_ macros, should not be like this!

namespace _Wolframe {
namespace log {

EventlogBackend::EventlogBackend( )
{
	logLevel_ = LogLevel::LOGLEVEL_UNDEFINED;
	// the one category we have at the moment in the resource
	// was '1 | 0x0FFF0000' before, why?!
	log_ = "Application";
	source_ = "Wolframe";
	eventSource_ = RegisterEventSource( NULL, source_.c_str( ) );
	if( !eventSource_ )
		LOG_WARNING 	<< "Could not register a minimalistic event source to Application/Wolframe: "
				<< LogError::LogWinerror;
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

void EventlogBackend::setLevel( const LogLevel::Level level_ )
{
	logLevel_ = level_;
}

void EventlogBackend::setLog( const std::string& log )
{
	log_ = log;
}

void EventlogBackend::setSource( const std::string& source )
{
	source_ = source;
	reopen( );
}

static WORD levelToEventlogLevel( const LogLevel::Level lvl )
{
	switch( lvl )	{
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

void EventlogBackend::log( const LogLevel::Level level_,
			   const std::string& msg )
{
	if ( level_ >= logLevel_ && eventSource_ ) {
		LPCSTR msg_arr[1];
		msg_arr[0] = (LPSTR)msg.c_str( );
		BOOL res = ReportEvent(
			eventSource_,
			levelToEventlogLevel( level_ ),
			WOLFRAME_CATEGORY_NONE,
			messageIdToEventlogId( level_ ),
			sid_, // SID of the user owning the process, not now, later..
			1, // at the moment no strings to replace, just the message itself
			0, // no binary data
			msg_arr, // array of strings to log (msg.c_str() for now)
			NULL ); // no binary data
		if( !res ) {
			LOG_CRITICAL	<< "Can't report event to event log: "
					<< LogError::LogWinerror;
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
	if( !eventSource_ )
		LOG_WARNING 	<< "Could not register event source '" << source_ << "'"
				<< LogError::LogWinerror;
	else
		LOG_DEBUG	<< "Registered new eventlog source '" << source_ << "' of log '"
				<< log_ << "'";
}

void EventlogBackend::calculateSid( )
{
// get the token for the current process token 'tokenProcess' with proper rights,
// process is only a pseudoToken (which we also don't have to close)
	HANDLE tokenProcess = NULL;
	HANDLE process = GetCurrentProcess( );
	if( process == NULL ) {
		LOG_CRITICAL	<< "Unable to get current process handle (GetCurrentProcess): "
				<< LogError::LogWinerror;
		return;
	}
	if( !OpenProcessToken( process, TOKEN_QUERY, &tokenProcess ) ) {
		LOG_CRITICAL	<< "Unable to get process token of current process (OpenProcessToken): "
				<< LogError::LogWinerror;
		return;
	}

// get the required size to store the user token and allocate memory for it */
	DWORD tokenUserSize = 0;
	(void)GetTokenInformation( tokenProcess, TokenUser, NULL, 0, &tokenUserSize );
	PTOKEN_USER tokenUser = (PTOKEN_USER)malloc( tokenUserSize * sizeof( BYTE ) );
	if( tokenUser == NULL ) {
		LOG_CRITICAL	<< "Unable to get memory to store user token (malloc)";
		CloseHandle( tokenProcess );
		return;
	}

// get the user token
	if( !GetTokenInformation( tokenProcess, TokenUser, (LPVOID)tokenUser, tokenUserSize, &tokenUserSize ) ) {
		LOG_CRITICAL	<< "Unable to get user token (GetTokenInformation): "
				<< LogError::LogWinerror;
		free( tokenUser );
		CloseHandle( tokenProcess );
		return;
	}

// make a local copy of the sid and return that one
	PSID tokenSid = (PSID)tokenUser->User.Sid;
	DWORD sidSize = GetLengthSid( tokenSid );
	sid_ = (PSID)malloc( sidSize );
	if( sid_ != NULL ) {
		if( !CopySid( sidSize, sid_, tokenSid ) ) {
			LOG_CRITICAL	<< "Unable to make a copy of the current SID (CopySid): "
					<< LogError::LogWinerror;
			free( tokenUser );
			CloseHandle( tokenProcess );
			return;
		}
	} else {
		LOG_CRITICAL	<< "Unable to get memory to store copy of user token (malloc)";
		free( tokenUser );
		CloseHandle( tokenProcess );
		return;
	}

// the process handle is a pseudo handle which doesn't need closing,
// all others have to be freed
	free( tokenUser );
	CloseHandle( tokenProcess );
}

}} // namespace _Wolframe::log
