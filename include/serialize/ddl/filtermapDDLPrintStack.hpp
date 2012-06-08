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
///\file serialize/ddl/filtermapDDLPrintStack.hpp
///\brief Defines the Parsing STM for DDL serialization for filters

#ifndef _Wolframe_SERIALIZE_DDL_FILTERMAP_PRINT_STACK_HPP_INCLUDED
#define _Wolframe_SERIALIZE_DDL_FILTERMAP_PRINT_STACK_HPP_INCLUDED
#include "filter/typedfilter.hpp"
#include "serialize/mapContext.hpp"
#include "ddl/structType.hpp"
#include <vector>
#include <cstddef>
#include <stdexcept>

namespace _Wolframe {
namespace serialize {

class FiltermapDDLPrintState
{
public:
	FiltermapDDLPrintState( const FiltermapDDLPrintState& o)
		:m_value(o.m_value)
		,m_stateidx(o.m_stateidx)
		,m_tag(o.m_tag)
		{}

	FiltermapDDLPrintState( const ddl::StructType* v, const langbind::TypedFilterBase::Element& t)
		:m_value(v)
		,m_stateidx(0)
		,m_tag(t)
		{}

	const ddl::StructType* value() const
	{
		return m_value;
	}

	std::size_t state() const
	{
		return m_stateidx;
	}

	void state( std::size_t idx)
	{
		m_stateidx = idx;
	}

	const langbind::TypedFilterBase::Element& tag() const
	{
		return m_tag;
	}

private:
	const ddl::StructType* m_value;
	std::size_t m_stateidx;
	langbind::TypedFilterBase::Element m_tag;
};

typedef std::vector<FiltermapDDLPrintState> FiltermapDDLPrintStateStack;

}}//namespace
#endif
