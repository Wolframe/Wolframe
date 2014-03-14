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
///\file struct/parseStack.cpp
///\brief Implements the parsing stack for deserialization
#include "serialize/struct/parseStack.hpp"

using namespace _Wolframe;
using namespace _Wolframe::serialize;

ParseState::ParseState( const ParseState& o)
	:m_parse(o.m_parse)
	,m_initar(0)
	,m_size(o.m_size)
	,m_value(o.m_value)
	,m_name(o.m_name)
	,m_stateidx(o.m_stateidx)
{
	if (o.m_initar)
	{
		m_initar = (int*)std::calloc( m_size, sizeof(*m_initar));
		std::memcpy( m_initar, o.m_initar, m_size*sizeof(*m_initar));
	}
}

ParseState::ParseState( const char* name_, Parse p, void* v)
	:m_parse(p)
	,m_initar(0)
	,m_size(0)
	,m_value(v)
	,m_name(name_)
	,m_stateidx(0)
	{}

ParseState::~ParseState()
{
	if (m_initar) std::free( m_initar);
}

std::size_t ParseState::selectElement( std::size_t idx, std::size_t size)
{
	if (!m_initar) m_initar = (int*)std::calloc( m_size=size, sizeof(*m_initar));
	if (idx > m_size) throw std::logic_error( "ABW in intrusive filter map parser");
	return m_initar[ idx]++;
}

std::size_t ParseState::initCount( std::size_t idx) const
{
	if (!m_initar) return 0;
	if (idx > m_size) throw std::logic_error( "ABW in intrusive filter map parser");
	return m_initar[ idx];
}


