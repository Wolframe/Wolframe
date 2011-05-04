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
#include "luaDebug.hpp"
#include "logger.hpp"
#include <string>

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

using namespace _Wolframe;
using namespace iproc;
using namespace lua;

int _Wolframe::iproc::lua::printLog( lua_State *ls)
{
	/* first parameter maps to a log level, rest gets printed depending on
	 * whether it's a string or a number
	 */
	int ii,nn = lua_gettop(ls);
	if (nn <= 0) luaL_error( ls, "no arguments passed to logger");

	const char *logLevel = luaL_checkstring( ls, 1);
	std::string logmsg;

	for (ii=2; ii<=nn; ii++)
	{
		if (!getDescription( ls, ii, logmsg))
		{
			luaL_error( ls, "failed to map printLog arguments to a string");
		}
	}

	_Wolframe::log::Logger( _Wolframe::log::LogBackend::instance() ).Get(
				_Wolframe::log::LogLevel::strToLogLevel( logLevel ) )
		<< _Wolframe::log::LogComponent::LogLua
		<< logmsg;
	return 0;
}

