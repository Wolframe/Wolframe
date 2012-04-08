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
///\file langbind/luaTypeTraits.hpp
///\brief Type Traits for embedding types in lua binding
#ifndef _Wolframe_LANGBIND_LUA_TYPE_TRAITS_HPP_INCLUDED
#define _Wolframe_LANGBIND_LUA_TYPE_TRAITS_HPP_INCLUDED
#include <string>
#include <stdexcept>

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

namespace _Wolframe {
namespace langbind {
namespace traits {

///\brief Operators and Methods checked in this module
template<typename TYPE>
struct Interface
{
	TYPE operator + ( const TYPE&) const;
	TYPE operator - ( const TYPE&) const;
	TYPE operator / ( const TYPE&) const;
	TYPE operator * ( const TYPE&) const;
	TYPE pow( unsigned int) const;
	TYPE operator - () const;
	unsigned int len() const;
	std::string tostring() const;

	bool set( const char* key, const std::string&);
	bool set( const char* key, const char*);
	bool set( const std::string&);
	bool set( const char*);
	bool get( const char* key, std::string&) const;

	bool operator <( const TYPE&) const;
	bool operator <=( const TYPE&) const;
	bool operator ==( const TYPE&) const;
};

template<typename T>
struct has_method_get_key
{
	typedef char small_type;
	struct large_type {small_type dummy[2];};

	template<bool (T::*)( const char* key, std::string& val) const> struct tester_member_signature;

	template<typename U>
	static small_type has_matching_member(tester_member_signature<&U::get >*);
	template<typename U>
	static large_type has_matching_member(...);

	static const bool value=sizeof(has_matching_member<T>(0))==sizeof(small_type);
};

template<typename T>
struct has_method_set_key
{
	typedef char small_type;
	struct large_type {small_type dummy[2];};

	template<bool (T::*)( const char* key, const std::string& val) const> struct tester_member_signature;
	template<bool (T::*)( const char* key, const char* val) const> struct tester_charp_member_signature;

	template<typename U>
	static small_type has_matching_member(tester_member_signature<&U::set >*);
	template<typename U>
	static small_type has_matching_member(tester_charp_member_signature<&U::set >*);
	template<typename U>
	static large_type has_matching_member(...);

	static const bool value=sizeof(has_matching_member<T>(0))==sizeof(small_type);
};

template<typename T>
struct has_method_get
{
	typedef char small_type;
	struct large_type {small_type dummy[2];};

	template<bool (T::*)( std::string& val) const> struct tester_member_signature;

	template<typename U>
	static small_type has_matching_member(tester_member_signature<&U::get >*);
	template<typename U>
	static large_type has_matching_member(...);

	static const bool value=sizeof(has_matching_member<T>(0))==sizeof(small_type);
};

template<typename T>
struct has_method_set
{
	typedef char small_type;
	struct large_type {small_type dummy[2];};

	template<bool (T::*)( const std::string& val) const> struct tester_member_signature;
	template<bool (T::*)( const char* val) const> struct tester_charp_member_signature;

	template<typename U>
	static small_type has_matching_member(tester_member_signature<&U::set >*);
	template<typename U>
	static small_type has_matching_member(tester_charp_member_signature<&U::set >*);
	template<typename U>
	static large_type has_matching_member(...);

