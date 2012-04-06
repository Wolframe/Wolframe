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
///\file types_bignum.cpp
///\brief Implements a bignum type for Lua
#include "types/bignum.hpp"
extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

using namespace _Wolframe;
using namespace langbind;

namespace luaname
{
	static const char* Bignum = "wolframe.Bignum";
}

struct LuaBignum :public types::Bignum
{
	static const luaL_Reg methodtable[14];

	template <bool Fun(types::Bignum& ) LuaBignum::*method>
	static int

	static void createMetatable( lua_State* ls, lua_CFunction indexf, lua_CFunction newindexf, const luaL_Reg* mt)
	{
		luaL_newmetatable( ls, metaTableName<ObjectType>());
		lua_pushliteral( ls, "__index");
		if (indexf)
		{
			lua_pushcfunction( ls, indexf);
		}
		else
		{
			lua_pushvalue( ls, -2);
		}
		lua_rawset( ls, -3);

		lua_pushliteral( ls, "__newindex");
		if (newindexf)
		{
			lua_pushcfunction( ls, newindexf);
		}
		else
		{
			lua_pushvalue( ls, -2);
		}
		lua_rawset( ls, -3);

		if (mt) luaL_setfuncs( ls, mt, 0);

		lua_pushliteral( ls, "__gc");
		lua_pushcfunction( ls, destroy);
		lua_rawset( ls, -3);

		lua_pop( ls, 1);
	}

	void* operator new (std::size_t num_bytes, lua_State* ls) throw (std::bad_alloc)
	{
		void* rt = lua_newuserdata( ls, num_bytes);
		if (rt == 0) throw std::bad_alloc();
		luaL_getmetatable( ls, luaname::Bignum);
		lua_setmetatable( ls, -2);
		return rt;
	}

	//\brief does nothing because the LUA garbage collector does the job.
	//\warning CAUTION: DO NOT CALL THIS FUNCTION ! DOES NOT WORK ON MSVC 9.0. (The compiler links with the std delete) (just avoids C4291 warning)
	void operator delete (void *, lua_State*) {}

	static void destroy( lua_State* ls)
	{
		LuaBignum *THIS = (LuaBignum*)lua_touserdata( ls, 1);
		if (THIS) THIS->~LuaObject();
	}
};

static const luaL_Reg methodtable[] =
{
	{"__add",	&LuaBignum::lreg_add},
	{"__sub",	&LuaBignum::lreg_sub},
	{"__mul",	&LuaBignum::lreg_mul},
	{"__div",	&LuaBignum::lreg_div},
	{"__pow",	&LuaBignum::lreg_pow},
	{"__lt",	&LuaBignum::lreg_lt},
	{"__le",	&LuaBignum::lreg_le},
	{"__eq",	&LuaBignum::lreg_eq},
	{"__call",	&LuaBignum::lreg_eq},
	{"__unm",	&LuaBignum::lreg_unm},
	{"__tostring",	&LuaBignum::lreg_tostring},
	{"__len",	&LuaBignum::lreg_len},
	{"__gc",	&LuaBignum::destroy},
	{0,0}
};




