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
#include "langbind/luaDebug.hpp"
#include "langbind/luaException.hpp"
#include <stdexcept>
#include <cstddef>
#include <stdint.h>
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

static void hash_string( lua_State* ls, int index, std::size_t& val)
{
	static boost::hash<std::string> ht;
	val = (val >> 31) ^ (val << 1) ^ ht( std::string( lua_tostring( ls, index)));
}

static void hash_number( lua_State* ls, int index, std::size_t& val)
{
	static boost::hash<double> ht;
	val = (val >> 31) ^ (val << 1) ^ ht( lua_tonumber( ls, index));
}

static void hash_type( lua_State* ls, int index, std::size_t& val)
{
	static boost::hash<int> ht;
	val = (val >> 31) ^ (val << 1) ^ ht( lua_type( ls, index));
}

static void hash_userdata( lua_State* ls, int index, std::size_t& val)
{
	static boost::hash<uintptr_t> ht;
	val = (val >> 31) ^ (val << 1) ^ ht( (uintptr_t)lua_touserdata( ls, index));
}


static std::size_t tablehash( lua_State* ls, int index, int depth=1)
{
	std::size_t rt = 16777551;
	lua_pushvalue( ls, index);
	lua_pushnil( ls);
	while (lua_next( ls, -2) != 0)
	{
		hash_type( ls, -2, rt);
		hash_type( ls, -1, rt);

		switch (lua_type( ls, -2))
		{
			case LUA_TSTRING:
				hash_string( ls, -2, rt);
				break;

			case LUA_TNUMBER:
				hash_number( ls, -2, rt);
				break;

			case LUA_TUSERDATA:
			case LUA_TLIGHTUSERDATA:
				hash_userdata( ls, -2, rt);
				break;
		}
		switch (lua_type( ls, -1))
		{
			case LUA_TSTRING:
				hash_string( ls, -1, rt);
				break;

			case LUA_TNUMBER:
				hash_number( ls, -1, rt);
				break;

			case LUA_TUSERDATA:
			case LUA_TLIGHTUSERDATA:
				hash_userdata( ls, -1, rt);
				break;

			case LUA_TTABLE:
				if (depth > 0 && lua_isnumber( ls, -2))
				{
					rt = (rt >> 31) ^ (rt << 1) ^ tablehash( ls, -1, 0);
				}
		}
		lua_pop( ls, 1);
	}
	lua_pop( ls, 1);
	return rt;
}

static bool enter( lua_State* ls, int index, std::vector<std::size_t>& stk)
{
	std::size_t hh = tablehash( ls, index);
	std::vector<std::size_t>::const_iterator itr = stk.begin(), end = stk.end();
	for (; itr != end; ++itr)
	{
		if (*itr == hh) return false;
	}
	stk.push_back( hh);
	return true;
}

static void leave( std::vector<std::size_t>& stk)
{
	stk.pop_back();
}

static void getDescription_( lua_State *ls, int index, std::string& rt, std::vector<std::size_t>& stk)
{
	int type = lua_type( ls, index);
	switch (type)
	{
		case LUA_TUSERDATA:
			lua_pushvalue( ls, index);		///...STK: udata
			lua_getmetatable( ls, -1);		///...STK: udata mt
			lua_pushliteral( ls, "__tostring");	///...STK: udata mt __tostring
			lua_rawget( ls, -2);			///...STK: udata mt mt[__tostring]
			if (lua_isnil( ls, -1))
			{
				rt.append( "userdata ");
				getDescription_( ls, -2, rt, stk);
				lua_pop( ls, 3);		///... STK:
			}
			else
			{
				lua_pushvalue( ls, index);	///... STK: udata mt mt[__tostring] udata
				lua_call( ls, 1, 1);		///... STK: udata mt str
				rt.append( lua_tostring( ls, -1));
				lua_pop( ls, 3);		///... STK:
			}
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
			if (!lua_checkstack( ls, 10))
			{
				throw std::runtime_error( "lua stack overflow");
			}
			if (enter( ls, index, stk))
			{
				rt.append( "{ ");
				lua_pushvalue( ls, index);
				lua_pushnil( ls);
				while (lua_next( ls, -2) != 0)
				{
					getDescription_( ls, -2, rt, stk);
					bool istable = (lua_type( ls, -1) == LUA_TTABLE);
					rt.append( istable?"=":"='");
					getDescription_( ls, -1, rt, stk);
					rt.append( istable?" ":"' ");
					lua_pop( ls, 1);
				}
				lua_pop( ls, 1);
				rt.append( "}");
				leave( stk);
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

std::string _Wolframe::langbind::getDescription( lua_State *ls, int index)
{
	std::string rt;
	std::vector<std::size_t> stk;
	getDescription_( ls, index, rt, stk);
	return rt;
}

bool _Wolframe::langbind::getDescription( lua_State *ls, int index, std::string& ret)
{
	try
	{
		LuaExceptionHandlerScope exceptionHandler( ls);
		{
			std::vector<std::size_t> stk;
			getDescription_( ls, index, ret, stk);
		}
		return true;
	}
	catch (std::bad_alloc) { }
	catch (std::bad_cast) { }
	return false;
}