	static const bool value=sizeof(has_matching_member<T>(0))==sizeof(small_type);
};

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
struct has_operator_sub
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

template <class ObjectType>
struct LuaGetValue
{
	static int exec( lua_State* ls)
	{
		ObjectType* self = ObjectType::get( ls, 1);
		const char* key = lua_tostring( ls, 2);
		if (!key) luaL_error( ls, "__index called with an invalid argument 2 (key)");

		bool rt = false;
		bool overfl = false;
		char valbuf[ 256];
		std::size_t nn = 0;
		try
		{
			std::string val;
			rt = self->getValue( key, val);
			nn = (overfl=(val.size() >= sizeof( valbuf)))?(sizeof(valbuf)-1):val.size();
			std::memcpy( valbuf, val.c_str(), nn);
			valbuf[ nn] = 0;
		}
		catch (const std::bad_alloc&)
		{
			return luaL_error( ls, "out of memory calling __index");
		}
		catch (const std::exception& e)
		{
			return luaL_error( ls, "__index called with illegal value (%s)", e.what());
		}
		if (rt)
		{
			if (overfl)
			{
				return luaL_error( ls, "__index variable size exceeds maximum size (%u)", sizeof(valbuf));
			}
			lua_pushlstring( ls, valbuf, nn);
			return 1;
		}
		else
		{
			return luaL_error( ls, "__index called with unknown member name");
		}
		return 0;
	}
};

template <class ObjectType>
struct LuaSetValue
{
	static int exec( lua_State* ls)
	{
		ObjectType* self = ObjectType::get( ls, 1);
		const char* key = lua_tostring( ls, 2);
		if (!key) luaL_error( ls, "__newindex called with an invalid argument 2 (key)");
		const char* val = 0;
		int tp = lua_type( ls, 3);

		if (tp == LUA_TBOOLEAN)
		{
			val = lua_toboolean( ls, 3)?"true":"false";
		}
		else
		{
			val = lua_tostring( ls, 3);
		}
		if (!val) luaL_error( ls, "__newindex called with invalid argument 3 (value)");

		try
		{
			if (!self->setValue( key, val))
			{
				luaL_error( ls, "__newindex called with unknown variable name");
			}
		}
		catch (const std::bad_alloc&)
		{
			return luaL_error( ls, "out of memory calling __newindex");
		}
		catch (const std::exception& e)
		{
			return luaL_error( ls, "__newindex called with illegal value (%s)", e.what());
		}
		return 0;
	}
};

template <class ObjectType>
struct LuaAssignValue
{
	static int exec( lua_State* ls)
	{
		ObjectType* self = ObjectType::get( ls, 1);
		char* operand = lua_tostring( ls, 2);
		if (operand)
		{
			luaL_error( ls, "assigned value is not convertable to string");
		}
		try
		{
			if (!self->set( operand))
			{
				luaL_error( ls, "value cannot be assigned (conversion error): '%s'", operand);
			}
		}
		catch (const std::bad_alloc&)
		{
			luaL_error( ls, "memory allocation error in lua context");
		}
		catch (const std::exception&)
		{
			luaL_error( ls, "calculation exception in lua context");
		}
		return 0;
	}
};

template <class ObjectType, ObjectType ObjectType::*method( const ObjectType& operand)>
struct LuaBinOperator
{
	static int exec( lua_State* ls)
	{
		ObjectType* self = ObjectType::get( ls, 1);
		ObjectType* operand = ObjectType::get( ls, 2);
		try
		{
			ObjectType result = (self->*method)( *operand);
			ObjectType::push_luastack( ls, result);
		}
		catch (const std::bad_alloc&)
		{
			luaL_error( ls, "memory allocation error in lua context");
		}
		catch (const std::exception&)
		{
			luaL_error( ls, "calculation exception in lua context");
		}
		return 1;
	}
};

template <class ObjectType, typename OperandType, ObjectType ObjectType::*method( OperandType operand)>
struct LuaBinOperatorOp
{
	static void getOperand( lua_State* ls, int index, unsigned int& x)
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

	static int exec( lua_State* ls)
	{
		ObjectType* self = ObjectType::get( ls, 1);
		OperandType operand;
		getOperand( ls, 2, operand);
		try
		{
			ObjectType result = (self->*method)( operand);
			ObjectType::push_luastack( ls, result);
		}
		catch (const std::bad_alloc&)
		{
			luaL_error( ls, "memory allocation error in lua context");
		}
		catch (const std::exception&)
		{
			luaL_error( ls, "calculation exception in lua context");
		}
		return 1;
	}
};

template <class ObjectType, typename ResultType, ResultType ObjectType::*method()>
struct LuaPrefixOperator
{
	static void push_luastack( lua_State* ls, const std::string& p)
	{
		lua_pushlstring( ls, p.c_str(), p.size());
	}

	static void push_luastack( lua_State* ls, std::size_t p)
	{
		lua_pushnumber( ls, p);
	}

	static void push_luastack( lua_State* ls, const ObjectType& p)
	{
		push_luastack( ls, p);
	}

