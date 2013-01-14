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
/// \file logBackendConsole.cpp
/// \brief implementation of the console backend
///

#include "logger/logLevel.hpp"
#include "logBackendConsole.hpp"

#include <iostream>

namespace _Wolframe {
namespace log {

ConsoleLogBackend::ConsoleLogBackend( )
{
	m_logLevel = LogLevel::LOGLEVEL_ERROR;
}

ConsoleLogBackend::~ConsoleLogBackend( )
{
	// nothing to do here
}

void ConsoleLogBackend::setLevel( const LogLevel::Level level_ )
{
	m_logLevel = level_;
}

void ConsoleLogBackend::log( const LogLevel::Level level_, const std::string& msg )
{
	if ( level_ >= m_logLevel )	{
		boost::mutex::scoped_lock lock( m_mutex );
		std::cerr << level_ << ": " << msg << std::endl;
		std::cerr.flush( );
	}
}

void ConsoleLogBackend::reopen( )
{
	// nothing to do here
}

}} // namespace _Wolframe::log
