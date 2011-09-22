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
///\file serialize/luamapPrint.hpp
///\brief Defines the intrusive implementation of the printing part of serialization for the lua map

#ifndef _Wolframe_LUAMAP_PRINT_HPP_INCLUDED
#define _Wolframe_LUAMAP_PRINT_HPP_INCLUDED
#include "luamapTraits.hpp"

#include <boost/utility/value_init.hpp> 
#include <vector> 

namespace {

struct luanumeric_{};
struct luabool_{};
struct luastring_{};
struct luastruct_{};

///\brief get category luanumeric_ for a type
/// returns luanumeric_ if T fulfills the is_arithmetic condition or is a string
template <typename T>
typename boost::enable_if_c<
(boost::is_arithmetic<T>::value && !boost::is_same<bool,T>::value)
,luanumeric_>::type getLuaCategory( const T&) { return luanumeric_();}

///\brief get category luabool_ for a type
/// returns luabool_ if T is a bool
template <typename T>
typename boost::enable_if_c<
boost::is_same<bool,T>::value
,luabool_>::type getLuaCategory( const T&) { return luabool_();}

///\brief get category luastring_ for a type
/// returns luastring_ if T is a bool
template <typename T>
typename boost::enable_if_c<
boost::is_same<std::string,T>::value
,luastring_>::type getLuaCategory( const T&) { return luastring_();}

template <typename T>
typename boost::enable_if_c<
(!boost::is_arithmetic<T>::value && !boost::is_same<bool,T>::value && !boost::is_same<std::string,T>::value)
,luastruct_>::type getLuaCategory( const T&) { return luastruct_();}


struct Context
{
  char errormessage[256];
	char tag[256];
	
	Context()
	{
		errormessage[ 0] = 0;
		tag[ 0] = 0;
	}

	void printError( const char* tt, const char* ee)
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

	void printError( const char* tt, Context* ctx)
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
bool pushAtom_( void* obj, luanumeric_&, lua_State* ls, Context* ctx)
{
	lua_pushnumber( ls, *((T*)obj));
	return true;
}

template <typename T>
bool pushAtom_( void* obj, luabool_&, lua_State* ls, Context* ctx)
{
	lua_pushboolean( ls, *((T*)obj));
	return true;
}

template <typename T>
bool pushAtom_( void* obj, luastring_&, lua_State* ls, Context* ctx)
{
	lua_pushstring( ls, ((T*)obj)->c_str());
	return true;
}

template <typename T>
bool pushAtom_( void* obj, luastruct_&, lua_State* ls, Context* ctx)
{
	printError( 0, "atomic value expected");
	return false;
}

template <typename T>
bool push_( void* obj, lua_State* ls, Context* ctx)
{
	return pushAtom_( obj, getLuaCategory(T()), ls, ctx);
}


template <typename T>
bool print_( void* obj, struct_&, lua_State* ls, Context* ctx)
{
	static const DescriptionBase* descr = T::description();
	
	lua_newtable( ls);
	std::map<std::string,DescriptionBase>::const_iterator itr = descr->m_elem->begin();
	while (itr != descr->m_elem->end()(
	{
		lua_pushstring( ls, itr->first);
		if (!itr->m_print( 0, (char*)obj+itr->m_ofs, ls, ctx))
		{
			printError( itr->first, ctx);
			return false;
		}
		lua_settable( ls, -2);
		++itr;
	}
	return true;
}

template <typename T>
bool print_( void* obj, arithmetic_&, lua_State* ls, Context* ctx)
{
	return push_<T>( obj, ls, ctx);
}

template <typename T>
bool print_( void* obj, vector_&, lua_State* ls, Context* ctx)
{
	static const DescriptionBase* descr = T::description();
	
	lua_newtable( ls);
	std::size_t index = 0;

	T::const_iterator itr = ((T*)obj)->begin();
	while (itr != ((T*)obj)->end()(
	{
		lua_pushnumber( ls, ++index);
		if (!itr->m_print( 0, &(*itr), ls, ctx))
		{
			printError( 0, ctx);
			return false;
		}
		lua_settable( ls, -2);
		++itr;
	}
	return true;
}

}//anonymous namespace
namespace _Wolframe {
namespace serialize {

template <typename T>
static bool print( T* obj, lua_State* ls)
{
	Context ctx;
	if (!print_<T>( (void*)obj, getCategory(val), ls, &ctx))
	{
		luaL_error( "error in table deserialization at '%s': %s", ctx.tag, ctx.errormessage);
		return false;
	}
	return true;
}

}}//namespace
#endif

