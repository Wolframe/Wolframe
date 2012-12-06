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
#include "luaDebug.hpp"
#include "langbind/luaException.hpp"
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

static void iterate_and_print(lua_State *L, int index, std::string indent)
{
    // Push another reference to the table on top of the stack (so we know
    // where it is, and this function can work for negative, positive and
    // pseudo indices
    lua_pushvalue(L, index);
    // stack now contains: -1 => table
    lua_pushnil(L);
    // stack now contains: -1 => nil; -2 => table
    while (lua_next(L, -2))
    {
        // stack now contains: -1 => value; -2 => key; -3 => table
        // copy the key so that lua_tostring does not modify the original
        lua_pushvalue(L, -2);
        // stack now contains: -1 => key; -2 => value; -3 => key; -4 => table
        const char *key = lua_tostring(L, -1);
        if (lua_istable( L, -2))
        {
		std::cout << indent << (key?key:"*") << ":" << std::endl;
		iterate_and_print( L, -2, indent + "   ");
	}
	else
	{
		const char *value = lua_tostring(L, -2);
		std::cout << indent << (key?key:"*") << " " << (value?value:"*") << std::endl;
	}
        // pop value + copy of key, leaving original key
        lua_pop(L, 2);
        // stack now contains: -1 => key; -2 => table
    }
    // stack now contains: -1 => table (when lua_next returns 0 it pops the key
    // but does not push anything.)
    // Pop table
    lua_pop(L, 1);
    // Stack is now the same as it was on entry to this function
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
			rt.append( "userdata");
			lua_pushvalue( ls, index);		///...STK: udata
			lua_getmetatable( ls, -1);		///...STK: udata mt
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
			break;

		case LUA_TNIL:
			rt.append( "nil");
			break;

		case LUA_TBOOLEAN:
			rt.append( lua_toboolean( ls, index) ? "true":"false");
			break;

		case LUA_TSTRING:
			rt.append( lua_tostring( ls, index));
			break;

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
			iterate_and_print( ls, index, " ");

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

