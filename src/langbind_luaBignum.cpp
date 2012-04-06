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
#include <limits>
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

template<typename T>
struct has_operator_tostring
{
	typedef char small_type;
	struct large_type {small_type dummy[2];};

	template<const std::string& (T::*)() const> struct tester_const_member_signature;
	template<std::string (T::*)()> struct tester_member_signature;

	template<typename U>
	static small_type has_matching_member(tester_member_signature<&U::tostring >*);
	template<typename U>
	static small_type has_matching_member(tester_const_member_signature<&U::tostring >*);
	template<typename U>
	static large_type has_matching_member(...);

	static const bool value=sizeof(has_matching_member<T>(0))==sizeof(small_type);
};

template<typename T>
struct has_operator_len
{
	typedef char small_type;
	struct large_type {small_type dummy[2];};

	template<std::size_t (T::*)()> struct tester_member_signature;

	template<typename U>
	static small_type has_matching_member(tester_member_signature<&U::len >*);
	template<typename U>
	static large_type has_matching_member(...);

	static const bool value=sizeof(has_matching_member<T>(0))==sizeof(small_type);
};

template<typename T>
struct has_operator_pow
{
	typedef char small_type;
	struct large_type {small_type dummy[2];};

	template<T& (T::*)(unsigned int)> struct tester_member_signature;

	template<typename U>
	static small_type has_matching_member(tester_member_signature<&U::pow >*);
	template<typename U>
	static large_type has_matching_member(...);

	static const bool value=sizeof(has_matching_member<T>(0))==sizeof(small_type);
};

template<typename T>
struct has_operator_neg
{
	typedef char small_type;
	struct large_type {small_type dummy[2];};

	template<T& (T::*)()> struct tester_member_signature;

	template<typename U>
	static small_type has_matching_member(tester_member_signature<&U::operator- >*);
	template<typename U>
	static large_type has_matching_member(...);

	static const bool value=sizeof(has_matching_member<T>(0))==sizeof(small_type);
};

template<typename T>
struct has_operator_add
{
	typedef char small_type;
	struct large_type {small_type dummy[2];};

	template<T& (T::*)( const T&)> struct tester_member_signature;

	template<typename U>
	static small_type has_matching_member(tester_member_signature<&U::operator+ >*);
	template<typename U>
	static large_type has_matching_member(...);

	static const bool value=sizeof(has_matching_member<T>(0))==sizeof(small_type);
};

template<typename T>
struct has_operator_minus
{
	typedef char small_type;
	struct large_type {small_type dummy[2];};

	template<T& (T::*)( const T&)> struct tester_member_signature;

	template<typename U>
	static small_type has_matching_member(tester_member_signature<&U::operator- >*);
	template<typename U>
	static large_type has_matching_member(...);

	static const bool value=sizeof(has_matching_member<T>(0))==sizeof(small_type);
};

template<typename T>
struct has_operator_mul
{
	typedef char small_type;
	struct large_type {small_type dummy[2];};

	template<T& (T::*)( const T&)> struct tester_member_signature;

	template<typename U>
	static small_type has_matching_member(tester_member_signature<&U::operator* >*);
	template<typename U>
	static large_type has_matching_member(...);

	static const bool value=sizeof(has_matching_member<T>(0))==sizeof(small_type);
};

template<typename T>
struct has_operator_div
{
	typedef char small_type;
	struct large_type {small_type dummy[2];};

	template<T& (T::*)( const T&)> struct tester_member_signature;

	template<typename U>
	static small_type has_matching_member(tester_member_signature<&U::operator/>*);
	template<typename U>
	static large_type has_matching_member(...);

	static const bool value=sizeof(has_matching_member<T>(0))==sizeof(small_type);
};

template<typename T>
struct has_operator_le
{
	typedef char small_type;
	struct large_type {small_type dummy[2];};

	template<bool (T::*)( const T&)> struct tester_member_signature;

