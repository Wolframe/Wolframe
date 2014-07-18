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
/// \file logger.hpp
/// \brief Top-level include file for logging

#ifndef _LOGGER_HPP_INCLUDED
#define _LOGGER_HPP_INCLUDED

#include "logger/logLevel.hpp"
#include "logger/logSyslogFacility.hpp"
#include "logger/logError.hpp"
#include "logger/logBackend.hpp"

#include <sstream>

namespace _Wolframe {
namespace log {

/// Logger abstraction configurable to print to multiple backends
class Logger {
public:
	/// create a logger and connect it to a backend, typically
	/// not called directly
	Logger( LogBackend& backend ) : m_logBk( backend )	{}
	Logger( LogBackend* backend ) :	m_logBk( *backend )	{}

	~Logger( )	{
		m_logBk.log( m_msgLevel, m_os.str( ) );
	}

	inline Logger& Get( LogLevel::Level level )	{
		m_msgLevel = level;
		return *this;
	}

	template<typename T> friend Logger& operator<< ( Logger& logger, T thing );
	friend Logger& operator<< ( Logger& logger, LogError e );

protected:
	std::ostringstream m_os;

private:
	LogBackend&	m_logBk;
	LogLevel::Level	m_msgLevel;

	Logger( );
	Logger( const Logger& );
	Logger& operator= ( const Logger& );
};


/// \brief template functions for logging, default is we search for the << operator
/// and log with this one..
template <typename T>
Logger& operator<< ( Logger& logger, T t )
{
	logger.m_os << t;
	return logger;
}

}} // namespace _Wolframe::log


// shortcut macros
#define LOG_DATA2	if ( _Wolframe::log::LogBackend::instance().minLogLevel() > _Wolframe::log::LogLevel::LOGLEVEL_DATA2 ) ; \
			else _Wolframe::log::Logger( _Wolframe::log::LogBackend::instance() ).Get( _Wolframe::log::LogLevel::LOGLEVEL_DATA2 )
#define LOG_DATA	if ( _Wolframe::log::LogBackend::instance().minLogLevel() > _Wolframe::log::LogLevel::LOGLEVEL_DATA ) ; \
			else _Wolframe::log::Logger( _Wolframe::log::LogBackend::instance() ).Get( _Wolframe::log::LogLevel::LOGLEVEL_DATA )
#define LOG_TRACE	if ( _Wolframe::log::LogBackend::instance().minLogLevel() > _Wolframe::log::LogLevel::LOGLEVEL_TRACE ) ; \
			else _Wolframe::log::Logger( _Wolframe::log::LogBackend::instance() ).Get( _Wolframe::log::LogLevel::LOGLEVEL_TRACE )
#define LOG_DEBUG	if ( _Wolframe::log::LogBackend::instance().minLogLevel() > _Wolframe::log::LogLevel::LOGLEVEL_DEBUG ) ; \
			else _Wolframe::log::Logger( _Wolframe::log::LogBackend::instance() ).Get( _Wolframe::log::LogLevel::LOGLEVEL_DEBUG )
#define LOG_INFO	if ( _Wolframe::log::LogBackend::instance().minLogLevel() > _Wolframe::log::LogLevel::LOGLEVEL_INFO ) ; \
			else _Wolframe::log::Logger( _Wolframe::log::LogBackend::instance() ).Get( _Wolframe::log::LogLevel::LOGLEVEL_INFO )
#define LOG_NOTICE	if ( _Wolframe::log::LogBackend::instance().minLogLevel() > _Wolframe::log::LogLevel::LOGLEVEL_NOTICE ) ; \
			else _Wolframe::log::Logger( _Wolframe::log::LogBackend::instance() ).Get( _Wolframe::log::LogLevel::LOGLEVEL_NOTICE )
#define LOG_WARNING	if ( _Wolframe::log::LogBackend::instance().minLogLevel() > _Wolframe::log::LogLevel::LOGLEVEL_WARNING ) ; \
			else _Wolframe::log::Logger( _Wolframe::log::LogBackend::instance() ).Get( _Wolframe::log::LogLevel::LOGLEVEL_WARNING )
#define LOG_ERROR	if ( _Wolframe::log::LogBackend::instance().minLogLevel() > _Wolframe::log::LogLevel::LOGLEVEL_ERROR ) ; \
			else _Wolframe::log::Logger( _Wolframe::log::LogBackend::instance() ).Get( _Wolframe::log::LogLevel::LOGLEVEL_ERROR )
#define LOG_SEVERE	if ( _Wolframe::log::LogBackend::instance().minLogLevel() > _Wolframe::log::LogLevel::LOGLEVEL_SEVERE ) ; \
			else _Wolframe::log::Logger( _Wolframe::log::LogBackend::instance() ).Get( _Wolframe::log::LogLevel::LOGLEVEL_SEVERE )
#define LOG_CRITICAL	if ( _Wolframe::log::LogBackend::instance().minLogLevel() > _Wolframe::log::LogLevel::LOGLEVEL_CRITICAL ) ; \
			else _Wolframe::log::Logger( _Wolframe::log::LogBackend::instance() ).Get( _Wolframe::log::LogLevel::LOGLEVEL_CRITICAL )
#define LOG_ALERT	if ( _Wolframe::log::LogBackend::instance().minLogLevel() > _Wolframe::log::LogLevel::LOGLEVEL_ALERT ) ; \
			else _Wolframe::log::Logger( _Wolframe::log::LogBackend::instance() ).Get( _Wolframe::log::LogLevel::LOGLEVEL_ALERT )
#define LOG_FATAL	if ( _Wolframe::log::LogBackend::instance().minLogLevel() > _Wolframe::log::LogLevel::LOGLEVEL_FATAL ) ; \
			else _Wolframe::log::Logger( _Wolframe::log::LogBackend::instance() ).Get( _Wolframe::log::LogLevel::LOGLEVEL_FATAL )

#endif // _LOGGER_HPP_INCLUDED
