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
///\file serialize/mapContext.cpp
///\brief Defines the error handling of serialization/deserialization functions and some other common stuff

#include "serialize/mapContext.hpp"
#include <cstring>
#include <stdexcept>

using namespace _Wolframe;
using namespace serialize;

Context::Context( Flags f)
	:m_flags(f)
	,m_has_elem(false)
{
	m_lasterror[0] = 0;
	m_tag[0] = 0;
}

Context::Context( const Context& o)
	:m_flags(o.m_flags)
	,m_has_elem(o.m_has_elem)
{
	std::memcpy( m_lasterror, o.m_lasterror, sizeof(m_lasterror));
	std::memcpy( m_tag, o.m_tag, sizeof(m_tag));
}

bool Context::getFlag( const char* name, Flags& flg)
{
	if (std::strcmp( name, "attributes") == 0)
	{
		flg = ValidateAttributes;
		return true;
	}
	return false;
}

void Context::clear()
{
	m_has_elem = false;
	m_lasterror[0] = 0;
	m_tag[0] = 0;
}

void Context::setTag( const char* tt)
{
	if (!tt)
	{
		m_tag[0] = 0;
	}
	else
	{
		std::size_t nn = std::strlen(tt);
		if (nn >=  sizeof(m_tag)) nn = sizeof(m_tag)-1;
		std::memcpy( m_tag, tt, nn);
		m_tag[nn] = 0;
	}
}

void Context::setError( const char* m1, const char* m2)
{
	try
	{
		std::string msg;
		if (m1) msg.append( m1);
		if (m2)
		{
			msg.append( " (");
			msg.append( m2);
			msg.push_back(')');
		}
		std::size_t nn = msg.size();
		if (nn >=  sizeof(m_lasterror)) nn = sizeof(m_lasterror)-1;
		std::memcpy( m_lasterror, msg.c_str(), nn);
		m_lasterror[ nn] = 0;
	}
	catch (const std::bad_alloc& e)
	{
		std::size_t nn = std::strlen( e.what());
		if (nn >=  sizeof(m_lasterror)) nn = sizeof(m_lasterror)-1;
		std::memcpy( m_lasterror, e.what(), nn);
		m_lasterror[ nn] = 0;
	}
}


