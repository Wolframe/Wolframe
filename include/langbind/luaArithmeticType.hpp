 
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
///\file langbind/luaTypeArtithmetic.hpp
///\brief Type Template for arithmetic type bindings in lua
/*
	The arithmetic type should have a constructor from string, direct construction
	from other types or more than one argument
	are not supported yet. If need it will be supported in the future.

	Besides the mandatory constructor, the arithmetic type binding checks for
	some methods and operators ans binds them to the lua state.
*/

#ifndef _Wolframe_LANGBIND_LUA_ARTITHMETIC_TYPE_HPP_INCLUDED
#define _Wolframe_LANGBIND_LUA_ARTITHMETIC_TYPE_HPP_INCLUDED
#include "langbind/luaException.hpp"
#include "langbind/luaCppCall.hpp"
#include <string>
#include <stdexcept>
#include <cstring>
#include <boost/integer_traits.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/remove_const.hpp>
#include <boost/numeric/conversion/cast.hpp>

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "luaconf.h"
}

namespace _Wolframe {
namespace langbind {

///\brief Operators and Methods checked in this module
template<typename TYPE>
struct Interface
{
	TYPE operator + ( const TYPE&) const;
	TYPE operator + ( long) const;
	TYPE operator - ( const TYPE&) const;
	TYPE operator - ( long) const;
	TYPE operator / ( const TYPE&) const;
	TYPE operator / ( long) const;
	TYPE operator * ( const TYPE&) const;
	TYPE operator * ( long) const;
	TYPE operator - () const;
	std::string tostring() const;
	void format( unsigned int show_prec, unsigned int calc_prec);

	bool operator <( const TYPE&) const;
	bool operator <=( const TYPE&) const;
	bool operator ==( const TYPE&) const;
};


template<typename T>
struct has_operator_tostring
{
	typedef char small_type;
	struct large_type {small_type dummy[2];};

	template<const std::string& (T::*)() const> struct tester_const_member_signature;
	template<std::string (T::*)() const> struct tester_member_signature;

	template<typename U>
	static small_type has_matching_member(tester_member_signature<&U::tostring >*);
	template<typename U>
	static small_type has_matching_member(tester_const_member_signature<&U::tostring >*);
	template<typename U>
	static large_type has_matching_member(...);

	static const bool value=sizeof(has_matching_member<T>(0))==sizeof(small_type);
};

template<typename T>
struct has_function_format
{
	typedef char small_type;
	struct large_type {small_type dummy[2];};

	template<void (T::*)( unsigned int, unsigned int)> struct tester_member_signature;

	template<typename U>
	static small_type has_matching_member(tester_member_signature<&U::tostring >*);
	template<typename U>
	static large_type has_matching_member(...);

	static const bool value=sizeof(has_matching_member<T>(0))==sizeof(small_type);
};

template<typename T>
struct has_operator_neg
{
	typedef char small_type;
	struct large_type {small_type dummy[2];};

	template<T (T::*)()> struct tester_member_signature;

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

	template<T (T::*)( const T&) const> struct tester_member_signature;

	template<typename U>
	static small_type has_matching_member(tester_member_signature<&U::operator+ >*);
	template<typename U>
	static large_type has_matching_member(...);

	static const bool value=sizeof(has_matching_member<T>(0))==sizeof(small_type);
};

template<typename T>
struct has_operator_add_long
{
	typedef char small_type;
	struct large_type {small_type dummy[2];};

	template<T (T::*)( long) const> struct tester_member_signature;

	template<typename U>
	static small_type has_matching_member(tester_member_signature<&U::operator+ >*);
	template<typename U>
	static large_type has_matching_member(...);

	static const bool value=sizeof(has_matching_member<T>(0))==sizeof(small_type);
};

template<typename T>
struct has_operator_sub
{
	typedef char small_type;
	struct large_type {small_type dummy[2];};

	template<T (T::*)( const T&) const> struct tester_member_signature;

	template<typename U>
	static small_type has_matching_member(tester_member_signature<&U::operator- >*);
	template<typename U>
	static large_type has_matching_member(...);

