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
/// \file logBackendEventlog.hpp
/// \brief header file for logging to the Windows event logger
///

#ifndef _LOG_BACKEND_EVENTLOG_HPP_INCLUDED
#define _LOG_BACKEND_EVENTLOG_HPP_INCLUDED

#include "logger/logLevel.hpp"

#define WIN32_MEAN_AND_LEAN
#define NOMINMAX
#include <windows.h>

namespace _Wolframe {
namespace log {

class EventlogBackend
{
public:
	EventlogBackend( );

	~EventlogBackend( );

	void setLevel( const LogLevel::Level level_ );
	LogLevel::Level level() const		{ return logLevel_; }

	void setLog( const std::string& log );

	void setSource( const std::string& source );

	void log( const LogLevel::Level level_, const std::string& msg );

	void reopen( );

private:
	LogLevel::Level logLevel_;
	HANDLE eventSource_;
	std::string log_;
	std::string source_;
	PSID sid_;

	void calculateSid( );
};

}} // namespace _Wolframe::log

#endif // _LOG_BACKEND_EVENTLOG_HPP_INCLUDED
