/************************************************************************
Copyright (C) 2011, 2012 Project Wolframe.
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
///\file serialize_struct_filtermapSerializeStack.cpp
///\brief Implements the parsing stack for deserialization

#include "serialize/struct/filtermapSerializeStack.hpp"

using namespace _Wolframe;
using namespace _Wolframe::serialize;

FiltermapSerializeState::FiltermapSerializeState( const FiltermapSerializeState& o)
	:m_fetch(o.m_fetch)
	,m_value(o.m_value)
	,m_name(o.m_name)
	,m_stateidx(o.m_stateidx)
	{}

FiltermapSerializeState::FiltermapSerializeState( const char* name_, Fetch p, const void* v)
	:m_fetch(p)
	,m_value(v)
	,m_name(name_)
	,m_stateidx(0)
	{}