	static const bool value=sizeof(has_matching_member<T>(0))==sizeof(small_type);
};

template<typename T>
struct has_operator_diff
{
	typedef char small_type;
	struct large_type {small_type dummy[2];};

	template<long (T::*)( const T&) const> struct tester_member_signature1;
	template<T (T::*)( long) const> struct tester_member_signature2;

	template<typename U>
	static small_type has_matching_member1(tester_member_signature1<&U::operator- >*);
	template<typename U>
	static large_type has_matching_member1(...);
	template<typename U>
	static small_type has_matching_member2(tester_member_signature2<&U::operator- >*);
	template<typename U>
	static large_type has_matching_member2(...);

	static const bool value=sizeof(has_matching_member1<T>(0))==sizeof(small_type)
				&& sizeof(has_matching_member2<T>(0))==sizeof(small_type);
};

template<typename T>
struct has_operator_sub_long
{
	typedef char small_type;
	struct large_type {small_type dummy[2];};

	template<T (T::*)( long) const> struct tester_member_signature;

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

	template<T (T::*)( const T&) const> struct tester_member_signature;

	template<typename U>
	static small_type has_matching_member(tester_member_signature<&U::operator* >*);
	template<typename U>
	static large_type has_matching_member(...);

	static const bool value=sizeof(has_matching_member<T>(0))==sizeof(small_type);
};

template<typename T>
struct has_operator_mul_long
{
	typedef char small_type;
	struct large_type {small_type dummy[2];};

	template<T (T::*)( long) const> struct tester_member_signature;

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

	template<T (T::*)( const T&) const> struct tester_member_signature;

	template<typename U>
	static small_type has_matching_member(tester_member_signature<&U::operator/ >*);
	template<typename U>
	static large_type has_matching_member(...);

	static const bool value=sizeof(has_matching_member<T>(0))==sizeof(small_type);
};

template<typename T>
struct has_operator_div_long
{
	typedef char small_type;
	struct large_type {small_type dummy[2];};

	template<T (T::*)( long) const> struct tester_member_signature;

	template<typename U>
	static small_type has_matching_member(tester_member_signature<&U::operator/ >*);
	template<typename U>
	static large_type has_matching_member(...);

	static const bool value=sizeof(has_matching_member<T>(0))==sizeof(small_type);
};

template<typename T>
struct has_operator_le
{
	typedef char small_type;
	struct large_type {small_type dummy[2];};

	template<bool (T::*)( const T&) const> struct tester_member_signature;

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

	template<bool (T::*)( const T&) const> struct tester_member_signature;

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

	template<bool (T::*)( const T&) const> struct tester_member_signature;

	template<typename U>
	static small_type has_matching_member(tester_member_signature<&U::operator== >*);
	template<typename U>
	static large_type has_matching_member(...);

	static const bool value=sizeof(has_matching_member<T>(0))==sizeof(small_type);
};

template <class ArithmeticType, class MetaTypeInfo>
struct LuaArithmeticType :public ArithmeticType
{
	LuaArithmeticType( const std::string& str)
		:ArithmeticType(str){}
	LuaArithmeticType( const ArithmeticType& o)
		:ArithmeticType(o){}
	LuaArithmeticType(){}

	static void createMetatable( lua_State* ls)
	{
		luaL_newmetatable( ls, MetaTypeInfo::metatableName());

		lua_pushliteral( ls, "__gc");
		lua_pushcfunction( ls, &destroy);
		lua_rawset( ls, -3);
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

	//\brief does nothing because the LUA garbage collector does the job.
	//\warning CAUTION: DO NOT CALL THIS FUNCTION ! DOES NOT WORK ON MSVC 9.0. (The compiler links with the std delete) (just avoids C4291 warning)
	void operator delete (void *, lua_State*) {}

	static int destroy( lua_State* ls)
	{
		LuaArithmeticType *THIS = (LuaArithmeticType*)lua_touserdata( ls, 1);
		if (THIS) THIS->~LuaArithmeticType();
		return 0;
	}

	static void push_luastack( lua_State* ls, const ArithmeticType& o)
	{
		try
		{
			(void*)new (ls) LuaArithmeticType( o);
		}
		catch (const std::exception&)
		{
			luaL_error( ls, "memory allocation error in lua context");
		}
	}

	static ArithmeticType* get( lua_State* ls, int index)
	{
		ArithmeticType* rt = (ArithmeticType*) luaL_testudata( ls, index, MetaTypeInfo::metatableName());
		if (!rt)
		{
			const char* arg = lua_tostring( ls, index);
			if (arg == 0) throw std::runtime_error( "cannot convert argument to arithmetic type");
			LuaExceptionHandlerScope escope(ls);
			{
				(void)new (ls) LuaArithmeticType( std::string(arg));
			}
			rt = (ArithmeticType*) luaL_checkudata( ls, -1, MetaTypeInfo::metatableName());
		}
		return rt;
	}
};

template <class ObjectType, class MetaTypeInfo>
struct LuaBinOperatorAdd
{
	typedef LuaBinOperatorAdd This;
	typedef LuaArithmeticType<ObjectType,MetaTypeInfo> ThisOperand;

