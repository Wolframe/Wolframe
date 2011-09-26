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
///\file serialize/luamapParse.hpp
///\brief Defines the intrusive implementation of the parsing part of serialization for the lua map
#ifndef _Wolframe_LUAMAP_PARSE_HPP_INCLUDED
#define _Wolframe_LUAMAP_PARSE_HPP_INCLUDED
#include "serialize/luamapBase.hpp"
#include <stdexcept>
#include <boost/utility/value_init.hpp> 

///the intrusive part of the definitions is put into an anonymous namespace:
namespace {

struct Context
{
  char errormessage[256];
	char tag[256];
	
	Context()
	{
		errormessage[ 0] = 0;
		tag[ 0] = 0;
	}

	void parseError( const char* tt, const char* ee)
	{
		std::size_t nn = strlen(ee);
		if (nn >= sizeof( errormessage)) nn = sizeof( errormessage)-1;
		std::memcpy( errormessage, ee, nn);
		errormessage[ nn] = 0;
		if (tt)
		{
			nn = strlen(tt);
			if (nn >= sizeof( tag)) nn = sizeof( tag)-1;
			std::memcpy( tag, tt, nn);
			tag[ nn] = 0;
		}
		else
		{
			tag[ 0] = 0;
		}
	}

	void parseError( const char* tt, Context* ctx)
	{
		if (!tt) return;
		char buf[ sizeof(tag)];
		std::size_t nn = std::strlen(tt);
		if (tt >= sizeof( buf)) nn = sizeof( buf)-2;
		std::memcpy( buf, tt, nn);
		buf[ nn] = '/';
		buf[ ++nn] = 0;
		std::size_t mm = std::strlen(tag);
		if (mm+nn >= sizeof( buf)) mm = sizeof( buf)-1-nn;
		std::memcpy( buf+nn, tag, mm);
		buf[ nn+mm] = 0;
		std::memcpy( tag, buf, nn+mm+1);
	}
}

template <typename T>
bool parse_( void* obj, const struct_&, lua_State* ls, Context* ctx)
{
	static const DescriptionBase* descr = T::description();
	
	if (!lua_istable( ls, -1))
	{
		parseError( 0, "table expected for structure");
		return false;
	}
	lua_pushnil( ls);
	while (lua_next( ls, -2))
	{
		lua_pushvalue( ls, -2);
		DescriptionBase::Map::const_iterator itr = descr->find( lua_tostring( ls, -1));
		lua_pop( ls, 1);
		if (itr != descr->m_elem->end()
		{
			if (!itr->m_parse( 0, (char*)obj+itr->m_ofs, ls, ctx))
			{
				parseError( itr->first, ctx);
				return false;
			}
		}
		else
		{
			parseError( itr->first, "element not defined");
			return false;
		}
		lua_pop( ls, 1);
	}
	lua_pop( ls, 1);
	return true;
}

template <typename T>
bool parse_( void* obj, const arithmetic_&, lua_State* ls, Context* ctx)
{
	bool rt = true;
	try
	{
		switch (lua_type( ls, -1))
		{
			case LUA_TNIL: *((T*)obj) = value_initialized<T>(); break;
			case LUA_TNUMBER: *((T*)obj) = boost::lexical_cast<T>(lua_tonumber(ls,-1)); break;
			case LUA_TBOOLEAN: *((T*)obj) = boost::lexical_cast<T>(lua_toboolean(ls,-1)); break;
			case LUA_TSTRING: *((T*)obj) = boost::lexical_cast<T>(lua_tostring(ls,-1)); break;
			case LUA_TTABLE: parseError( 0, "arithmetic value expected instead of table"); rt = false; break;
			case LUA_TFUNCTION: parseError( 0, "arithmetic value expected instead of function"); rt = false; break;
			case LUA_TUSERDATA: parseError( 0, "arithmetic value expected instead of userdata"); rt = false; break;
			case LUA_TTHREAD: parseError( 0, "arithmetic value expected instead of thread"); rt = false; break;
			case LUA_TLIGHTUSERDATA: parseError( 0, "arithmetic value expected instead of lightuserdata"); rt = false; break;
		}
	}
	catch (const std::exception& e)
	{
		parseError( 0, e.what());
		rt = false;
	}
	return rt;
}

template <typename T>
bool parse_( void* obj, const vector_&, lua_State* ls, Context* ctx)
{
	static const DescriptionBase* descr = T::description();
	
	if (!lua_istable( ls, -1))
	{
		parseError( 0, "table expected for vector");
		return false;
	}
	lua_pushnil( ls);
	while (lua_next( ls, -2))
	{
		DescriptionBase::Map::const_iterator itr = descr->m_elem->begin();
		while (itr != descr->m_elem->end()
		{
			T::value_type elem;
			if (!itr->m_parse( 0, (char*)&elem, ls, ctx))
			{
				parseError( itr->first, ctx);
				return false;
			}
			else try
			{
				((T*)obj)->push_back( val);
			}
			catch (std::exception& e)
			{
				parseError( itr->first, e.what());
				rt = false;
			}
		}
		lua_pop( ls, 1);
	}
	lua_pop( ls, 1);
	return true;
}

}//anonymous namespace

namespace _Wolframe {
namespace parse {
namespace dm {

template <typename T>
static bool parse( T* obj, lua_State* ls)
{
	Context ctx;
	if (!parse_<T>( (void*)obj, getCategory(val), ls, &ctx))
	{
		luaL_error( "error in table serialization at '%s': %s", ctx.tag, ctx.errormessage);
		return false;
	}
	return true;
}

}}}//namespace
#endif


