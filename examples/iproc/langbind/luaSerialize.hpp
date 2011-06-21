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
///\file config/luaSerialize.hpp
///\brief Intrusive serialization/deserialization template definitions for the Lua application processor

#ifndef _Wolframe_LUA_APPLICATION_PROCESSOR_SERIALIZE_HPP_INCLUDED
#define _Wolframe_LUA_APPLICATION_PROCESSOR_SERIALIZE_HPP_INCLUDED
#include <boost/property_tree/ptree.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <string>
#include <vector>
#include <cstring>
#include "serialize/traits.hpp"

namespace _Wolframe {
namespace serialize {
namespace lua {

#include <boost/property_tree/ptree.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <string>
#include <vector>
#include <cstring>
#include "serialize/traits.hpp"
#include "serialize/descriptionBase.hpp"

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

struct AppProcessorObject
{
	lua_State* ls;
};

template <typename Element>
struct Serializer
{
	static void serialize( void* st, std::size_t ofs, AppProcessorObject* appobj)
	{
		serializeElement( name, *reinterpret_cast<Element*>((char*)st + ofs), appobj);
	}
};

struct Error
{
	std::string m_location;
	std::string m_message;

	Error( const std::string& location, const std::string& message)
		:m_location(location),m_message(message){}
	Error( const std::string& location, const Error& prev)
		:m_location(location),m_message(prev.m_message)
	{
		m_location.append( ".");
		m_location.append( prev.m_location);
	}
};

template <typename TYPE>
void castAtom( TYPE& dst, lua_State* ls, int index)
{
	const char* st;
	switch (lua_type (ls, index))
	{
		case LUA_TNUMBER:
			dst = boost::lexical_cast<TYPE>( lua_tonumber( ls, index));
			break;
		case LUA_TBOOLEAN:
			dst = boost::lexical_cast<TYPE>( lua_toboolean( ls, index));
			break;
		default:
			lua_pushvalue( ls, index);
			st = lua_tostring( ls, -1);
			if (st != 0)
			{
				dst = boost::lexical_cast<TYPE>(st);
				lua_pop( ls, 1);
				break;
			}
			lua_pop( ls, 1);
			throw boost::bad_lexical_cast( lua_typename(ls, index));
	}
}

template <typename T>
static void serializeElement_( T& value, const traits::struct_&, AppProcessorObject* appobj)
{
	static const DescriptionBase* descr = T::description();
	std::vector<DescriptionBase::Item>::const_iterator itr,end;

	if (lua_type (ls, -1) != LUA_TTABLE)
	{
		throw Error( "", "structure has to be defined as table");
	}
	for (itr=descr->m_ar.begin(),end=descr->m_ar.end(); itr != end; ++itr)
	{
		lua_getfield( ls, -1, itr->m_name.c_str());
		try
		{
			itr->m_serialize( &value, itr->m_ofs, appobj);
		}
		catch (const boost::bad_lexical_cast& e)
		{
			throw Error(  itr->m_name.c_str(), e.what());
		}
		catch (const Error& e)
		{
			throw Error(  itr->m_name.c_str(), e);
		}
		lua_pop( ls, 1);
	}
}

template <typename T>
static void serializeElement_( T& value, const traits::vector_&, AppProcessorObject* appobj)
{
	if (lua_type (ls, -1) != LUA_TTABLE)
	{
		throw boost::bad_lexical_cast( lua_typename( ls, -1));
	}
	lua_pushnil( ls);
	while (lua_next( ls, -2))
	{
		typename T::value_type elem;
		Serializer<T::value_type>::serialize( &elem, 0, appobj);
		value.push_back( elem);
		lua_pop( ls, 1);
	}
}

template <typename T>
static void serializeElement_( T& value, const traits::atom_&, AppProcessorObject* appobj)
{
	castAtom( value, appobj->ls, -1);
}

///\brief The unified serialize/deserialize function template
template <typename T>
static void serializeElement( T& value, AppProcessorObject* appobj)
{
	serializeElement_( value, traits::getCategory(value), appobj);
}

}}}// end namespace
#endif

