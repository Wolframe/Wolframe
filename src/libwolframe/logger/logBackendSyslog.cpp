/************************************************************************

 Copyright (C) 2011 - 2013 Project Wolframe.
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
/// \file logBackendSyslog.cpp
/// \brief implementation of logging to syslog
///

#include "logBackendSyslog.hpp"

#include <sstream>

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
#define _LOG_DATA	_Wolframe::log::Logger( _Wolframe::log::LogBackend::instance() ).Get( _Wolframe::log::LogLevel::LOGLEVEL_DATA )
#define _LOG_TRACE	_Wolframe::log::Logger( _Wolframe::log::LogBackend::instance() ).Get( _Wolframe::log::LogLevel::LOGLEVEL_TRACE )
#define _LOG_DEBUG	_Wolframe::log::Logger( _Wolframe::log::LogBackend::instance() ).Get( _Wolframe::log::LogLevel::LOGLEVEL_DEBUG )
#define _LOG_INFO	_Wolframe::log::Logger( _Wolframe::log::LogBackend::instance() ).Get( _Wolframe::log::LogLevel::LOGLEVEL_INFO )
#define _LOG_NOTICE	_Wolframe::log::Logger( _Wolframe::log::LogBackend::instance() ).Get( _Wolframe::log::LogLevel::LOGLEVEL_NOTICE )
#define _LOG_WARNING	_Wolframe::log::Logger( _Wolframe::log::LogBackend::instance() ).Get( _Wolframe::log::LogLevel::LOGLEVEL_WARNING )
#define _LOG_ERROR	_Wolframe::log::Logger( _Wolframe::log::LogBackend::instance() ).Get( _Wolframe::log::LogLevel::LOGLEVEL_ERROR )
#define _LOG_SEVERE	_Wolframe::log::Logger( _Wolframe::log::LogBackend::instance() ).Get( _Wolframe::log::LogLevel::LOGLEVEL_SEVERE )
#define _LOG_CRITICAL	_Wolframe::log::Logger( _Wolframe::log::LogBackend::instance() ).Get( _Wolframe::log::LogLevel::LOGLEVEL_CRITICAL )
#define _LOG_ALERT	_Wolframe::log::Logger( _Wolframe::log::LogBackend::instance() ).Get( _Wolframe::log::LogLevel::LOGLEVEL_ALERT )
#define _LOG_FATAL	_Wolframe::log::Logger( _Wolframe::log::LogBackend::instance() ).Get( _Wolframe::log::LogLevel::LOGLEVEL_FATAL )

#include <cstring>

#include <errno.h>
#if !defined( _WIN32 )
#include <syslog.h>
#else
#include "syslog_win32.h"
#endif

namespace _Wolframe {
namespace log {

SyslogBackend::SyslogBackend( )
{
	logLevel_ = LogLevel::LOGLEVEL_UNDEFINED;
	ident_ = "<undefined>";
	facility_ = SyslogFacility::WOLFRAME_SYSLOG_FACILITY_DAEMON;
	openlog( ident_.c_str( ), LOG_PID, facility_ );
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

void SyslogBackend::setLevel( const LogLevel::Level level_ )
{
	logLevel_ = level_;
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

void SyslogBackend::log( const LogLevel::Level level_, const std::string& msg )
{
	if ( level_ >= logLevel_ ) {
		std::ostringstream os;
		os << msg;
		syslog( levelToSyslogLevel( level_ ), "%s", os.str( ).c_str( ) );
	}
}

void SyslogBackend::reopen( )
{
	closelog( );
	openlog( ident_.c_str( ), LOG_PID, facility_ );
}

}} // namespace _Wolframe::log
