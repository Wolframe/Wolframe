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
///\file ddl_atomicType.cpp
///\brief implementation of the form DDL atomic type
#include "ddl/atomicType.hpp"

using namespace _Wolframe;
using namespace ddl;

bool AtomicType::getType( const char* name, Type& tp)
{
	const char* rt;
	unsigned int ii;
	for (ii=0,rt=typeName((Type)(ii)); rt!=0; ii++,rt=typeName((Type)(ii)))
	{
		if (std::strcmp( rt, name) == 0)
		{
			tp = (Type)ii;
			return true;
		}
	}
	return false;
}

void AtomicType::print( std::ostream& out, size_t indent) const
{
	while (indent--) out << "\t";
	out << typeName( m_type) << " '" << m_value << "'" << std::endl;
}

void AtomicType::init()
{
	switch (m_type)
	{
		case double_:
		case float_:
		case bigint_:
		case int_:
		case uint_:
		case short_:
		case ushort_:
		case char_:	m_value = "0"; ;
		case string_:	m_value.clear();
	}
}