	static int call( lua_State* ls)
	{
		LuaExceptionHandlerScope escope( ls);
		{
			ObjectType* self = ThisOperand::get( ls, 1);
			ObjectType* operand = ThisOperand::get( ls, 2);
			ThisOperand::push_luastack( ls, self->operator+( *operand));
		}
		return 1;
	}
	static int exec( lua_State* ls)
	{
		LuaFunctionCppCall<This> func;
		return func.run( "__add", ls);
	}
};

template <class ObjectType, class MetaTypeInfo>
struct LuaBinOperatorAddLong
{
	typedef LuaBinOperatorAddLong This;
	typedef LuaArithmeticType<ObjectType,MetaTypeInfo> ThisOperand;

	static int call( lua_State* ls)
	{
		LuaExceptionHandlerScope escope( ls);
		{
			ObjectType* self = ThisOperand::get( ls, 1);
			if (!lua_isnumber( ls, 2)) throw std::runtime_error( "number expected as 2nd argument");
			long operand = boost::numeric_cast<long>( lua_tonumber( ls, 2));
			ThisOperand::push_luastack( ls, self->operator+( operand));
		}
		return 1;
	}
	static int exec( lua_State* ls)
	{
		LuaFunctionCppCall<This> func;
		return func.run( "__add", ls);
	}
};

template <class ObjectType, class MetaTypeInfo>
struct LuaBinOperatorAddAny
{
	typedef LuaBinOperatorAddAny This;
	typedef LuaArithmeticType<ObjectType,MetaTypeInfo> ThisOperand;

	static int call( lua_State* ls)
	{
		if (lua_isnumber( ls, 2))
		{
			return LuaBinOperatorAddLong<ObjectType,MetaTypeInfo>::call(ls);
		}
		else
		{
			return LuaBinOperatorAdd<ObjectType,MetaTypeInfo>::call(ls);
		}
	}

	static int exec( lua_State* ls)
	{
		LuaFunctionCppCall<This> func;
		return func.run( "__add", ls);
	}
};

template <class ObjectType, class MetaTypeInfo>
struct LuaBinOperatorSub
{
	typedef LuaBinOperatorSub This;
	typedef LuaArithmeticType<ObjectType,MetaTypeInfo> ThisOperand;

	static int call( lua_State* ls)
	{
		LuaExceptionHandlerScope escope( ls);
		{
			ObjectType* self = ThisOperand::get( ls, 1);
			ObjectType* operand = ThisOperand::get( ls, 2);
			ThisOperand::push_luastack( ls, self->operator-( *operand));
		}
		return 1;
	}
	static int exec( lua_State* ls)
	{
		LuaFunctionCppCall<This> func;
		return func.run( "__sub", ls);
	}
};

template <class ObjectType, class MetaTypeInfo>
struct LuaBinOperatorSubLong
{
	typedef LuaBinOperatorSubLong This;
	typedef LuaArithmeticType<ObjectType,MetaTypeInfo> ThisOperand;

