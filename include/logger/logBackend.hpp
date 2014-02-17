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
/// \file logBackend.hpp
/// \brief Interface for the logging backend
///

#ifndef _LOG_BACKEND_HPP_INCLUDED
#define _LOG_BACKEND_HPP_INCLUDED

#include "singleton.hpp"
#include "logger/logLevel.hpp"
#include "logger/logSyslogFacility.hpp"

#include <string>

namespace _Wolframe {
namespace log {

class LogBackend : public Singleton< LogBackend >
{
public:
	LogBackend( );

	~LogBackend( );

	void setConsoleLevel( const LogLevel::Level level );

	void setLogfileLevel( const LogLevel::Level level );

	void setLogfileName( const std::string filename );

	void setSyslogLevel( const LogLevel::Level level );

	void setSyslogFacility( const SyslogFacility::Facility facility );

	void setSyslogIdent( const std::string ident );

#if defined( _WIN32 )
	void setEventlogLevel( const LogLevel::Level level );

	void setEventlogLog( const std::string log );

	void setEventlogSource( const std::string source );

	void setWinDebugLevel( const LogLevel::Level level );
#endif // defined( _WIN32 )

	void log( const LogLevel::Level level, const std::string& msg );

	LogLevel::Level minLogLevel() const;
private:
	class LogBackendImpl;
	LogBackendImpl	*impl_;
};

}} // namespace _Wolframe::log

#endif // _LOG_BACKEND_HPP_INCLUDED
