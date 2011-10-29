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
#include "luaDebug.hpp"
#include "logger-v1.hpp"
#include <boost/lexical_cast.hpp>

extern "C" {
#include "lualib.h"
#include "lauxlib.h"
#include "lua.h"
}

static void getDescription_( lua_State *ls, int index, std::string& rt, int depth)
{
	int type = lua_type( ls, index);
	switch (type)
	{
		case LUA_TUSERDATA:
			lua_getmetatable( ls, index);
			rt.append( "userdata ");
			if (depth >= 0) getDescription_( ls, -1, rt, depth-1);
			lua_pop( ls, 1);
			break;

		case LUA_TNIL:
			rt.append( "nil");
			break;

		case LUA_TSTRING: {
			rt.append( lua_tostring( ls, index));
			break;

		case LUA_TNUMBER:
			rt.append( boost::lexical_cast<std::string>( lua_tonumber( ls, index)));
			break;

		case LUA_TFUNCTION:
			rt.append( "function");
			break;

		case LUA_TTABLE:
			if (depth > 0)
			{
				rt.append( "{ ");
				lua_pushvalue( ls, index);
				lua_pushnil( ls);
				while (lua_next( ls, -2) != 0)
				{
					getDescription_( ls, -2, rt, depth-1);
					bool istable = (lua_type( ls, -1) == LUA_TTABLE);
					rt.append( istable?"=":"='");
					getDescription_( ls, -1, rt, depth-1);
					rt.append( istable?" ":"' ");
					lua_pop( ls, 1);
				}
				lua_pop( ls, 1);
				rt.append( "}");
			}
			else
			{
				rt.append( "{...}");
			}
			break;

		default:
			rt.append( "(none)");
		}
	}
}

using namespace _Wolframe;
using namespace iproc;
using namespace lua;

std::string _Wolframe::iproc::lua::getDescription( lua_State *ls, int index)
{
	std::string rt;
	getDescription_( ls, index, rt, 8);
	return rt;
}

bool _Wolframe::iproc::lua::getDescription( lua_State *ls, int index, std::string& ret)
{
	try
	{
		getDescription_( ls, index, ret, 8);
		return true;
	}
	catch (std::bad_alloc) { }
	catch (std::bad_cast) { }
	return false;
}