	static int call( lua_State* ls)
	{
		LuaExceptionHandlerScope escope( ls);
		{
			ObjectType* self = ThisOperand::get( ls, 1);
			if (!lua_isnumber( ls, 2)) throw std::runtime_error( "number expected as 2nd argument");
			long operand = boost::numeric_cast<long>( lua_tonumber( ls, 2));
			ThisOperand::push_luastack( ls, self->operator-( operand));
		}
		return 1;
	}
	static int exec( lua_State* ls)
	{
		LuaFunctionCppCall<This> func;
		return func.run( "__sub", ls);
	}
};

template <class ObjectType, class MetaTypeInfo>
struct LuaBinOperatorSubAny
{
	typedef LuaBinOperatorSubAny This;
	typedef LuaArithmeticType<ObjectType,MetaTypeInfo> ThisOperand;

	static int call( lua_State* ls)
	{
		if (lua_isnumber( ls, 2))
		{
			return LuaBinOperatorSubLong<ObjectType,MetaTypeInfo>::call(ls);
		}
		else
		{
			return LuaBinOperatorSub<ObjectType,MetaTypeInfo>::call(ls);
		}
	}

	static int exec( lua_State* ls)
	{
		LuaFunctionCppCall<This> func;
		return func.run( "__sub", ls);
	}
};

template <class ObjectType, class MetaTypeInfo>
struct LuaBinOperatorDiff
{
	typedef LuaBinOperatorDiff This;
	typedef LuaArithmeticType<ObjectType,MetaTypeInfo> ThisOperand;

	static int call( lua_State* ls)
	{
		LuaExceptionHandlerScope escope( ls);
		{
			ObjectType* self = ThisOperand::get( ls, 1);
			if (lua_isnumber( ls, 2))
			{
				long operand = boost::numeric_cast<long>( lua_tonumber( ls, 2));
				ThisOperand::push_luastack( ls, self->operator-( operand));
			}
			else
			{
				ObjectType* operand = ThisOperand::get( ls, 2);
				lua_pushinteger( ls, self->operator-( *operand));
			}
		}
		return 1;
	}
	static int exec( lua_State* ls)
	{
		LuaFunctionCppCall<This> func;
		return func.run( "__sub", ls);
	}
};

template <class ObjectType, class MetaTypeInfo>
struct LuaBinOperatorMul
{
	typedef LuaBinOperatorMul This;
	typedef LuaArithmeticType<ObjectType,MetaTypeInfo> ThisOperand;

	static int call( lua_State* ls)
	{
		LuaExceptionHandlerScope escope( ls);
		{
			ObjectType* self = ThisOperand::get( ls, 1);
			ObjectType* operand = ThisOperand::get( ls, 2);
			ThisOperand::push_luastack( ls, self->operator*( *operand));
		}
		return 1;
	}
	static int exec( lua_State* ls)
	{
		LuaFunctionCppCall<This> func;
		return func.run( "__mul", ls);
	}
};

template <class ObjectType, class MetaTypeInfo>
struct LuaBinOperatorMulLong
{
	typedef LuaBinOperatorMulLong This;
	typedef LuaArithmeticType<ObjectType,MetaTypeInfo> ThisOperand;

	static int call( lua_State* ls)
	{
		LuaExceptionHandlerScope escope( ls);
		{
			ObjectType* self = ThisOperand::get( ls, 1);
			if (!lua_isnumber( ls, 2)) throw std::runtime_error( "number expected as 2nd argument");
			long operand = boost::numeric_cast<long>( lua_tonumber( ls, 2));
			ThisOperand::push_luastack( ls, self->operator*( operand));
		}
		return 1;
	}
	static int exec( lua_State* ls)
	{
		LuaFunctionCppCall<This> func;
		return func.run( "__mul", ls);
	}
};

template <class ObjectType, class MetaTypeInfo>
struct LuaBinOperatorMulAny
{
	typedef LuaBinOperatorMulAny This;
	typedef LuaArithmeticType<ObjectType,MetaTypeInfo> ThisOperand;

	static int call( lua_State* ls)
	{
		if (lua_isnumber( ls, 2))
		{
			return LuaBinOperatorMulLong<ObjectType,MetaTypeInfo>::call(ls);
		}
		else
		{
			return LuaBinOperatorMul<ObjectType,MetaTypeInfo>::call(ls);
		}
	}

