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
/// \file logComponent.hpp
/// \brief Defines the components of the system
///

#ifndef _LOG_COMPONENT_HPP_INCLUDED
#define _LOG_COMPONENT_HPP_INCLUDED

namespace _Wolframe {
	namespace Logging {

	class LogComponent
	{
	public:
		/// Internal enum representing the components. The class around is
		/// needed to avoid funny automatic casts when using the log componet
		/// markers in the logging stream.
		enum Component {
			LOGCOMPONENT_NONE,		///< no loging component
			LOGCOMPONENT_LOGGING,		///< internal logger errors
			LOGCOMPONENT_NETWORK,		///< networking
			LOGCOMPONENT_AUTH,		///< authentication
			LOGCOMPONENT_LUA		///< lua processor
		};
		
	private:
		enum Component _component;

	public:			
		bool operator==( const LogComponent& o ) const {
			return _component == o._component;
		}
		
		enum Component component( ) const { return _component; }
		
		LogComponent( const enum Component& c = LOGCOMPONENT_NONE ) : _component( c ) { }
		
		const char* str( ) const;

		/// ostream marker for absence of component (usually not used directly)
		static const LogComponent LogNone;

		/// ostream marker for the logging component
		static const LogComponent LogLogging;

		/// ostream marker for the Lua handler
		static const LogComponent LogLua;

		/// ostream marker for networking part
		static const LogComponent LogNetwork;

		/// ostream marker for authentication component
		static const LogComponent LogAuth;			
	};
			
	} // namespace Logging
} // namespace _Wolframe

#endif // _LOG_COMPONENT_HPP_INCLUDED
