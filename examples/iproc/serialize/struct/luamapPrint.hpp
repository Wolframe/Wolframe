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
///\file serialize/struct/luamapPrint.hpp
///\brief Defines the intrusive implementation of the printing part of serialization for the lua map

#ifndef _Wolframe_SERIALIZE_STRUCT_LUAMAP_PRINT_HPP_INCLUDED
#define _Wolframe_SERIALIZE_STRUCT_LUAMAP_PRINT_HPP_INCLUDED
#include "serialize/struct/mapContext.hpp"
#include "serialize/struct/luamapTraits.hpp"
#include <boost/utility/value_init.hpp> 
#include <vector>

namespace _Wolframe {
namespace serialize {

template <typename T>
struct IntrusivePrinter;

template <typename T>
bool pushAtom_( const void* obj, const luanumeric_&, lua_State* ls, Context*)
{
	lua_pushnumber( ls, *((T*)obj));
	return true;
}

template <typename T>
bool pushAtom_( const void* obj, const luabool_&, lua_State* ls, Context*)
{
	lua_pushboolean( ls, *((T*)obj));
	return true;
}

template <typename T>
bool pushAtom_( const void* obj, const luastring_&, lua_State* ls, Context*)
{
	lua_pushstring( ls, ((T*)obj)->c_str());
	return true;
}

template <typename T>
bool pushAtom_( const void* obj, const luastruct_&, lua_State* ls, Context* ctx)
{
	ctx->setError( 0, "atomic value expected");
	return false;
}

template <typename T>
bool push_( const void* obj, lua_State* ls, Context* ctx)
{
	return pushAtom_<T>( obj, getLuaCategory(T()), ls, ctx);
}


template <typename T>
bool printObject_( const void* obj, const struct_&, lua_State* ls, Context* ctx)
{
	static const DescriptionBase* descr = T::getDescription();
	
	lua_newtable( ls);
	DescriptionBase::Map::const_iterator itr = descr->begin();
	while (itr != descr->end())
	{
		lua_pushstring( ls, itr->first);
		if (!itr->second.print()( (char*)obj+itr->second.ofs(), ls, ctx))
		{
			ctx->setError( itr->first);
			return false;
		}
		lua_settable( ls, -3);
		++itr;
	}
	return true;
}

template <typename T>
bool printObject_( const void* obj, const arithmetic_&, lua_State* ls, Context* ctx)
{
	return push_<T>( obj, ls, ctx);
}

template <typename T>
bool printObject_( const void* obj, const vector_&, lua_State* ls, Context* ctx)
{
	lua_newtable( ls);
	std::size_t index = 0;

	typename T::const_iterator itr = ((T*)obj)->begin();
	while (itr != ((T*)obj)->end())
	{
		lua_pushnumber( ls, (lua_Number)(++index));
		if (!IntrusivePrinter<typename T::value_type>::print( (const void*)&(*itr), ls, ctx))
		{
			return false;
		}
		lua_settable( ls, -3);
		++itr;
	}
	return true;
}

template <typename T>
struct IntrusivePrinter
{
	static bool print( const void* obj, lua_State* ls, Context* ctx)
	{
		return printObject_<T>( obj, getCategory(*(T*)obj), ls, ctx);
	}
};


}}//namespace
#endif