	static int exec( lua_State* ls)
	{
		LuaFunctionCppCall<This> func;
		return func.run( "__mul", ls);
	}
};

template <class ObjectType, class MetaTypeInfo>
struct LuaBinOperatorDiv
{
	typedef LuaBinOperatorDiv This;
	typedef LuaArithmeticType<ObjectType,MetaTypeInfo> ThisOperand;

	static int call( lua_State* ls)
	{
		LuaExceptionHandlerScope escope( ls);
		{
			ObjectType* self = ThisOperand::get( ls, 1);
			ObjectType* operand = ThisOperand::get( ls, 2);
			ThisOperand::push_luastack( ls, self->operator/( *operand));
		}
		return 1;
	}
	static int exec( lua_State* ls)
	{
		LuaFunctionCppCall<This> func;
		return func.run( "__div", ls);
	}
};

template <class ObjectType, class MetaTypeInfo>
struct LuaBinOperatorDivLong
{
	typedef LuaBinOperatorDivLong This;
	typedef LuaArithmeticType<ObjectType,MetaTypeInfo> ThisOperand;

	static int call( lua_State* ls)
	{
		LuaExceptionHandlerScope escope( ls);
		{
			ObjectType* self = ThisOperand::get( ls, 1);
			if (!lua_isnumber( ls, 2)) throw std::runtime_error( "number expected as 2nd argument");
			long operand = boost::numeric_cast<long>( lua_tonumber( ls, 2));
			ThisOperand::push_luastack( ls, self->operator/( operand));
		}
		return 1;
	}
	static int exec( lua_State* ls)
	{
		LuaFunctionCppCall<This> func;
		return func.run( "__div", ls);
	}
};

template <class ObjectType, class MetaTypeInfo>
struct LuaBinOperatorDivAny
{
	typedef LuaBinOperatorDivAny This;
	typedef LuaArithmeticType<ObjectType,MetaTypeInfo> ThisOperand;

	static int call( lua_State* ls)
	{
		if (lua_isnumber( ls, 2))
		{
			return LuaBinOperatorDivLong<ObjectType,MetaTypeInfo>::call(ls);
		}
		else
		{
			return LuaBinOperatorDiv<ObjectType,MetaTypeInfo>::call(ls);
		}
	}

	static int exec( lua_State* ls)
	{
		LuaFunctionCppCall<This> func;
		return func.run( "__div", ls);
	}
};

template <class ObjectType, class MetaTypeInfo>
struct LuaOperatorNeg
{
	typedef LuaOperatorNeg This;
	typedef LuaArithmeticType<ObjectType,MetaTypeInfo> ThisOperand;

	static int call( lua_State* ls)
	{
		LuaExceptionHandlerScope escope( ls);
		{
			ObjectType* self = ThisOperand::get( ls, 1);
			ThisOperand::push_luastack( ls, self->operator-());
		}
		return 1;
	}
	static int exec( lua_State* ls)
	{
		LuaFunctionCppCall<This> func;
		return func.run( "__neg", ls);
	}
};

template <class ObjectType, class MetaTypeInfo>
struct LuaOperatorToString
{
	typedef LuaOperatorToString This;
	typedef LuaArithmeticType<ObjectType,MetaTypeInfo> ThisOperand;

	static int call( lua_State* ls)
	{
		LuaExceptionHandlerScope escope( ls);
		{
			ObjectType* self = ThisOperand::get( ls, 1);
			std::string content = self->tostring();
			lua_pushlstring( ls, content.c_str(), content.size());
		}
		return 1;
	}
	static int exec( lua_State* ls)
	{
		LuaFunctionCppCall<This> func;
		return func.run( "__tostring", ls);
	}
};

template <class ObjectType, class MetaTypeInfo>
struct LuaFunctionFormat
{
	typedef LuaFunctionFormat This;
	typedef LuaArithmeticType<ObjectType,MetaTypeInfo> ThisOperand;

