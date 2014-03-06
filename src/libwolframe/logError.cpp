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
/// \file logError.cpp
/// \brief implementation of logging error
///

// for strerror, errno, FormatMessage, GetLastError
#ifdef _WIN32
#include <tchar.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <strsafe.h>
#pragma warning(disable : 4995)
#else
#include <errno.h>
#include <cstring>
#endif

#include "logger-v1.hpp"
#include "logger/logError.hpp"

#include <stdexcept>
#include <sstream>

namespace _Wolframe {
namespace log {

const LogError LogError::LogStrerror( LogError::LOGERROR_STRERROR );
const LogError LogError::LogWinerror( LogError::LOGERROR_WINERROR );

// template functions for error markers in the output stream
// e.g. LOG_ERROR << "f() had a booboo, reason: " << Logger::LogStrerror
Logger& operator<< ( Logger& logger, LogError e )
{
	switch( e.error( ) ) {
#if !defined( _WIN32 )
		case LogError::LOGERROR_STRERROR: {
			char errbuf[512];
#if defined( __USE_GNU )
			char *ss = strerror_r( errno, errbuf, 512 );
			logger.m_os << ss;
#else
			int res = strerror_r( errno, errbuf, 512 );
			if( res == EINVAL ) {
				logger.os_ << "(missing strerror translation)";
			} else if( res == ERANGE ) {
				logger.m_os << "(memory allocation error in strerror translation)";
			} else {
				logger.m_os << errbuf;
			}
#endif // defined( __USE_GNU )
			logger.m_os << " (errno: " << errno << ")";

			return logger;
		}
#endif // !defined( _WIN32 )

#if defined( _WIN32 )
		case LogError::LOGERROR_WINERROR: {
			DWORD last_error = GetLastError( );
			TCHAR errbuf[512];
			LPVOID werrbuf;
			//DWORD wbuf_size;
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

			logger.m_os << errbuf;

			logger.m_os << " (errcode: " << last_error << ")";

			return logger;
		}

#endif // defined( _WIN32 )

#if !defined( _WIN32 )
		case LogError::LOGERROR_WINERROR:
#endif // defined( _WIN32 )
#if defined( _WIN32 )
		case LogError::LOGERROR_STRERROR:
#endif // defined( _WIN32 )
		case LogError::LOGERROR_UNDEF:
		default: {
			std::ostringstream ss;
			ss << "Illegal error type " << e.error( ) << " in error log marker";
			throw new std::runtime_error( ss.str( ) );
		}
	}
}

}} // namespace _Wolframe::log
