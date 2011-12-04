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
///\file serialize/struct/luamapParse.hpp
///\brief Defines the intrusive implementation of the parsing part of serialization for the lua map
#ifndef _Wolframe_SERIALIZE_STRUCT_LUAMAP_PARSE_HPP_INCLUDED
#define _Wolframe_SERIALIZE_STRUCT_LUAMAP_PARSE_HPP_INCLUDED
#include "serialize/struct/mapContext.hpp"
#include "serialize/struct/luamapBase.hpp"
#include "serialize/struct/luamapTraits.hpp"
#include <stdexcept>
#include <boost/utility/value_init.hpp>
#include <boost/lexical_cast.hpp>
#include <cmath>
#include <limits>

namespace _Wolframe {
namespace serialize {

template <typename T>
struct IntrusiveParser;

static void setLuaError( Context* ctx, lua_State* ls, int tagIndex, const char* msg, const char* param)
{
	lua_pushvalue( ls, tagIndex);
	const char* tablename = lua_tostring( ls, -1);
	ctx->setError( tablename, msg, param);
	lua_pop( ls, 1);
}

template <typename T>
static bool parseObject_( void* obj, const struct_&, lua_State* ls, Context* ctx)
{
	static const DescriptionBase* descr = T::getDescription();
	
	if (!lua_istable( ls, -1))
	{
		ctx->setError( 0, "table expected for structure");
		return false;
	}

	lua_pushnil( ls);
	while (lua_next( ls, -2))
	{
		lua_pushvalue( ls, -2);
		const char* key = lua_tostring( ls, -1);
		if (!key)
		{
			setLuaError( ctx, ls, -5, "string expected as key for struct in table instead of ", lua_typename( ls, lua_type( ls, -1)));
			return false;
		}
		DescriptionBase::Map::const_iterator itr = descr->find( key);

		if (itr == descr->end())
		{
			setLuaError( ctx, ls, -5, "element not defined ", key);
			return false;
		}
		lua_pop( ls, 1);

		if (!itr->second.parse()( (char*)obj+itr->second.ofs(), ls, ctx))
		{
			ctx->setError( itr->first);
			return false;
		}
		lua_pop( ls, 1);
	}
	return true;
}

template <typename T>
static bool parseObject_( void* obj, const string_&, lua_State* ls, Context* ctx)
{
	bool rt = true;
	try
	{
		switch (lua_type( ls, -1))
		{
			case LUA_TNIL: *((T*)obj) = boost::value_initialized<T>(); break;
			case LUA_TNUMBER: *((T*)obj) = boost::lexical_cast<T>( lua_tonumber(ls,-1)); break;
			case LUA_TBOOLEAN: *((T*)obj) = boost::lexical_cast<T>( (bool)lua_toboolean(ls,-1)); break;
			case LUA_TSTRING: *((T*)obj) = boost::lexical_cast<T>( lua_tostring(ls,-1)); break;
			case LUA_TTABLE: ctx->setError( 0, "arithmetic value expected instead of table"); rt = false; break;
			case LUA_TFUNCTION: ctx->setError( 0, "arithmetic value expected instead of function"); rt = false; break;
			case LUA_TUSERDATA: ctx->setError( 0, "arithmetic value expected instead of userdata"); rt = false; break;
			case LUA_TTHREAD: ctx->setError( 0, "arithmetic value expected instead of thread"); rt = false; break;
			case LUA_TLIGHTUSERDATA: ctx->setError( 0, "arithmetic value expected instead of lightuserdata"); rt = false; break;
		}
	}
	catch (const std::exception& e)
	{
		ctx->setError( 0, e.what());
		rt = false;
	}
	return rt;
}


template <typename T>
typename boost::enable_if_c<boost::is_same<bool,T>::value,T>::type convertNumber( double n)
{
	if (n > std::numeric_limits<T>::epsilon()) return false;
	if (n < -std::numeric_limits<T>::epsilon()) return false;
	return true;
}

template <typename T>
typename boost::enable_if_c<boost::is_floating_point<T>::value,T>::type convertNumber( double n)
{
	if (n > (double)std::numeric_limits<T>::max() || n < (double)std::numeric_limits<T>::min())
	{
		throw std::out_of_range( "arithmetic value");
	}
	return (T)n;
}

template <typename T>
typename boost::enable_if_c<boost::is_integral<T>::value,T>::type convertNumber( double n)
{
	if (n > (double)std::numeric_limits<T>::max() || n < (double)std::numeric_limits<T>::min())
	{
		throw std::out_of_range( "arithmetic value");
	}
	return (T)n;
}

template <typename T>
static bool parseObject_( void* obj, const arithmetic_&, lua_State* ls, Context* ctx)
{
	bool rt = true;
	try
	{
		switch (lua_type( ls, -1))
		{
			case LUA_TNIL: *((T*)obj) = boost::value_initialized<T>(); break;
			case LUA_TNUMBER: *((T*)obj) = convertNumber<T>( lua_tonumber( ls,-1)); break;
			case LUA_TBOOLEAN: *((T*)obj) = convertNumber<T>( lua_tonumber( ls,-1)); break;
			case LUA_TSTRING: *((T*)obj) = boost::lexical_cast<T>( lua_tostring( ls,-1)); break;
			case LUA_TTABLE: ctx->setError( 0, "arithmetic value expected instead of table"); rt = false; break;
			case LUA_TFUNCTION: ctx->setError( 0, "arithmetic value expected instead of function"); rt = false; break;
			case LUA_TUSERDATA: ctx->setError( 0, "arithmetic value expected instead of userdata"); rt = false; break;
			case LUA_TTHREAD: ctx->setError( 0, "arithmetic value expected instead of thread"); rt = false; break;
			case LUA_TLIGHTUSERDATA: ctx->setError( 0, "arithmetic value expected instead of lightuserdata"); rt = false; break;
		}
	}
	catch (const std::exception& e)
	{
		ctx->setError( 0, e.what());
		rt = false;
	}
	return rt;
}

template <typename T>
static bool parseVectorElement( void* vectorobj, lua_State* ls, Context* ctx)
{
	typename T::value_type val;
	if (!IntrusiveParser<typename T::value_type>::parse( &val, ls, ctx))
	{
		return false;
	}
	try
	{
		((T*)vectorobj)->push_back( val);
		return true;
	}
	catch (std::exception& e)
	{
		ctx->setError( 0, e.what());
		return false;
	}
}

template <typename T>
static bool parseObject_( void* obj, const vector_&, lua_State* ls, Context* ctx)
{
	std::size_t vecidx = 0;
	if (!lua_istable( ls, -1))
	{
		/// ... If the element parsed as a vector is not a table,
		/// ... then we assume it to be the single atomic
		/// ... element of the vector parsed. If this hypothesis
		/// ... works, everything is fine. If not we have to give
		/// ... up with an error.
		if (!parseVectorElement<T>( obj, ls, ctx))
		{
			ctx->setError( 0, "single vector element does not match to element type");
			return false;
		}
		return true;
	}
	lua_pushnil( ls);
	while (lua_next( ls, -2))
	{
		if (lua_type( ls, -2) != LUA_TNUMBER)
		{
			if (vecidx == 0)
			{
				/// ... If the table parsed as a vector
				/// ... has not a number index as first element,
				/// ... then we assume it to be the single structure
				/// ... element of the vector parsed. If this hypothesis
				/// ... works, everything is fine. If not we have to give
				/// ... up with an error.
				lua_pushvalue( ls, -3);
				if (!parseVectorElement<T>( obj, ls, ctx))
				{
					ctx->setError( 0, "single vector element does not match to element type");
					return false;
				}
				lua_pop( ls, 3);
				return true;
			}
			else
			{
				ctx->setError( 0, "only number indices expected for vector");
				return false;
			}
		}
		++vecidx;
		if (!parseVectorElement<T>( obj, ls, ctx)) return false;
		lua_pop( ls, 1);
	}
	return true;
}

template <typename T>
struct IntrusiveParser
{
	static bool parse( void* obj, lua_State* ls, Context* ctx)
	{
		return parseObject_<T>( obj, getCategory(*(T*)obj), ls, ctx);
	}
};
}}//namespace
#endif