	static int call( lua_State* ls)
	{
		LuaExceptionHandlerScope escope( ls);
		{
			ObjectType* self = ThisOperand::get( ls, 1);
			if (lua_gettop( ls) < 2) throw std::runtime_error( "too few arguments");
			if (lua_gettop( ls) > 3) throw std::runtime_error( "too many arguments");
			unsigned int show_prec = 0;
			unsigned int calc_prec = 0;

			if (lua_gettop( ls) == 3)
			{
				if (lua_type( ls, 2) != LUA_TNUMBER) throw std::runtime_error( "number expected as 2nd argument");
				show_prec = lua_tointeger( ls, 2);
				if (lua_type( ls, 3) != LUA_TNUMBER) throw std::runtime_error( "number expected as 3rd argument");
				calc_prec = lua_tointeger( ls, 3);
			}
			else
			{
				if (lua_type( ls, 2) != LUA_TNUMBER) throw std::runtime_error( "number expected as 2nd argument");
				calc_prec = show_prec = lua_tointeger( ls, 2);
			}
			self->format( show_prec, calc_prec);
		}
		return 1;
	}
	static int exec( lua_State* ls)
	{
		LuaFunctionCppCall<This> func;
		return func.run( "format", ls);
	}
};

template <class ObjectType, class MetaTypeInfo>
struct LuaCmpOperatorEqual
{
	typedef LuaCmpOperatorEqual This;
	typedef LuaArithmeticType<ObjectType,MetaTypeInfo> ThisOperand;

	static int call( lua_State* ls)
	{
		LuaExceptionHandlerScope escope( ls);
		{
			ObjectType* self = ThisOperand::get( ls, 1);
			ObjectType* operand = ThisOperand::get( ls, 2);
			lua_pushboolean( ls, self->operator==( *operand));
		}
		return 1;
	}
	static int exec( lua_State* ls)
	{
		LuaFunctionCppCall<This> func;
		return func.run( "__eq", ls);
	}
};

template <class ObjectType, class MetaTypeInfo>
struct LuaCmpOperatorLessEqual
{
	typedef LuaCmpOperatorLessEqual This;
	typedef LuaArithmeticType<ObjectType,MetaTypeInfo> ThisOperand;

	static int call( lua_State* ls)
	{
		LuaExceptionHandlerScope escope( ls);
		{
			ObjectType* self = ThisOperand::get( ls, 1);
			ObjectType* operand = ThisOperand::get( ls, 2);
			lua_pushboolean( ls, self->operator<=( *operand));
		}
		return 1;
	}
	static int exec( lua_State* ls)
	{
		LuaFunctionCppCall<This> func;
		return func.run( "__le", ls);
	}
};

template <class ObjectType, class MetaTypeInfo>
struct LuaCmpOperatorLessThan
{
	typedef LuaCmpOperatorLessThan This;
	typedef LuaArithmeticType<ObjectType,MetaTypeInfo> ThisOperand;

