 
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
	The arithmetic type should have a constructor from string, direct construction
	from other types or more than one argument
	are not supported yet. If need it will be supported in the future.

	Besides the mandatory constructor, the arithmetic type binding checks for
	some methods and operators ans binds them to the lua state.
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

template <class ArithmeticType, class MetaTypeInfo>
static void createLuaArithmeticTypeMetatable( lua_State* ls)
{
	LuaArithmeticType<ArithmeticType,MetaTypeInfo>::createMetatable( ls);
	luaL_getmetatable( ls, MetaTypeInfo::metatableName());
	defineOperator_add<ArithmeticType, MetaTypeInfo>( ls);
	defineOperator_sub<ArithmeticType, MetaTypeInfo>( ls);
	defineOperator_mul<ArithmeticType, MetaTypeInfo>( ls);
	defineOperator_div<ArithmeticType, MetaTypeInfo>( ls);
	defineOperator_lt<ArithmeticType, MetaTypeInfo>( ls);
	defineOperator_le<ArithmeticType, MetaTypeInfo>( ls);
	defineOperator_eq<ArithmeticType, MetaTypeInfo>( ls);
	defineOperator_neg<ArithmeticType, MetaTypeInfo>( ls);
	defineOperator_tostring<ArithmeticType, MetaTypeInfo>( ls);
	lua_pop( ls, 1);
}

template <class ArithmeticType, class MetaTypeInfo>
struct LuaArithmeticTypeConstructor
{
	typedef LuaArithmeticTypeConstructor This;
	typedef LuaArithmeticType<ArithmeticType,MetaTypeInfo> ThisType;

	static int call( lua_State* ls)
	{
		if (lua_gettop( ls) == 0)
		{
			(void)new (ls) LuaArithmeticType<ArithmeticType,MetaTypeInfo>();
		}
		else if (lua_gettop( ls) == 1)
		{
			const char* arg = lua_tostring( ls , 1);
			if (!arg)
			{
				throw std::runtime_error( "argument not convertible to a string");
			}
			(void)new (ls) ThisType( std::string(arg));
		}
		else
		{
			throw std::runtime_error( "too many arguments");
		}
		return 1;
	}

	static int create( lua_State* ls)
	{
		LuaFunctionCppCall<This> func;
		return func.run( MetaTypeInfo::typeName(), ls);
	}
};

}}//namespace
#endif



