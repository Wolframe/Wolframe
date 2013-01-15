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
/// \file logBackendWinDebug.cpp
/// \brief implementation of the Windows debugging log backend
///

#define WIN32_MEAN_AND_LEAN
#include <windows.h>
#include <sstream>

#include "logger/logLevel.hpp"
#include "logBackendWinDebug.hpp"

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

void WinDebugLogBackend::setLevel( const LogLevel::Level level_ )
{
	logLevel_ = level_;
}

void WinDebugLogBackend::log( const LogLevel::Level level_, const std::string& msg )
{
	if ( level_ >= logLevel_ ) {
		std::ostringstream oss;
		oss << level_ << ": " << msg << "\r";
		OutputDebugString( oss.str( ).c_str( ) );
	}
}

void WinDebugLogBackend::reopen( )
{
	// nothing to do here
}

}} // namespace _Wolframe::log