	template<typename U>
	static small_type has_matching_member(tester_member_signature<&U::operator<= >*);
	template<typename U>
	static large_type has_matching_member(...);

	static const bool value=sizeof(has_matching_member<T>(0))==sizeof(small_type);
};

template<typename T>
struct has_operator_lt
{
	typedef char small_type;
	struct large_type {small_type dummy[2];};

	template<bool (T::*)( const T&)> struct tester_member_signature;

	template<typename U>
	static small_type has_matching_member(tester_member_signature<&U::operator< >*);
	template<typename U>
	static large_type has_matching_member(...);

	static const bool value=sizeof(has_matching_member<T>(0))==sizeof(small_type);
};

template<typename T>
struct has_operator_eq
{
	typedef char small_type;
	struct large_type {small_type dummy[2];};

	template<bool (T::*)( const T&)> struct tester_member_signature;

	template<typename U>
	static small_type has_matching_member(tester_member_signature<&U::operator== >*);
	template<typename U>
	static large_type has_matching_member(...);

	static const bool value=sizeof(has_matching_member<T>(0))==sizeof(small_type);
};



struct LuaBignum :public types::Bignum
{
	static const char* metatableName()
	{
		return luaname::Bignum;
	}

	void getOperand( lua_State* ls, int index, unsigned int& x)
	{
		double value = lua_tonumber( ls, index);
		lua_Integer vv = lua_tointeger( ls, index);
		if (vv == 0)
		{
			lua_Number bb = lua_tonumber( ls, index);
			if (bb < 0) bb = -bb;
			if (bb < std::numeric_limits<double>::epsilon())
			{
				x = 0;
			}
			else
			{
				luaL_error( ls, "illegal operand (float) in operation");
			}
		}
		else
		{
			if (vv < 0)
			{
				luaL_error( ls, "illegal operand (negative) in operation");
			}
			x = vv;
		}
	}

	template <types::Bignum& types::Bignum::*method( const types::Bignum& operand)>
	struct LuaBinOperator
	{
		static int exec( lua_State* ls)
		{
			types::Bignum* self = get( ls, 1);
			types::Bignum* operand = get( ls, 2);
			try
			{
				types::Bignum result = (self->*method)( *operand);
				push_luastack( ls, result);
			}
			catch (const std::bad_alloc&)
			{
				luaL_error( ls, "memory allocation error in lua context");
			}
			catch (const std::exception&)
			{
				luaL_error( ls, "calculation exception in lua context");
			}
		}
	};

	template <Operand, types::Bignum& types::Bignum::*method( Operand operand)>
	struct LuaBinOperatorBase
	{
		static int exec( lua_State* ls)
		{
			types::Bignum* self = get( ls, 1);
			getOperand( ls, 2, operand);
			try
			{
				types::Bignum result = (self->*method)( operand);
				push_luastack( ls, result);
			}
			catch (const std::bad_alloc&)
			{
				luaL_error( ls, "memory allocation error in lua context");
			}
			catch (const std::exception&)
			{
				luaL_error( ls, "calculation exception in lua context");
			}
		}
	};

	template <bool types::Bignum::*method( const types::Bignum& operand)>
	struct LuaBinOperatorBool
	{
		static int exec( lua_State* ls)
		{
			types::Bignum* self = get( ls, 1);
			types::Bignum* operand = get( ls, 2);
			try
			{
				bool result = (self->*method)( *operand);
				lua_pushboolean( ls, result);
			}
			catch (const std::bad_alloc&)
			{
				luaL_error( ls, "memory allocation error in lua context");
			}
			catch (const std::exception&)
			{
				luaL_error( ls, "calculation exception in lua context");
			}
		}
	};

	static void push_property( lua_State* ls, const std::string& p)
	{
		lua_pushlstring( ls, p.c_str(), p.size());
	}

	static void push_property( lua_State* ls, std::size_t p)
	{
		lua_pushnumber( ls, p);
	}

