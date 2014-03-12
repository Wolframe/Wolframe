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
///\file pythonStructureBuilder.cpp
///\brief Implementation of python data structure building
#include "pythonStructureBuilder.hpp"
#include "logger-v1.hpp"
#include "pythonException.hpp"
extern "C" {
#include <Python.h>
}

using namespace _Wolframe;
using namespace _Wolframe::langbind;
using namespace _Wolframe::langbind::python;

StructureBuilder::StructureBuilder()
{
	LOG_DATA << "[python structure builder] create";
	m_stk.push_back( StackElement());
}

void StructureBuilder::openElement( const std::string& elemid_)
{
	LOG_DATA << "[python structure builder] open structure element '" << elemid_ << "'";
	m_stk.push_back( StackElement( elemid_));
}

void StructureBuilder::openArrayElement()
{
	LOG_DATA << "[python structure builder] open array element";
	m_stk.push_back( StackElement());
}

void StructureBuilder::closeElement()
{
	LOG_DATA << "[python structure builder] close element";
	if (m_stk.size() <= 1) throw std::runtime_error("element tags not balanced (close)");

	Object obj = m_stk.back().m_value;
	bool isArrayElement = m_stk.back().m_arrayelem;
	std::string key = m_stk.back().m_key;
	m_stk.pop_back();
	if (obj.defined())
	{
		if (isArrayElement)
		{
			if (m_stk.back().m_value.defined())
			{
				if (!PyList_Check( m_stk.back().m_value.borrowed()))
				{
					throw std::runtime_error( "array element added to non list object");
				}
			}
			else
			{
				m_stk.back().m_value = Object( PyList_New(0), false);
			}
			if (0>PyList_Append( m_stk.back().m_value.borrowed(), obj.borrowed()))
			{
				THROW_ON_ERROR( "failed to insert value into list (close element)");
			}
		}
		else
		{
			Object keyobj( key);
			if (0>PyDict_SetItem( m_stk.back().m_value.borrowed(), keyobj.borrowed(), obj.borrowed()))
			{
				THROW_ON_ERROR( "failed to insert value into dictionary (close element)");
			}
		}
	}
}

void StructureBuilder::setValue( const types::Variant& value_)
{
	LOG_DATA << "[python structure builder] set value '" << value_.tostring() << "'";
	switch (m_stk.back().m_value.type())
	{
		case Object::Nil:
		case Object::Atomic:
			m_stk.back().m_value = Object(value_);
			break;
		case Object::Array:
		case Object::Map:
			throw std::runtime_error("assigning value to non atomic type");
	}
}

Py_ssize_t StructureBuilder::lastArrayIndex() const
{
	if (m_stk.back().m_value.defined() && m_stk.back().m_value.array())
	{
		return PyList_Size( const_cast<PyObject*>( m_stk.back().m_value.borrowed()));
	}
	else
	{
		return 0;
	}
}

StructureR StructureBuilder::get() const
{
	if (m_stk.size() != 1) throw std::runtime_error("element tags not balanced (final structure)");
	return StructureR( new Structure( m_stk.back().m_value));
}

int StructureBuilder::taglevel() const
{
	return (int)m_stk.size()-1;
}

bool StructureBuilder::defined() const
{
	return m_stk.back().m_value.defined();
}

bool StructureBuilder::atomic() const
{
	return m_stk.back().m_value.type() == Object::Atomic;
}

std::string StructureBuilder::currentElementPath() const
{
	std::string rt;
	std::vector<StackElement>::const_iterator ci = m_stk.begin(), ce = m_stk.end();
	for (; ci != ce; ++ci)
	{
		if (rt.size()) rt.append( "/");
		if (!ci->m_arrayelem)
		{
			rt.append( ci->m_key);
		}
	}
	return rt;
}

void StructureBuilder::clear()
{
	m_stk.clear();
	m_stk.push_back( StackElement());
}


