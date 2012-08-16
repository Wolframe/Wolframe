/************************************************************************

 Copyright (C) 2011, 2012 Project Wolframe.
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
/// \file logBackendWinDebug.cpp
/// \brief implementation of the Windows debugging log backend
///

// TODO: why!!!? logLevel.hpp with << definition should be enough
#include "logger-v1.hpp"
#include "logger/logLevel.hpp"
#include "logBackendWinDebug.hpp"

#include <sstream>

#define WIN32_MEAN_AND_LEAN
#include <windows.h>

namespace _Wolframe {
	namespace log {

WinDebugLogBackend::WinDebugLogBackend( )
{
	// default is not to log to the Windows debug log, this should be
	// set manually, if needed (in debug code mainly)
	logLevel_ = LogLevel::LOGLEVEL_UNDEFINED;
}

WinDebugLogBackend::~WinDebugLogBackend( )
{
	// nothing to do here
}

void WinDebugLogBackend::setLevel( const LogLevel::Level level )
{
	logLevel_ = level;
}

void WinDebugLogBackend::log( const LogComponent /* component */, const LogLevel::Level level, const std::string& msg )
{
	if ( level >= logLevel_ ) {
		std::ostringstream oss;
		oss << level << ": " << msg << "\r";
		OutputDebugString( oss.str( ).c_str( ) );
	}
}

void WinDebugLogBackend::reopen( )
{
	// nothing to do here
}

	} // namespace log
} // namespace _Wolframe
