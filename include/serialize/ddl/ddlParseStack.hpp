/************************************************************************
Copyright (C) 2011 - 2014 Project Wolframe.
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
/// \file serialize/ddl/ddlParseStack.hpp
/// \brief Defines the Parsing STM for DDL serialization

#ifndef _Wolframe_SERIALIZE_DDL_PARSE_STACK_HPP_INCLUDED
#define _Wolframe_SERIALIZE_DDL_PARSE_STACK_HPP_INCLUDED
#include "filter/typedfilter.hpp"
#include "types/variantStruct.hpp"
#include "types/normalizeFunction.hpp"
#include <vector>
#include <stdexcept>

namespace _Wolframe {
namespace serialize {

/// \class DDLParseState
/// \brief State stack element for an initializer of a DDL structure from an iterator (serialization)
class DDLParseState
{
public:
	/// \brief Copy constructor
	DDLParseState( const DDLParseState& o)
		:m_size(o.m_size)
		,m_elemidx(o.m_elemidx)
		,m_value(o.m_value)
		,m_normalizer(o.m_normalizer)
		,m_name(o.m_name)
		,m_stateidx(o.m_stateidx)
		{}

	/// \brief Constructor
	DDLParseState( const char* name_, types::VariantStruct* v, const types::NormalizeFunction* n)
		:m_size(0)
		,m_elemidx(0)
		,m_value(v)
		,m_normalizer(n)
		,m_name(name_)
		,m_stateidx(0)
		{}

	/// \brief Destructor
	~DDLParseState(){}

	/// \brief Reference to the substructure visited in this state
	types::VariantStruct* value() const
	{
		return m_value;
	}

	/// \brief Reference name of the substructure visited in this state
	const char* name() const
	{
		return m_name;
	}

	/// \brief Set the internal state
	void state( std::size_t idx)
	{
		m_stateidx = idx;
	}

	/// \brief Get the internal state
	std::size_t state() const
	{
		return m_stateidx;
	}

	/// \brief Get the normalizer function of the value node of this state
	const types::NormalizeFunction* normalizer() const
	{
		return m_normalizer;
	}

private:
	std::size_t m_size;
	std::size_t m_elemidx;
	types::VariantStruct* m_value;
	const types::NormalizeFunction* m_normalizer;
	const char* m_name;
	std::size_t m_stateidx;
};

/// \brief State stack for an initializer of a DDL structure from an iterator (serialization)
typedef std::vector<DDLParseState> DDLParseStateStack;


}}//namespace
#endif