	static int exec( lua_State* ls)
	{
		ObjectType* self = ObjectType::get( ls, 1);
		try
		{
			ResultType result = (self->*method)();
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
		return 1;
	}
};

template <class ObjectType, bool ObjectType::*method( const ObjectType& operand)>
struct LuaCmpOperator
{
	static int exec( lua_State* ls)
	{
		ObjectType* self = ObjectType::get( ls, 1);
		ObjectType* operand = ObjectType::get( ls, 2);
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
		return 1;
	}
};


template <typename T>
typename boost::enable_if_c<
	has_operator_eq<T>::value
	,void>::type defineOperator_eq( lua_State* ls)
{
	lua_pushliteral( ls, "__eq");
	bool (T::*cmp)( const T&) = &T::operator==;
	int (*fun)( lua_State*) = &LuaCmpOperator< T, cmp>::exec;
	lua_pushcfunction( ls, fun);
	lua_rawset( ls, -3);
}
template <typename T>
typename boost::enable_if_c<
	! has_operator_eq<T>::value
	,void>::type defineOperator_eq( lua_State*)
{}

template <typename T>
typename boost::enable_if_c<
	has_operator_le<T>::value
	,void>::type defineOperator_le( lua_State* ls)
{
	lua_pushliteral( ls, "__le");
	bool (T::*cmp)( const T&) = &T::operator<=;
	int (*fun)( lua_State*) = &LuaCmpOperator< T, cmp >::exec;
	lua_pushcfunction( ls, fun);
	lua_rawset( ls, -3);
}
template <typename T>
typename boost::enable_if_c<
	! has_operator_le<T>::value
	,void>::type defineOperator_le( lua_State*)
{}

template <typename T>
typename boost::enable_if_c<
	has_operator_lt<T>::value
	,void>::type defineOperator_lt( lua_State* ls)
{
	lua_pushliteral( ls, "__lt");
	bool (T::*cmp)( const T&) = &T::operator<;
	int (*fun)( lua_State*) = &LuaCmpOperator< T, cmp >::exec;
	lua_pushcfunction( ls, fun);
	lua_rawset( ls, -3);
}
template <typename T>
typename boost::enable_if_c<
	! has_operator_lt<T>::value
	,void>::type defineOperator_lt( lua_State*)
{}

template <typename T>
typename boost::enable_if_c<
	has_operator_tostring<T>::value
	,void>::type defineOperator_tostring( lua_State* ls)
{
	lua_pushliteral( ls, "__tostring");
	std::string (T::*call)() = &T::tostring;
	int (*fun)( lua_State*) = LuaPrefixOperator< T, std::string, call >::exec;
	lua_pushcfunction( ls, fun );
	lua_rawset( ls, -3);
}
template <typename T>
typename boost::enable_if_c<
	! has_operator_tostring<T>::value
	,void>::type defineOperator_tostring( lua_State*)
{}

template <typename T>
typename boost::enable_if_c<
	has_operator_len<T>::value
	,void>::type defineOperator_len( lua_State* ls)
{
	lua_pushliteral( ls, "__len");
	std::size_t (T::*call)() = &T::len;
	int (*fun)( lua_State*) = LuaPrefixOperator< T, std::size_t, call >::exec;
	lua_pushcfunction( ls, fun);
	lua_rawset( ls, -3);
}
template <typename T>
typename boost::enable_if_c<
	! has_operator_len<T>::value
	,void>::type defineOperator_len( lua_State*)
{}

template <typename T>
typename boost::enable_if_c<
	has_operator_neg<T>::value
	,void>::type defineOperator_neg( lua_State* ls)
{
	lua_pushliteral( ls, "__unm");
	T (T::*call)() = &T::operator-;
	int (*fun)( lua_State*) = &LuaPrefixOperator<T, T, call >::exec;
	lua_pushcfunction( ls, fun);
	lua_rawset( ls, -3);
}
template <typename T>
typename boost::enable_if_c<
	! has_operator_neg<T>::value
	,void>::type defineOperator_neg( lua_State*)
{}

template <typename T>
typename boost::enable_if_c<
	has_operator_add<T>::value
	,void>::type defineOperator_add( lua_State* ls)
{
	lua_pushliteral( ls, "__add");
	T (T::*call)( const T&) = &T::operator+;
	int (*fun)( lua_State*) = &LuaBinOperator<T, call>::exec;
	lua_pushcfunction( ls, fun);
	lua_rawset( ls, -3);
}
template <typename T>
typename boost::enable_if_c<
	! has_operator_add<T>::value
	,void>::type defineOperator_add( lua_State*)
{}

template <typename T>
typename boost::enable_if_c<
	has_operator_sub<T>::value
	,void>::type defineOperator_sub( lua_State* ls)
{
	lua_pushliteral( ls, "__sub");
	T (T::*call)( const T&) = &T::operator-;
	int (*fun)( lua_State*) = &LuaBinOperator<T, call>::exec;
	lua_pushcfunction( ls, fun);
	lua_rawset( ls, -3);
}
template <typename T>
typename boost::enable_if_c<
	! has_operator_sub<T>::value
	,void>::type defineOperator_sub( lua_State*)
{}

template <typename T>
typename boost::enable_if_c<
	has_operator_div<T>::value
	,void>::type defineOperator_div( lua_State* ls)
{
	lua_pushliteral( ls, "__div");
	T (T::*call)( const T&) = &T::operator/;
	int (*fun)( lua_State*) = &LuaBinOperator<T, call>::exec;
	lua_pushcfunction( ls, fun);
	lua_rawset( ls, -3);
}
template <typename T>
typename boost::enable_if_c<
	! has_operator_div<T>::value
	,void>::type defineOperator_div( lua_State*)
{}

template <typename T>
typename boost::enable_if_c<
	has_operator_mul<T>::value
	,void>::type defineOperator_mul( lua_State* ls)
{
	lua_pushliteral( ls, "__mul");
	T (T::*call)( const T&) = &T::operator*;
	int (*fun)( lua_State*) = &LuaBinOperator<T, call>::exec;
	lua_pushcfunction( ls, fun);
	lua_rawset( ls, -3);
}
template <typename T>
typename boost::enable_if_c<
	! has_operator_mul<T>::value
	,void>::type defineOperator_mul( lua_State*)
{}

template <typename T>
typename boost::enable_if_c<
	has_operator_pow<T>::value
	,void>::type defineOperator_pow( lua_State* ls)
{
	lua_pushliteral( ls, "__pow");
	T (T::*call)( unsigned int) = &T::pow;
	int (*fun)( lua_State*) = &LuaBinOperatorOp<T, unsigned int, call>::exec;
	lua_pushcfunction( ls, fun);
	lua_rawset( ls, -3);
}
template <typename T>
typename boost::enable_if_c<
	! has_operator_pow<T>::value
	,void>::type defineOperator_pow( lua_State*)
{}


template <typename T>
typename boost::enable_if_c<
	has_method_get_key<T>::value
	,void>::type defineMethod_index( lua_State* ls)
{
	lua_pushliteral( ls, "__index");
	int (*fun)( lua_State*) = &LuaGetValue<T>::exec;
	lua_pushcfunction( ls, fun);
	lua_rawset( ls, -3);
}
template <typename T>
typename boost::enable_if_c<
	! has_method_get_key<T>::value
	,void>::type defineMethod_index( lua_State* ls)
{
	lua_pushliteral( ls, "__index");
	lua_pushvalue( ls, -2);
	lua_rawset( ls, -3);
}

template <typename T>
typename boost::enable_if_c<
	has_method_set_key<T>::value
	,void>::type defineMethod_newindex( lua_State* ls)
{
	lua_pushliteral( ls, "__newindex");
	int (*fun)( lua_State*) = &LuaSetValue<T>::exec;
	lua_pushcfunction( ls, fun);
	lua_rawset( ls, -3);
}
template <typename T>
typename boost::enable_if_c<
	! has_method_set_key<T>::value
	,void>::type defineMethod_newindex( lua_State* ls)
{
	lua_pushliteral( ls, "__newindex");
	lua_pushvalue( ls, -2);
	lua_rawset( ls, -3);
}

template <typename T>
typename boost::enable_if_c<
	has_method_get<T>::value
	,void>::type defineMethod_get( lua_State* ls)
{
	lua_pushliteral( ls, "get");
	std::string (T::*call)() = &T::tostring;
	int (*fun)( lua_State*) = LuaPrefixOperator< T, std::string, call >::exec;
	lua_pushcfunction( ls, fun );
	lua_rawset( ls, -3);
}
template <typename T>
typename boost::enable_if_c<
	! has_method_get<T>::value
	,void>::type defineMethod_get( lua_State*)
{}

template <typename T>
typename boost::enable_if_c<
	has_method_set<T>::value
	,void>::type defineMethod_set( lua_State* ls)
{
	lua_pushliteral( ls, "set");
	int (*fun)( lua_State*) = &LuaAssignValue<T>::exec;
	lua_pushcfunction( ls, fun);
	lua_rawset( ls, -3);
}
template <typename T>
typename boost::enable_if_c<
	! has_method_set<T>::value
	,void>::type defineMethod_set( lua_State*)
{}

}}} //namespace
#endif