	static void push_property( lua_State* ls, const types::Bignum& p)
	{
		push_luastack( ls, p);
	}

	template <PROPTYPE, PROPTYPE types::Bignum::*method()>
	struct LuaPropertyOperator
	{
		static int exec( lua_State* ls)
		{
			types::Bignum* self = get( ls, 1);
			try
			{
				PROPTYPE result = (self->*method)();
				push_property( ls, result);
			}
			catch (const std::bad_alloc&)
			{
				luaL_error( ls, "memory allocation error in lua context");
			}
			catch (const std::exception&)
			{
				luaL_error( ls, "calculation exception in lua context");
			}
		}
	};

	typename boost::enable_if_c<
		has_operator_tostring<types::Bignum>::value
		,const void>::type defineOperator_tostring()
	{
		lua_pushliteral( ls, "__tostring");
		lua_pushcfunction( ls, LuaPropertyOperator<std::string,types::Bignum::tostring >);
		lua_rawset( ls, -3);
	}
	typename boost::enable_if_c<
		! has_operator_tostring<types::Bignum>::value
		,const void>::type defineOperator_tostring()
	{}

	typename boost::enable_if_c<
		has_operator_len<types::Bignum>::value
		,const void>::type defineOperator_len()
	{
		lua_pushliteral( ls, "__len");
		lua_pushcfunction( ls, LuaPropertyOperator<unsigned int,types::Bignum::len >);
		lua_rawset( ls, -3);
	}
	typename boost::enable_if_c<
		! has_operator_len<types::Bignum>::value
		,const void>::type defineOperator_len()
	{}

	typename boost::enable_if_c<
		has_operator_neg<types::Bignum>::value
		,const void>::type defineOperator_neg()
	{
		lua_pushliteral( ls, "__unm");
		lua_pushcfunction( ls, LuaPropertyOperator<types::Bignum,types::Bignum::operator-() >);
		lua_rawset( ls, -3);
	}
	typename boost::enable_if_c<
		! has_operator_neg<types::Bignum>::value
		,const void>::type defineOperator_neg()
	{}

	typename boost::enable_if_c<
		has_operator_add<types::Bignum>::value
		,const void>::type defineOperator_add()
	{
		lua_pushliteral( ls, "__add");
		lua_pushcfunction( ls, LuaBinOperator<types::Bignum::operator+ >);
		lua_rawset( ls, -3);
	}
	typename boost::enable_if_c<
		! has_operator_add<types::Bignum>::value
		,const void>::type defineOperator_add()
	{}

	typename boost::enable_if_c<
		has_operator_sub<types::Bignum>::value
		,const void>::type defineOperator_sub()
	{
		lua_pushliteral( ls, "__sub");
		lua_pushcfunction( ls, LuaBinOperator<types::Bignum::operator- >);
		lua_rawset( ls, -3);
	}
	typename boost::enable_if_c<
		! has_operator_sub<types::Bignum>::value
		,const void>::type defineOperator_sub()
	{}

	typename boost::enable_if_c<
		has_operator_div<types::Bignum>::value
		,const void>::type defineOperator_div()
	{
		lua_pushliteral( ls, "__div");
		lua_pushcfunction( ls, LuaBinOperator<types::Bignum::operator/ >);
		lua_rawset( ls, -3);
	}
	typename boost::enable_if_c<
		! has_operator_div<types::Bignum>::value
		,const void>::type defineOperator_div()
	{}

	typename boost::enable_if_c<
		has_operator_mul<types::Bignum>::value
		,const void>::type defineOperator_mul()
	{
		lua_pushliteral( ls, "__mul");
		lua_pushcfunction( ls, LuaBinOperator<types::Bignum::operator* >);
		lua_rawset( ls, -3);
	}
	typename boost::enable_if_c<
		! has_operator_mul<types::Bignum>::value
		,const void>::type defineOperator_mul()
	{}

