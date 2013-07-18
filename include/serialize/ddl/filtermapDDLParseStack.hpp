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
///\file serialize/ddl/filtermapDDLParseStack.hpp
///\brief Defines the Parsing STM for DDL serialization for filters

#ifndef _Wolframe_SERIALIZE_DDL_FILTERMAP_PARSE_STACK_HPP_INCLUDED
#define _Wolframe_SERIALIZE_DDL_FILTERMAP_PARSE_STACK_HPP_INCLUDED
#include "filter/typedfilter.hpp"
#include "serialize/mapContext.hpp"
#include "types/variantStruct.hpp"
#include <vector>
#include <stdexcept>

namespace _Wolframe {
namespace serialize {

class FiltermapDDLParseState
{
public:
	FiltermapDDLParseState( const FiltermapDDLParseState& o)
		:m_size(o.m_size)
		,m_elemidx(o.m_elemidx)
		,m_value(o.m_value)
		,m_name(o.m_name)
		,m_stateidx(o.m_stateidx)
		{}

	FiltermapDDLParseState( const char* name_, types::VariantStruct* v)
		:m_size(0)
		,m_elemidx(0)
		,m_value(v)
		,m_name(name_)
		,m_stateidx(0)
		{}

	~FiltermapDDLParseState(){}

	types::VariantStruct* value() const
	{
		return m_value;
	}

	const char* name() const
	{
		return m_name;
	}

	void state( std::size_t idx)
	{
		m_stateidx = idx;
	}

	std::size_t state() const
	{
		return m_stateidx;
	}

private:
	std::size_t m_size;
	std::size_t m_elemidx;
	types::VariantStruct* m_value;
	const char* m_name;
	std::size_t m_stateidx;
};

typedef std::vector<FiltermapDDLParseState> FiltermapDDLParseStateStack;


}}//namespace
#endif

