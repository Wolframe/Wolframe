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
///\file serialize/struct/luamapBase.hpp
///\brief Defines the non intrusive base class of serialization of structures for the lua map

#ifndef _Wolframe_SERIALIZE_STRUCT_LUAMAP_BASE_HPP_INCLUDED
#define _Wolframe_SERIALIZE_STRUCT_LUAMAP_BASE_HPP_INCLUDED
#include "serialize/mapContext.hpp"
extern "C"
{
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
}
#include <cstddef>
#include <string>
#include <cstring>
#include <cstddef>
#include <vector>

namespace _Wolframe {
namespace serialize {

class LuamapDescriptionBase
{
public:
	typedef std::vector<std::pair<const char*,LuamapDescriptionBase> > Map;
	typedef bool (*Parse)( void* obj, lua_State* ls, Context* ctx);
	typedef bool (*Print)( const void* obj, lua_State* ls, Context* ctx);
	typedef bool (*Constructor)( void* obj);
	typedef void (*Destructor)( void* obj);

	Parse parse() const {return m_parse;}
	Print print() const {return m_print;}

	LuamapDescriptionBase( Constructor c, Destructor d, const char* tn, std::size_t os, std::size_t sz, Parse pa, Print pr)
		:m_constructor(c),m_destructor(d),m_typename(tn),m_ofs(os),m_size(sz),m_parse(pa),m_print(pr){}
	LuamapDescriptionBase( const char* tn, std::size_t os, std::size_t sz, Parse pa, Print pr)
		:m_constructor(0),m_destructor(0),m_typename(tn),m_ofs(os),m_size(sz),m_parse(pa),m_print(pr){}
	LuamapDescriptionBase( const LuamapDescriptionBase& o)
		:m_constructor(o.m_constructor),m_destructor(o.m_destructor),m_typename(o.m_typename),m_ofs(o.m_ofs),m_size(o.m_size),m_elem(o.m_elem),m_parse(o.m_parse),m_print(o.m_print){}

	bool parse( void* obj, lua_State* ls, Context* ctx) const;
	bool print( const void* obj, lua_State* ls, Context* ctx) const;

	bool init( void* obj) const
	{
		return (m_constructor)?m_constructor( obj):true;
	}

	void done( void* obj) const
	{
		if (m_destructor) m_destructor( obj);
	}

	std::size_t size() const
	{
		return m_size;
	}

	std::size_t ofs() const
	{
		return m_ofs;
	}

	Map::const_iterator find( const char* name) const
	{
		for (Map::const_iterator itr = m_elem.begin(); itr!=m_elem.end(); ++itr)
		{
			if (std::strcmp( itr->first, name) == 0) return itr;
		}
		return m_elem.end();
	}

	Map::const_iterator begin() const {return m_elem.begin();}
	Map::const_iterator end() const {return m_elem.end();}

	void define( const char* name, const LuamapDescriptionBase& dd)
	{
		m_elem.push_back( std::pair<const char*,LuamapDescriptionBase>(name,dd));
	}
private:
	Constructor m_constructor;
	Destructor m_destructor;
	const char* m_typename;
	std::size_t m_ofs;
	std::size_t m_size;
	Map m_elem;
	Parse m_parse;
	Print m_print;
};

}}//namespace
#endif
