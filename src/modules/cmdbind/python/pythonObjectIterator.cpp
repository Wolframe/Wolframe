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
///\file pythonObjectIterator.cpp
#include "pythonObjectIterator.hpp"
#include "pythonException.hpp"

using namespace _Wolframe;
using namespace _Wolframe::langbind;
using namespace _Wolframe::langbind::python;

bool ObjectIterator::Element::atomic() const
{
	Object vv( val);
	return vv.type() == Object::Atomic;
}

bool ObjectIterator::Element::array() const
{
	Object vv( val);
	return vv.type() == Object::Array;
}

types::Variant ObjectIterator::Element::getValue() const
{
	Object vv( val);
	return vv.value();
}

ObjectIterator::ObjectIterator()
	:m_itr(0),m_pos(0)
{
	m_elem.key = 0;
	m_elem.val = 0;
}

ObjectIterator::ObjectIterator( const Object& obj_)
	:m_obj(obj_),m_itr(0),m_pos(0)
{
	m_elem.key = 0;
	m_elem.val = 0;

	m_itr = PyObject_GetIter( m_obj.borrowed());
	fetch_next();
}

ObjectIterator::ObjectIterator( const ObjectIterator& o)
	:m_obj(o.m_obj),m_itr(o.m_itr),m_pos(o.m_pos)
{
	if (m_itr) Py_INCREF( m_itr);
	m_elem.key = o.m_elem.key;
	m_elem.val = o.m_elem.val;
	if (m_elem.key == 0 && m_elem.val != 0)
	{
		//... val retrieved with PyIter_Next => New Reference that has to be duplicated
		Py_INCREF( m_elem.val);
	}
}

ObjectIterator::~ObjectIterator()
{
	if (m_itr) Py_DECREF( m_itr);
	if (m_elem.key == 0 && m_elem.val != 0)
	{
		//... val retrieved with PyIter_Next => New Reference that has to be released
		Py_DECREF( m_elem.val);
	}
}

void ObjectIterator::fetch_next()
{
	switch (m_obj.type())
	{
		case Object::Nil: break;
		case Object::Atomic: break;
		case Object::Map:
		{
			if (!PyDict_Next( m_obj.borrowed(), &m_pos, &m_elem.key, &m_elem.val))
			{
				m_elem.key = 0;
				m_elem.val = 0;
				m_pos = 0;
				THROW_ON_ERROR( "failed to get next entry in dictionary");
			}
			break;
		}
		case Object::Array:
		{
			if (m_itr)
			{
				if (m_elem.key == 0 && m_elem.val != 0)
				{
					//... val retrieved with PyIter_Next => New Reference that has to be duplicated
					Py_DECREF( m_elem.val);
				}
				else
				{
					m_elem.key = 0;
				}
				m_elem.val = PyIter_Next( m_itr);	// borrowed reference
				if (!m_elem.val)
				{
					THROW_ON_ERROR( "failed to get next entry in list");
					m_pos = 0;
				}
				else
				{
					++m_pos;
				}
			}
			else
			{
				m_elem.key = 0;
				m_elem.val = 0;
			}
			break;
		}
	}
}

