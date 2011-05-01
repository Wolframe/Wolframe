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
#include "luaLog.hpp"
#include "logger.hpp"

extern "C" {
#include "lualib.h"
#include "lauxlib.h"
#include "lua.h"
}

using namespace _Wolframe;
using namespace iproc;
using namespace lua;

// Aba: for snprintf, maybe better to use ostringstream?
#ifndef _WIN32
#define __STDC_FORMAT_MACROS
#include <cstdio>
#include <inttypes.h>
#else
#define snprintf _snprintf
#define PRIxPTR "%p"
#endif

int _Wolframe::iproc::lua::printLog( lua_State *ls)
{
	/* first parameter maps to a log level, rest gets printed depending on
	 * whether it's a string or a number
	 */
	const char *logLevel = luaL_checkstring( ls, 1 );

	std::ostringstream os;
	int n = lua_gettop( ls ) - 1;
	int i = 2;
	for( ; n--; i++ ) {
		int type = lua_type( ls, i );

		switch( type ) {
		case LUA_TNIL:
			os << "nil";
			break;

		case LUA_TSTRING: {
			const char *v = lua_tostring( ls, i );
			os << v;
		}
		break;

		case LUA_TNUMBER: {
			lua_Number v = lua_tonumber( ls, i );
			os << v;
		}
		break;

		case LUA_TFUNCTION: {
			lua_CFunction f = lua_tocfunction( ls, i );
			char buf[33];
			snprintf( buf, 32, "function[%016" PRIxPTR "]", (uintptr_t)f );
			os << buf;
		}
		break;

		case LUA_TTABLE:
			// TODO

		default:
			os << "<unknown>";
		}
	}

	_Wolframe::log::Logger( _Wolframe::log::LogBackend::instance() ).Get(
				_Wolframe::log::LogLevel::strToLogLevel( logLevel ) )
		<< _Wolframe::log::LogComponent::LogLua
		<< os.str( );

	lua_pop( ls, n );

	return 0;
}

