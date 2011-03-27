/************************************************************************

 Copyright (C) 2011 Project Wolframe.
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
/// \file logComponent.cpp
/// \brief implementation of logging components
///

#include "logger.hpp"

#include <ostream>

namespace _Wolframe {
	namespace Logging {

	const LogComponent LogComponent::LogNone( LogComponent::LOGCOMPONENT_NONE );
	const LogComponent LogComponent::LogLogging( LogComponent::LOGCOMPONENT_LOGGING );
	const LogComponent LogComponent::LogNetwork( LogComponent::LOGCOMPONENT_NETWORK );
	const LogComponent LogComponent::LogAuth( LogComponent::LOGCOMPONENT_AUTH );
	const LogComponent LogComponent::LogLua( LogComponent::LOGCOMPONENT_LUA );

	/// output a logging component in an output stream
	const char* LogComponent::str( ) const {
		static const char *const s[] = {
			"", "Logging", "Network", "Auth", "Lua" };
		if( static_cast< size_t >( _component ) < ( sizeof( s ) / sizeof( *s ) ) ) {
			return s[_component];
		} else {
			return "";
		}
	}

	/// component marker in a logger stream, the specific logging method decides
	/// how to output the component of a log messages (event log category, prefix
	/// in a logfile, etc.)
	Logger& operator<<( Logger& logger, LogComponent c )
	{
		logger.component_ = c;
		return logger;
	}

	} // namespace Logging
} // namespace _Wolframe
