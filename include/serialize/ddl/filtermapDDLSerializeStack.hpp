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
///\file serialize/ddl/FiltermapDDLSerializeStack.hpp
///\brief Defines the Parsing STM for DDL serialization for filters

#ifndef _Wolframe_SERIALIZE_DDL_FILTERMAP_SERIALIZE_STACK_HPP_INCLUDED
#define _Wolframe_SERIALIZE_DDL_FILTERMAP_SERIALIZE_STACK_HPP_INCLUDED
#include "filter/typedfilter.hpp"
#include "serialize/mapContext.hpp"
#include "types/variant.hpp"
#include "types/variantStruct.hpp"
#include <vector>
#include <cstddef>
#include <stdexcept>

namespace _Wolframe {
namespace serialize {

class FiltermapDDLSerializeState
{
public:
	FiltermapDDLSerializeState( const FiltermapDDLSerializeState& o)
		:m_value(o.m_value)
		,m_stateidx(o.m_stateidx)
		,m_elemtype(o.m_elemtype)
		,m_tag(o.m_tag)
		{}

	FiltermapDDLSerializeState( const types::VariantStruct* v, const types::VariantConst& t)
		:m_value(v)
		,m_stateidx(0)
		,m_elemtype(langbind::FilterBase::Value)
		,m_tag(t)
		{
			m_tag.setInitialized();
		}

	FiltermapDDLSerializeState( const types::VariantStruct* v)
		:m_value(v)
		,m_stateidx(0)
		,m_elemtype(langbind::FilterBase::Value)
		{}

	FiltermapDDLSerializeState( langbind::FilterBase::ElementType typ, const types::VariantConst& elem)
		:m_value(0)
		,m_stateidx(0)
		,m_elemtype(typ)
		,m_tag(elem)
		{
			m_tag.setInitialized();
		}

	const types::VariantStruct* value() const
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

	langbind::FilterBase::ElementType type() const
	{
		return m_elemtype;
	}

	const types::VariantConst& tag() const
	{
		return m_tag;
	}

private:
	const types::VariantStruct* m_value;
	std::size_t m_stateidx;
	langbind::FilterBase::ElementType m_elemtype;
	types::VariantConst m_tag;
};

typedef std::vector<FiltermapDDLSerializeState> FiltermapDDLSerializeStateStack;

}}//namespace
#endif