	typename boost::enable_if_c<
		has_operator_pow<types::Bignum>::value
		,const void>::type defineOperator_pow()
	{
		lua_pushliteral( ls, "__pow");
		lua_pushcfunction( ls, LuaBinOperatorBase<unsigned int, types::Bignum::pow >);
		lua_rawset( ls, -3);
	}
	typename boost::enable_if_c<
		! has_operator_pow<types::Bignum>::value
		,const void>::type defineOperator_pow()
	{}

	typename boost::enable_if_c<
		has_operator_le<types::Bignum>::value
		,const void>::type defineOperator_le()
	{
		lua_pushliteral( ls, "__le");
		lua_pushcfunction( ls, LuaBinOperatorBool<types::Bignum::operator<= >);
		lua_rawset( ls, -3);
	}
	typename boost::enable_if_c<
		! has_operator_le<types::Bignum>::value
		,const void>::type defineOperator_le()
	{}

	typename boost::enable_if_c<
		has_operator_lt<types::Bignum>::value
		,const void>::type defineOperator_lt()
	{
		lua_pushliteral( ls, "__lt");
		lua_pushcfunction( ls, LuaBinOperatorBool<types::Bignum::operator< >);
		lua_rawset( ls, -3);
	}
	typename boost::enable_if_c<
		! has_operator_lt<types::Bignum>::value
		,const void>::type defineOperator_lt()
	{}


	typename boost::enable_if_c<
		has_operator_eq<types::Bignum>::value
		,const void>::type defineOperator_eq()
	{
		lua_pushliteral( ls, "__eq");
		lua_pushcfunction( ls, LuaBinOperatorBool<types::Bignum::operator== >);
		lua_rawset( ls, -3);
	}
	typename boost::enable_if_c<
		! has_operator_eq<types::Bignum>::value
		,const void>::type defineOperator_eq()
	{}

	typename boost::enable_if_c<
		has_operator_pow<types::Bignum>::value
		,const void>::type defineOperator_pow()
	{
		lua_pushliteral( ls, "__pow");
		lua_pushcfunction( ls, LuaBinOperator<types::Bignum::operator* >);
		lua_rawset( ls, -3);
	}
	typename boost::enable_if_c<
		! has_operator_mul<types::Bignum>::value
		,const void>::type defineOperator_mul()
	{}

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

		defineOperator_add();
		defineOperator_sub();
		defineOperator_mul();
		defineOperator_div();
		defineOperator_pow();
		defineOperator_lt();
		defineOperator_le();
		defineOperator_eq();
		defineOperator_neg();
		defineOperator_tostring();
		defineOperator_len();

		lua_pop( ls, 1);
	}

	void* operator new (std::size_t num_bytes, lua_State* ls) throw (std::bad_alloc)
	{
		void* rt = lua_newuserdata( ls, num_bytes);
		if (rt == 0) throw std::bad_alloc();
		luaL_getmetatable( ls, metatableName());
		lua_setmetatable( ls, -2);
		return rt;
	}

	static void push_luastack( lua_State* ls, const types::Bignum& o)
	{
		try
		{
			const char* mt = metatableName();
			(void*)new (ls) types::Bignum( o);
		}
		catch (const std::bad_alloc&)
		{
			luaL_error( ls, "memory allocation error in lua context");
		}
	}

	//\brief does nothing because the LUA garbage collector does the job.
	//\warning CAUTION: DO NOT CALL THIS FUNCTION ! DOES NOT WORK ON MSVC 9.0. (The compiler links with the std delete) (just avoids C4291 warning)
	void operator delete (void *, lua_State*) {}

	static void destroy( lua_State* ls)
	{
		LuaBignum *THIS = (LuaBignum*)lua_touserdata( ls, 1);
		if (THIS) THIS->~LuaObject();
	}

	static LuaObject* get( lua_State* ls, int index)
	{
		LuaObject* rt = (LuaObject*) luaL_checkudata( ls, index, metatableName());
		return rt;
	}

};



