 
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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
///\file langbind/luaTypeArtithmetic.hpp
///\brief Type Template for arithmetic types in lua
/*
	The arithmetic type should have a constructor from string, direct construction from other types or more than one argument
	is not supported yet. If need it will be supported in the future.

	Besides the mandatory constructor, the arithmetic type binding checks for the following optional methods and operators
	ans binds them to the lua state. This works for any subset of the interface _Wolframe::langbind::traits::Interface.
*/

#ifndef _Wolframe_LANGBIND_LUA_TYPE_ARTITHMETIC_HPP_INCLUDED
#define _Wolframe_LANGBIND_LUA_TYPE_ARTITHMETIC_HPP_INCLUDED
#include <string>
#include <stdexcept>
#include "langbind/luaTypeTraits.hpp"

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "luaconf.h"
}

namespace _Wolframe {
namespace langbind {

template <class ArithmenticType, class MetaTypeInfo>
struct LuaArithmenticTypeTemplate :public ArithmenticType
{
	LuaArithmenticTypeTemplate( const ArithmenticType& o)
		:ArithmenticType(o){}
	LuaArithmenticTypeTemplate(){}

	static void createMetatable( lua_State* ls)
	{
		luaL_newmetatable( ls, MetaTypeInfo::metatableName());

		lua_pushliteral( ls, "__gc");
		lua_pushcfunction( ls, &destroy);
		lua_rawset( ls, -3);

		traits::defineMethod_index<LuaArithmenticTypeTemplate>( ls);
		traits::defineMethod_newindex<LuaArithmenticTypeTemplate>( ls);
		traits::defineOperator_add<LuaArithmenticTypeTemplate>( ls);
		traits::defineOperator_sub<LuaArithmenticTypeTemplate>( ls);
		traits::defineOperator_mul<LuaArithmenticTypeTemplate>( ls);
		traits::defineOperator_div<LuaArithmenticTypeTemplate>( ls);
		traits::defineOperator_pow<LuaArithmenticTypeTemplate>( ls);
		traits::defineOperator_lt<LuaArithmenticTypeTemplate>( ls);
		traits::defineOperator_le<LuaArithmenticTypeTemplate>( ls);
		traits::defineOperator_eq<LuaArithmenticTypeTemplate>( ls);
		traits::defineOperator_neg<LuaArithmenticTypeTemplate>( ls);
		traits::defineOperator_tostring<LuaArithmenticTypeTemplate>( ls);
		traits::defineOperator_len<LuaArithmenticTypeTemplate>( ls);
		traits::defineMethod_get<LuaArithmenticTypeTemplate>( ls);
		traits::defineMethod_set<LuaArithmenticTypeTemplate>( ls);

		lua_pop( ls, 1);
	}

	void* operator new (std::size_t num_bytes, lua_State* ls) throw (std::bad_alloc)
	{
		void* rt = lua_newuserdata( ls, num_bytes);
		if (rt == 0) throw std::bad_alloc();
		luaL_getmetatable( ls, MetaTypeInfo::metatableName());
		lua_setmetatable( ls, -2);
		return rt;
	}

	static void push_luastack( lua_State* ls, const ArithmenticType& o)
	{
		try
		{
			const char* mt = MetaTypeInfo::metatableName();
			(void*)new (ls) LuaArithmenticTypeTemplate( o);
		}
		catch (const std::bad_alloc&)
		{
			luaL_error( ls, "memory allocation error in lua context");
		}
	}

	//\brief does nothing because the LUA garbage collector does the job.
	//\warning CAUTION: DO NOT CALL THIS FUNCTION ! DOES NOT WORK ON MSVC 9.0. (The compiler links with the std delete) (just avoids C4291 warning)
	void operator delete (void *, lua_State*) {}

	static int destroy( lua_State* ls)
	{
		LuaArithmenticTypeTemplate *THIS = (LuaArithmenticTypeTemplate*)lua_touserdata( ls, 1);
		if (THIS) THIS->~LuaArithmenticTypeTemplate();
		return 0;
	}

	static int create( lua_State* ls)
	{
		try
		{
			if (lua_gettop( ls) == 0)
			{
				(void)new (ls) LuaArithmenticTypeTemplate();
			}
			else if (lua_gettop( ls) == 1)
			{
				const char* arg = lua_tostring( ls , 1);
				if (!arg)
				{
					luaL_error( ls, "argument of 'new' not convertible to a string");
				}
				(void)new (ls) LuaArithmenticTypeTemplate( std::string(arg));
			}
			else
			{
				luaL_error( ls, "too many arguments for call of 'new'");
			}

		}
		catch (const std::bad_alloc&)
		{
			luaL_error( ls, "memory allocation error in lua context");
		}
		catch (const std::exception& e)
		{
			luaL_error( ls, "unexpected exception for arithmetic type in lua context: %s", e.what());
		}
		return 1;
	}

	static LuaArithmenticTypeTemplate* get( lua_State* ls, int index)
	{
		LuaArithmenticTypeTemplate* rt = (LuaArithmenticTypeTemplate*) luaL_checkudata( ls, index, MetaTypeInfo::metatableName());
		return rt;
	}
};

}}//namespace
#endif



