/************************************************************************
Copyright (C) 2011 - 2013 Project Wolframe.
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
///\file luaBcdNumber.cpp
///\brief Implements a BCD number type of arbitrary length ("bigint") and a BCD fixed point number of any defined precision and arbitrary length ("bignum") for Lua
#include "luaBcdNumber.hpp"
#include "types/bcdArithmetic.hpp"
#include "langbind/luaArithmeticType.hpp"
#include <limits>
extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

using namespace _Wolframe;
using namespace langbind;

namespace
{

struct LuaBigintMetaInfo
{
	static const char* metatableName()
	{
		return "wolframe.bigint";
	}
	static const char* typeName()
	{
		return "bigint";
	}
};

typedef LuaArithmeticType<types::BigBCD, LuaBigintMetaInfo> LuaBigint;
typedef LuaArithmeticTypeConstructor<types::BigBCD, LuaBigintMetaInfo> ConstructorLuaBigint;

struct LuaBignumMetaInfo
{
	static const char* metatableName()
	{
		return "wolframe.bignum";
	}
	static const char* typeName()
	{
		return "bignum";
	}
};

typedef LuaArithmeticType<types::BigNumber, LuaBignumMetaInfo> LuaBignum;
typedef LuaArithmeticTypeConstructor<types::BigNumber, LuaBignumMetaInfo> ConstructorLuaBignum;

}//anonymous namespace

int _Wolframe::langbind::initBignumModule( lua_State* ls)
{
	createLuaArithmeticTypeMetatable<types::BigBCD, LuaBigintMetaInfo>( ls);
	lua_register( ls, "bigint", &ConstructorLuaBigint::create);

	createLuaArithmeticTypeMetatable<types::BigNumber, LuaBignumMetaInfo>( ls);
	lua_register( ls, "bignum", &ConstructorLuaBignum::create);
	return 0;
}

