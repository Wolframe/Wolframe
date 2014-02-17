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
/// \file logBackendImpl.cpp
/// \brief implementation of logging backends
///

#include "logBackendImpl.hpp"

#include <algorithm>

namespace _Wolframe {
namespace log {

LogBackend::LogBackendImpl::LogBackendImpl( )
{
	// logger are members, implicit initialization
}

LogBackend::LogBackendImpl::~LogBackendImpl( )
{
	// logger resources freed in destructors of members
}

void LogBackend::LogBackendImpl::setConsoleLevel( const LogLevel::Level level )
{
	m_consoleLogger.setLevel( level );
}

void LogBackend::LogBackendImpl::setLogfileLevel( const LogLevel::Level level )
{
	m_logfileLogger.setLevel( level );
}

void LogBackend::LogBackendImpl::setLogfileName( const std::string filename )
{
	m_logfileLogger.setFilename( filename );
}

void LogBackend::LogBackendImpl::setSyslogLevel( const LogLevel::Level level )
{
	m_syslogLogger.setLevel( level );
}

void LogBackend::LogBackendImpl::setSyslogFacility( const SyslogFacility::Facility facility )
{
	m_syslogLogger.setFacility( facility );
}

void LogBackend::LogBackendImpl::setSyslogIdent( const std::string ident )
{
	m_syslogLogger.setIdent( ident );
}

#if defined( _WIN32 )
void LogBackend::LogBackendImpl::setEventlogLevel( const LogLevel::Level level )
{
	eventlogLogger_.setLevel( level );
}

void LogBackend::LogBackendImpl::setEventlogLog( const std::string log )
{
	eventlogLogger_.setLog( log );
}

void LogBackend::LogBackendImpl::setEventlogSource( const std::string source )
{
	eventlogLogger_.setSource( source );
}

void LogBackend::LogBackendImpl::setWinDebugLevel( const LogLevel::Level level )
{
	windebugLogger_.setLevel( level );
}

#endif // defined( _WIN32 )

inline void LogBackend::LogBackendImpl::log( const LogLevel::Level level, const std::string& msg )
{
	m_consoleLogger.log( level, msg );
	m_logfileLogger.log( level, msg );
	m_syslogLogger.log( level, msg );
#if defined( _WIN32 )
	windebugLogger_.log( level, msg );
	eventlogLogger_.log( level, msg );
#endif // defined( _WIN32 )
}


LogLevel::Level LogBackend::LogBackendImpl::minLogLevel() const
{
	using namespace std;
	LogLevel::Level uLvl = min( m_consoleLogger.level(),
				    min( m_logfileLogger.level(), m_syslogLogger.level() ));
#if defined( _WIN32 )
	uLvl = std::min ( uLvl, std::min ( windebugLogger_.level(), eventlogLogger_.level() ));
#endif // defined( _WIN32 )
	return uLvl;
}

// Log backend PIMPL redirection
LogBackend::LogBackend() : impl_( new LogBackendImpl )	{}

LogBackend::~LogBackend()	{ delete impl_; }

void LogBackend::setConsoleLevel( const LogLevel::Level level )	{ impl_->setConsoleLevel( level ); }

void LogBackend::setLogfileLevel( const LogLevel::Level level )	{ impl_->setLogfileLevel( level ); }

void LogBackend::setLogfileName( const std::string filename )	{ impl_->setLogfileName( filename );}

void LogBackend::setSyslogLevel( const LogLevel::Level level )	{ impl_->setSyslogLevel( level );}

void LogBackend::setSyslogFacility( const SyslogFacility::Facility facility )	{ impl_->setSyslogFacility( facility );}

void LogBackend::setSyslogIdent( const std::string ident )	{ impl_->setSyslogIdent( ident );}

#if defined( _WIN32 )
void LogBackend::setEventlogLevel( const LogLevel::Level level ){ impl_->setEventlogLevel( level );}

void LogBackend::setEventlogLog( const std::string log )	{ impl_->setEventlogLog( log ); }

void LogBackend::setEventlogSource( const std::string source )	{ impl_->setEventlogSource( source ); }

void LogBackend::setWinDebugLevel( const LogLevel::Level level ) { impl_->setWinDebugLevel( level ); }

#endif // defined( _WIN32 )

void LogBackend::log( const LogLevel::Level level, const std::string& msg )
								{ impl_->log( level, msg ); }
LogLevel::Level LogBackend::minLogLevel() const			{ return impl_->minLogLevel(); }

}} // namespace _Wolframe::log
