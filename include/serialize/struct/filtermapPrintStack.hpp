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
///\file serialize/struct/filtermapPrintStack.hpp
///\brief Defines the Parsing STM for serialization for filters

#ifndef _Wolframe_SERIALIZE_STRUCT_FILTERMAP_PRINT_STACK_HPP_INCLUDED
#define _Wolframe_SERIALIZE_STRUCT_FILTERMAP_PRINT_STACK_HPP_INCLUDED
#include "filter/typedfilter.hpp"
#include "serialize/mapContext.hpp"
#include <vector>
#include <cstddef>
#include <stdexcept>

namespace _Wolframe {
namespace serialize {

class FiltermapPrintState
{
public:
	typedef bool (*Print)( langbind::TypedOutputFilter& inp, Context& ctx, std::vector<FiltermapPrintState>& stk);

public:
	FiltermapPrintState( const FiltermapPrintState& o)
		:m_print(o.m_print)
		,m_value(o.m_value)
		,m_stateidx(o.m_stateidx)
		{}

	FiltermapPrintState( Print p, const void* v)
		:m_print(p)
		,m_value(v)
		,m_stateidx(0)
		{}

	const void* value() const
	{
		return m_value;
	}

	Print print() const
	{
		return m_print;
	}

	std::size_t state() const
	{
		return m_stateidx;
	}

	void state( std::size_t idx)
	{
		m_stateidx = idx;
	}

private:
	Print m_print;
	const void* m_value;
	std::size_t m_stateidx;
};

typedef std::vector<FiltermapPrintState> FiltermapPrintStateStack;

}}//namespace
#endif
