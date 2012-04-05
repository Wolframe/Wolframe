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

static int bignum_add( lua_State* ls);
static int bignum_sub( lua_State* ls);
static int bignum_mul( lua_State* ls);
static int bignum_div( lua_State* ls);
static int bignum_pow( lua_State* ls);
static int bignum_lt( lua_State* ls);
static int bignum_le( lua_State* ls);
static int bignum_eq( lua_State* ls);
static int bignum_call( lua_State* ls);
static int bignum_unm( lua_State* ls);
static int bignum_tostring( lua_State* ls);
static int bignum_len( lua_State* ls);
static int bignum_gc( lua_State* ls);

static const luaL_Reg methodtable[] =
{
	{"__add",	&bignum_add},
	{"__sub",	&bignum_sub},
	{"__mul",	&bignum_mul},
	{"__div",	&bignum_div},
	{"__pow",	&bignum_pow},
	{"__lt",	&bignum_lt},
	{"__le",	&bignum_le},
	{"__eq",	&bignum_eq},
	{"__call",	&bignum_eq},
	{"__unm",	&bignum_unm},
	{"__tostring",	&bignum_tostring},
	{"__len",	&bignum_len},
	{"__gc",	&bignum_gc}
};

struct LuaBignum :public types::Bignum
{
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

	static void destroy()
	{
		LuaBignum *THIS = (LuaBignum*)lua_touserdata( ls, 1);
		if (THIS) THIS->~LuaObject();
	}
};

static int bignum_add( lua_State* ls);
static int bignum_sub( lua_State* ls);
static int bignum_mul( lua_State* ls);
static int bignum_div( lua_State* ls);
static int bignum_pow( lua_State* ls);
static int bignum_lt( lua_State* ls);
static int bignum_le( lua_State* ls);
static int bignum_eq( lua_State* ls);
static int bignum_call( lua_State* ls)
{
}

static int bignum_unm( lua_State* ls)
{
}

static int bignum_tostring( lua_State* ls)
{
}

static int bignum_len( lua_State* ls)
{
}

static int bignum_gc( lua_State* ls)
{
	LuaBignum::destroy();
}



