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
/// \file logLevel.cpp
/// \brief Implementation of the log level
///

#include "logger/logLevel.hpp"

namespace _Wolframe {
namespace log {

LogLevel::Level LogLevel::strToLogLevel( const std::string& s )
{
	if( s == "DATA" )		return LogLevel::LOGLEVEL_DATA;
	else if( s == "TRACE" )		return LogLevel::LOGLEVEL_TRACE;
	else if( s == "DEBUG" )		return LogLevel::LOGLEVEL_DEBUG;
	else if( s == "INFO" )		return LogLevel::LOGLEVEL_INFO;
	else if( s == "NOTICE" )	return LogLevel::LOGLEVEL_NOTICE;
	else if( s == "WARNING" )	return LogLevel::LOGLEVEL_WARNING;
	else if( s == "ERROR" )		return LogLevel::LOGLEVEL_ERROR;
	else if( s == "SEVERE" )	return LogLevel::LOGLEVEL_SEVERE;
	else if( s == "CRITICAL" )	return LogLevel::LOGLEVEL_CRITICAL;
	else if( s == "ALERT" )		return LogLevel::LOGLEVEL_ALERT;
	else if( s == "FATAL" )		return LogLevel::LOGLEVEL_FATAL;
	else				return LogLevel::LOGLEVEL_UNDEFINED;
}

}} // namespace _Wolframe::log