	static int call( lua_State* ls)
	{
		LuaExceptionHandlerScope escope( ls);
		{
			ObjectType* self = ThisOperand::get( ls, 1);
			ObjectType* operand = ThisOperand::get( ls, 2);
			lua_pushboolean( ls, self->operator<( *operand));
		}
		return 1;
	}
	static int exec( lua_State* ls)
	{
		LuaFunctionCppCall<This> func;
		return func.run( "__lt", ls);
	}
};


template <typename T, typename M>
typename boost::enable_if_c<
	has_operator_eq<T>::value
	,void>::type defineOperator_eq( lua_State* ls)
{
	lua_pushliteral( ls, "__eq");
	lua_CFunction f = &LuaCmpOperatorEqual<T,M>::exec;
	lua_pushcfunction( ls, f);
	lua_rawset( ls, -3);
}
template <typename T, typename M>
typename boost::enable_if_c<
	! has_operator_eq<T>::value
	,void>::type defineOperator_eq( lua_State*)
{}

template <typename T, typename M>
typename boost::enable_if_c<
	has_operator_le<T>::value
	,void>::type defineOperator_le( lua_State* ls)
{
	lua_pushliteral( ls, "__le");
	lua_CFunction f = &LuaCmpOperatorLessEqual<T,M>::exec;
	lua_pushcfunction( ls, f);
	lua_rawset( ls, -3);
}
template <typename T, typename M>
typename boost::enable_if_c<
	! has_operator_le<T>::value
	,void>::type defineOperator_le( lua_State*)
{}

template <typename T, typename M>
typename boost::enable_if_c<
	has_operator_lt<T>::value
	,void>::type defineOperator_lt( lua_State* ls)
{
	lua_pushliteral( ls, "__lt");
	lua_CFunction f = &LuaCmpOperatorLessThan<T,M>::exec;
	lua_pushcfunction( ls, f);
	lua_rawset( ls, -3);
}
template <typename T, typename M>
typename boost::enable_if_c<
	! has_operator_lt<T>::value
	,void>::type defineOperator_lt( lua_State*)
{}

template <typename T, typename M>
typename boost::enable_if_c<
	has_operator_tostring<T>::value
	,void>::type defineOperator_tostring( lua_State* ls)
{
	lua_pushliteral( ls, "__tostring");
	lua_CFunction f = &LuaOperatorToString<T,M>::exec;
	lua_pushcfunction( ls, f);
	lua_rawset( ls, -3);
}
template <typename T, typename M>
typename boost::enable_if_c<
	! has_operator_tostring<T>::value
	,void>::type defineOperator_tostring( lua_State*)
{}

template <typename T, typename M>
typename boost::enable_if_c<
	has_function_format<T>::value
	,void>::type defineFunction_format( lua_State* ls)
{
	lua_pushliteral( ls, "format");
	lua_CFunction f = &LuaFunctionFormat<T,M>::exec;
	lua_pushcfunction( ls, f);
	lua_rawset( ls, -3);
}
template <typename T, typename M>
typename boost::enable_if_c<
	! has_function_format<T>::value
	,void>::type defineFunction_format( lua_State*)
{}


template <typename T, typename M>
typename boost::enable_if_c<
	has_operator_neg<T>::value
	,void>::type defineOperator_neg( lua_State* ls)
{
	lua_pushliteral( ls, "__unm");
	lua_CFunction f = &LuaOperatorNeg<T,M>::exec;
	lua_pushcfunction( ls, f);
	lua_rawset( ls, -3);
}
template <typename T, typename M>
typename boost::enable_if_c<
	! has_operator_neg<T>::value
	,void>::type defineOperator_neg( lua_State*)
{}

template <typename T, typename M>
typename boost::enable_if_c<
	has_operator_add<T>::value && ! has_operator_add_long<T>::value
	,void>::type defineOperator_add( lua_State* ls)
{
	lua_pushliteral( ls, "__add");
	lua_CFunction f = &LuaBinOperatorAdd<T,M>::exec;
	lua_pushcfunction( ls, f);
	lua_rawset( ls, -3);
}
template <typename T, typename M>
typename boost::enable_if_c<
	! has_operator_add<T>::value && has_operator_add_long<T>::value
	,void>::type defineOperator_add( lua_State* ls)
{
	lua_pushliteral( ls, "__add");
	lua_CFunction f = &LuaBinOperatorAddLong<T,M>::exec;
	lua_pushcfunction( ls, f);
	lua_rawset( ls, -3);
}
template <typename T, typename M>
typename boost::enable_if_c<
	has_operator_add<T>::value && has_operator_add_long<T>::value
	,void>::type defineOperator_add( lua_State* ls)
{
	lua_pushliteral( ls, "__add");
	lua_CFunction f = &LuaBinOperatorAddAny<T,M>::exec;
	lua_pushcfunction( ls, f);
	lua_rawset( ls, -3);
}
template <typename T, typename M>
typename boost::enable_if_c<
	! has_operator_add<T>::value && ! has_operator_add_long<T>::value
	,void>::type defineOperator_add( lua_State*)
{}

template <typename T, typename M>
typename boost::enable_if_c<
	has_operator_sub<T>::value && ! has_operator_sub_long<T>::value && ! has_operator_diff<T>::value
	,void>::type defineOperator_sub( lua_State* ls)
{
	lua_pushliteral( ls, "__sub");
	lua_CFunction f = &LuaBinOperatorSub<T,M>::exec;
	lua_pushcfunction( ls, f);
	lua_rawset( ls, -3);
}
template <typename T, typename M>
typename boost::enable_if_c<
	! has_operator_sub<T>::value && has_operator_sub_long<T>::value && ! has_operator_diff<T>::value
	,void>::type defineOperator_sub( lua_State* ls)
{
	lua_pushliteral( ls, "__sub");
	lua_CFunction f = &LuaBinOperatorSubLong<T,M>::exec;
	lua_pushcfunction( ls, f);
	lua_rawset( ls, -3);
}
template <typename T, typename M>
typename boost::enable_if_c<
	has_operator_sub<T>::value && has_operator_sub_long<T>::value && ! has_operator_diff<T>::value
	,void>::type defineOperator_sub( lua_State* ls)
{
	lua_pushliteral( ls, "__sub");
	lua_CFunction f = &LuaBinOperatorSubAny<T,M>::exec;
	lua_pushcfunction( ls, f);
	lua_rawset( ls, -3);
}
template <typename T, typename M>
typename boost::enable_if_c<
	has_operator_diff<T>::value
	,void>::type defineOperator_sub( lua_State* ls)
{
	lua_pushliteral( ls, "__sub");
	lua_CFunction f = &LuaBinOperatorDiff<T,M>::exec;
	lua_pushcfunction( ls, f);
	lua_rawset( ls, -3);
}
template <typename T, typename M>
typename boost::enable_if_c<
	! has_operator_sub<T>::value && ! has_operator_sub_long<T>::value && ! has_operator_diff<T>::value
	,void>::type defineOperator_sub( lua_State*)
{}

template <typename T, typename M>
typename boost::enable_if_c<
	has_operator_div<T>::value && ! has_operator_div_long<T>::value
	,void>::type defineOperator_div( lua_State* ls)
{
	lua_pushliteral( ls, "__div");
	lua_CFunction f = &LuaBinOperatorDiv<T,M>::exec;
	lua_pushcfunction( ls, f);
	lua_rawset( ls, -3);
}
template <typename T, typename M>
typename boost::enable_if_c<
	! has_operator_div<T>::value && has_operator_div_long<T>::value
	,void>::type defineOperator_div( lua_State* ls)
{
	lua_pushliteral( ls, "__div");
	lua_CFunction f = &LuaBinOperatorDivLong<T,M>::exec;
	lua_pushcfunction( ls, f);
	lua_rawset( ls, -3);
}
template <typename T, typename M>
typename boost::enable_if_c<
	has_operator_div<T>::value && has_operator_div_long<T>::value
	,void>::type defineOperator_div( lua_State* ls)
{
	lua_pushliteral( ls, "__div");
	lua_CFunction f = &LuaBinOperatorDivAny<T,M>::exec;
	lua_pushcfunction( ls, f);
	lua_rawset( ls, -3);
}
template <typename T, typename M>
typename boost::enable_if_c<
	! has_operator_div<T>::value && ! has_operator_div_long<T>::value
	,void>::type defineOperator_div( lua_State*)
{}

template <typename T, typename M>
typename boost::enable_if_c<
	has_operator_mul<T>::value && ! has_operator_mul_long<T>::value
	,void>::type defineOperator_mul( lua_State* ls)
{
	lua_pushliteral( ls, "__mul");
	lua_CFunction f = &LuaBinOperatorMul<T,M>::exec;
	lua_pushcfunction( ls, f);
	lua_rawset( ls, -3);
}
template <typename T, typename M>
typename boost::enable_if_c<
	! has_operator_mul<T>::value && has_operator_mul_long<T>::value
	,void>::type defineOperator_mul( lua_State* ls)
{
	lua_pushliteral( ls, "__mul");
	lua_CFunction f = &LuaBinOperatorMulLong<T,M>::exec;
	lua_pushcfunction( ls, f);
	lua_rawset( ls, -3);
}
template <typename T, typename M>
typename boost::enable_if_c<
	has_operator_mul<T>::value && has_operator_mul_long<T>::value
	,void>::type defineOperator_mul( lua_State* ls)
{
	lua_pushliteral( ls, "__mul");
	lua_CFunction f = &LuaBinOperatorMulAny<T,M>::exec;
	lua_pushcfunction( ls, f);
	lua_rawset( ls, -3);
}
template <typename T, typename M>
typename boost::enable_if_c<
	! has_operator_mul<T>::value && ! has_operator_mul_long<T>::value
	,void>::type defineOperator_mul( lua_State*)
{}

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



