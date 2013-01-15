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
/// \file logBackendConsole.hpp
/// \brief header file for the console backend
///

#ifndef _LOG_BACKEND_CONSOLE_HPP_INCLUDED
#define _LOG_BACKEND_CONSOLE_HPP_INCLUDED

#include <boost/thread/mutex.hpp>

#include "logger/logLevel.hpp"

namespace _Wolframe {
namespace log {

// *MBa*
// Note: the mutex solution might be temporary, we shoul do some performance tests here
class ConsoleLogBackend
{
public:
	ConsoleLogBackend( );

	~ConsoleLogBackend( );

	void setLevel( const LogLevel::Level level_ );

	void log( const LogLevel::Level level_, const std::string& msg );

	void reopen( );

	LogLevel::Level level() const	{ return m_logLevel; }
private:
	LogLevel::Level	m_logLevel;
	boost::mutex	m_mutex;
};

}} // namespace _Wolframe::log

#endif // _LOG_BACKEND_CONSOLE_HPP_INCLUDED
