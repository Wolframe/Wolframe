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
/// \file logBackend.cpp
/// \brief implementation of the logger which logs to a simple logfile
///

#include "logger-v1.hpp"
#include "logBackendFile.hpp"

#if defined( _WIN32 )
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif // defined( _WIN32 )

namespace _Wolframe {
namespace log {

LogfileBackend::LogfileBackend( )
{
	m_logLevel = LogLevel::LOGLEVEL_UNDEFINED;
	m_isOpen = false;
	// we can't open the logfile here, wait for setFilename
}

LogfileBackend::~LogfileBackend( )
{
	if( m_isOpen ) {
		m_logFile.close( );
	}
}

void LogfileBackend::setLevel( const LogLevel::Level level_ )
{
	m_logLevel = level_;
}

void LogfileBackend::setFilename( const std::string filename )
{
	m_filename = filename;
	reopen( );
}

void LogfileBackend::reopen( )
{
	if( m_isOpen ) {
		m_logFile.close( );
		m_isOpen = false;
	}

	m_logFile.exceptions( m_logFile.badbit | m_logFile.failbit );

	try {
		m_logFile.open( m_filename.c_str( ), std::ios_base::app );
		m_isOpen = true;
	} catch( const std::ofstream::failure& ) {
		m_isOpen = false;
		LOG_CRITICAL	<< "Can't open logfile '" << m_filename << "'";
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

void LogfileBackend::log( const LogLevel::Level _level, const std::string& msg )
{
	if( _level >= m_logLevel && m_isOpen ) {
		m_logFile	<< timestamp( ) << " "
				<< _level << ": " << msg << std::endl;
		m_logFile.flush( );
	}
}

}} // namespace _Wolframe::log
