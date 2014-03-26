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
///\file luaDebug.hpp
#include "luaDebug.hpp"
#include "luaException.hpp"
#include <stdexcept>
#include <cstddef>
#include <string>
#include <vector>
#include "logger-v1.hpp"
#include <boost/lexical_cast.hpp>
#include <boost/functional/hash.hpp>
extern "C" {
#include "lualib.h"
#include "lauxlib.h"
#include "lua.h"
}

static void getDescription_( lua_State *ls, int index, std::string& rt)
{
	int type = lua_type( ls, index);
	switch (type)
	{
		case LUA_TLIGHTUSERDATA:
			rt.append( "lightuserdata");
			break;

		case LUA_TUSERDATA:
			rt.append( " userdata ");
			lua_pushvalue( ls, index);		///...STK: udata
			if (!lua_getmetatable( ls, index))	///...STK: udata mt
			{
				rt.append( "(lightuserdata)");
				lua_pop( ls, 1);
			}
			else
			{
				lua_pushliteral( ls, "__tostring");	///...STK: udata mt __tostring
				lua_rawget( ls, -2);			///...STK: udata mt mt[__tostring]
				if (lua_isnil( ls, -1))
				{
					lua_pop( ls, 3);		///... STK:
				}
				else
				{
					rt.append( " ");
					lua_pushvalue( ls, -3);		///... STK: udata mt mt[__tostring] udata
					lua_call( ls, 1, 1);		///... STK: udata mt str
					rt.append( lua_tostring( ls, -1));
					lua_pop( ls, 3);		///... STK:
				}
			}
			break;

		case LUA_TNIL:
			rt.append( "nil");
			break;

		case LUA_TBOOLEAN:
			rt.append( lua_toboolean( ls, index) ? "true":"false");
			break;

		case LUA_TSTRING:
		{
			std::size_t len;
			const char* ptr = lua_tolstring( ls, index, &len);
			rt.append( ptr, len);
			break;
		}

		case LUA_TNUMBER:
			rt.append( boost::lexical_cast<std::string>( lua_tonumber( ls, index)));
			break;

		case LUA_TFUNCTION:
			rt.append( "function");
			break;

		case LUA_TTABLE:
			if (!lua_checkstack( ls, 10))
			{
				throw std::runtime_error( "lua stack overflow");
			}
			rt.append( "{ ");
			lua_pushvalue( ls, index);
			lua_pushnil( ls);
			while (lua_next( ls, -2))
			{
				lua_pushvalue( ls, -2);
				const char *key = lua_tostring( ls, -1);

				if (key && std::memcmp( key, "__", 2) != 0)
				{
					rt.append( key);
					bool istable = (lua_type( ls, -2) == LUA_TTABLE);
					rt.append( istable?"=":"='");
					getDescription_( ls, -2, rt);
					rt.append( istable?" ":"' ");
				}
				lua_pop( ls, 2);
			}
			lua_pop( ls, 1);
			rt.append( "}");
			break;

		default:
			rt.append( "(none)");
	}
}

std::string _Wolframe::langbind::getDescription( lua_State *ls, int index)
{
	std::string rt;
	getDescription_( ls, index, rt);
	return rt;
}

bool _Wolframe::langbind::getDescription( lua_State *ls, int index, std::string& ret)
{
	try
	{
		LuaExceptionHandlerScope exceptionHandler( ls);
		{
			getDescription_( ls, index, ret);
		}
		return true;
	}
	catch (std::bad_alloc) { }
	catch (std::bad_cast) { }
	return false;
}

