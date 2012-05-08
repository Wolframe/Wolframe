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
///\file serialize/struct/mapDescription.hpp
///\brief Union of all mapping descriptions
#ifndef _Wolframe_SERIALIZE_STRUCT_MAP_DESCRIPTION_HPP_INCLUDED
#define _Wolframe_SERIALIZE_STRUCT_MAP_DESCRIPTION_HPP_INCLUDED
#include "serialize/struct/filtermapDescription.hpp"
#if WITH_LUA
#include "serialize/struct/luamapDescription.hpp"
#endif
#include "serialize/struct/mapStructure.hpp"
#include <typeinfo>
#include <exception>

namespace _Wolframe {
namespace serialize {

///\class FiltermapDescription
///\brief Intrusive description of a filter/form map
///\tparam Structure structure that is represented by this description
template <class Structure>
class MapDescription  :public MapDescriptionBase
{
public:
	///\brief Constructor
	MapDescription()
	{
		m_filtermap.reset( new FiltermapDescription<Structure>());
#if WITH_LUA
		m_luamap.reset( new LuamapDescription<Structure>());
#endif
	}

	///\brief Operator to build the structure element by element
	///\tparam Element element type
	///\param[in] name name of the element
	///\param[in] eptr pointer to member of the element
	template <typename Element>
	MapDescription& operator()( const char* name, Element Structure::*eptr)
	{
		thisFiltermap()( name, eptr);
#if WITH_LUA
		thisLuamap()( name, eptr);
#endif
		return *this;
	}

	MapDescription& operator--(int)
	{
		thisFiltermap().defineEndOfAttributes();
#if WITH_LUA
		thisLuamap().defineEndOfAttributes();
#endif
		return *this;
	}
private:
	FiltermapDescription<Structure>& thisFiltermap()
	{
		return *static_cast< FiltermapDescription< Structure> * >(m_filtermap.get());
	}

#if WITH_LUA
	LuamapDescription<Structure>& thisLuamap()
	{
		return *static_cast< LuamapDescription<Structure> * >(m_luamap.get());
	}
#endif
};

}}// end namespace
#endif

