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
/// \file logger.hpp
/// \brief Top-level include file for logging
///

#ifndef _LOGGER_HPP_INCLUDED
#define _LOGGER_HPP_INCLUDED

#include "logger/logLevel.hpp"
#include "logger/logSyslogFacility.hpp"
#include "logger/logError.hpp"
#include "logger/logComponent.hpp"
#include "logger/logBackend.hpp"

#include <sstream>

namespace _Wolframe {
namespace log {

class Logger {
public:
	/// create a logger and connect it to a backend, typically
	/// not called directly
	Logger( LogBackend& backend );
	Logger( LogBackend* backend );

	~Logger( );

	Logger& Get( LogLevel::Level level );

	template<typename T> friend Logger& operator<<( Logger& logger, T thing );
	friend Logger& operator<<( Logger& logger, LogComponent c );
	friend Logger& operator<<( Logger& logger, LogError e );

protected:
	std::ostringstream os_;

private:
	LogBackend&	logBk_;
	LogLevel::Level	msgLevel_;
	LogComponent	component_;

	Logger( );
	Logger( const Logger& );
	Logger& operator= ( const Logger& );
};

/// template functions for logging, default is we search for the << operator
/// and log with this one..
template<typename T>
Logger& operator<<( Logger& logger, T t )
{
	logger.os_ << t;
	return logger;
}

}} // namespace _Wolframe::log

// shortcut macros
#define LOG_DATA	_Wolframe::log::Logger( _Wolframe::log::LogBackend::instance() ).Get( _Wolframe::log::LogLevel::LOGLEVEL_DATA )
#define LOG_TRACE	_Wolframe::log::Logger( _Wolframe::log::LogBackend::instance() ).Get( _Wolframe::log::LogLevel::LOGLEVEL_TRACE )
#define LOG_DEBUG	_Wolframe::log::Logger( _Wolframe::log::LogBackend::instance() ).Get( _Wolframe::log::LogLevel::LOGLEVEL_DEBUG )
#define LOG_INFO	_Wolframe::log::Logger( _Wolframe::log::LogBackend::instance() ).Get( _Wolframe::log::LogLevel::LOGLEVEL_INFO )
#define LOG_NOTICE	_Wolframe::log::Logger( _Wolframe::log::LogBackend::instance() ).Get( _Wolframe::log::LogLevel::LOGLEVEL_NOTICE )
#define LOG_WARNING	_Wolframe::log::Logger( _Wolframe::log::LogBackend::instance() ).Get( _Wolframe::log::LogLevel::LOGLEVEL_WARNING )
#define LOG_ERROR	_Wolframe::log::Logger( _Wolframe::log::LogBackend::instance() ).Get( _Wolframe::log::LogLevel::LOGLEVEL_ERROR )
#define LOG_SEVERE	_Wolframe::log::Logger( _Wolframe::log::LogBackend::instance() ).Get( _Wolframe::log::LogLevel::LOGLEVEL_SEVERE )
#define LOG_CRITICAL	_Wolframe::log::Logger( _Wolframe::log::LogBackend::instance() ).Get( _Wolframe::log::LogLevel::LOGLEVEL_CRITICAL )
#define LOG_ALERT	_Wolframe::log::Logger( _Wolframe::log::LogBackend::instance() ).Get( _Wolframe::log::LogLevel::LOGLEVEL_ALERT )
#define LOG_FATAL	_Wolframe::log::Logger( _Wolframe::log::LogBackend::instance() ).Get( _Wolframe::log::LogLevel::LOGLEVEL_FATAL )

// temporary modules hack
extern _Wolframe::log::LogBackend*	logBackendPtr;
#define MOD_LOG_DATA	_Wolframe::log::Logger( logBackendPtr ).Get( _Wolframe::log::LogLevel::LOGLEVEL_DATA )
#define MOD_LOG_TRACE	_Wolframe::log::Logger( logBackendPtr ).Get( _Wolframe::log::LogLevel::LOGLEVEL_TRACE )
#define MOD_LOG_DEBUG	_Wolframe::log::Logger( logBackendPtr ).Get( _Wolframe::log::LogLevel::LOGLEVEL_DEBUG )
#define MOD_LOG_INFO	_Wolframe::log::Logger( logBackendPtr ).Get( _Wolframe::log::LogLevel::LOGLEVEL_INFO )
#define MOD_LOG_NOTICE	_Wolframe::log::Logger( logBackendPtr ).Get( _Wolframe::log::LogLevel::LOGLEVEL_NOTICE )
#define MOD_LOG_WARNING	_Wolframe::log::Logger( logBackendPtr ).Get( _Wolframe::log::LogLevel::LOGLEVEL_WARNING )
#define MOD_LOG_ERROR	_Wolframe::log::Logger( logBackendPtr ).Get( _Wolframe::log::LogLevel::LOGLEVEL_ERROR )
#define MOD_LOG_SEVERE	_Wolframe::log::Logger( logBackendPtr ).Get( _Wolframe::log::LogLevel::LOGLEVEL_SEVERE )
#define MOD_LOG_CRITICAL _Wolframe::log::Logger( logBackendPtr ).Get( _Wolframe::log::LogLevel::LOGLEVEL_CRITICAL )
#define MOD_LOG_ALERT	_Wolframe::log::Logger( logBackendPtr ).Get( _Wolframe::log::LogLevel::LOGLEVEL_ALERT )
#define MOD_LOG_FATAL	_Wolframe::log::Logger( logBackendPtr ).Get( _Wolframe::log::LogLevel::LOGLEVEL_FATAL )

#endif // _LOGGER_HPP_INCLUDED
