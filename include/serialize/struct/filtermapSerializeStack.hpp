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
///\file serialize/struct/filtermapSerializeStack.hpp
///\brief Defines the parsing stack for serialization of objects

#ifndef _Wolframe_SERIALIZE_STRUCT_FILTERMAP_SERIALIZE_STACK_HPP_INCLUDED
#define _Wolframe_SERIALIZE_STRUCT_FILTERMAP_SERIALIZE_STACK_HPP_INCLUDED
#include "filter/typedfilter.hpp"
#include "serialize/mapContext.hpp"
#include <vector>
#include <cstddef>
#include <stdexcept>

namespace _Wolframe {
namespace serialize {

//\class FiltermapSerializeState
//\brief State stack element for an iterator on a structure (serializer)
class FiltermapSerializeState
{
public:
	typedef bool (*Fetch)( Context& ctx, std::vector<FiltermapSerializeState>& stk);

public:
	FiltermapSerializeState( const FiltermapSerializeState& o);

	FiltermapSerializeState( const char* name_, Fetch p, const void* v);

	const void* value() const			{return m_value;}
	const char* name() const			{return m_name;}
	Fetch fetch() const				{return m_fetch;}
	std::size_t state() const			{return m_stateidx;}
	void state( std::size_t idx)			{m_stateidx = idx;}

private:
	Fetch m_fetch;
	const void* m_value;
	const char* m_name;
	std::size_t m_stateidx;
};

//\brief State stack for an iterator on a structure (serializer)
typedef std::vector<FiltermapSerializeState> FiltermapSerializeStateStack;

}}//namespace
#endif
