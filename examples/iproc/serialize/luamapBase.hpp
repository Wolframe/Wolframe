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
///\file serialize/luamapBase.hpp
///\brief Defines the non intrusive base class of serialization for the lua map

#ifndef _Wolframe_LUAMAP_BASE_HPP_INCLUDED
#define _Wolframe_LUAMAP_BASE_HPP_INCLUDED
extern "C"
{
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
}
#include <cstddef>
#include <string>
#include <map>

namespace _Wolframe {
namespace serialize {

class DescriptionBase
{
public:
	typedef bool (*Parse)( void* obj, lua_State* ls);
	typedef bool (*Print)( void* obj, lua_State* ls);
	
	DescriptionBase( const char* tn, std::size_t ofs, Parse pa, Print pr)
		:m_typename(tn),m_ofs(ofs),m_parse(pa),m_print(pr){}
	DescriptionBase( const DescriptionBase& o)
		:m_typename(o.m_typename),m_ofs(o.m_ofs),m_elem(o.m_elem),m_parse(o.m_parse),m_print(o.m_print){}

	bool parse( void* obj, lua_State* ls) const	{return m_parse(obj,ls);}
	bool print( void* obj, lua_State* ls) const	{return m_print(obj,ls);}
public:
	const char* m_typename;
	std::size_t m_ofs;
	typedef std::map<std::string,DescriptionBase> Map;
	Map m_elem;
	Parse m_parse;
	Print m_print;
};

}}//namespace
#endif
