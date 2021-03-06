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
///\file pythonObject.hpp
#include "pythonObject.hpp"
#include "pythonException.hpp"
#include "types/customDataType.hpp"
#include "types/datetime.hpp"
#include "types/bignumber.hpp"
#include <datetime.h>
#include <Python.h>

using namespace _Wolframe;
using namespace _Wolframe::langbind;
using namespace _Wolframe::langbind::python;

std::string Object::tostring( PyObject* obj)
{
	PyObject* serialization = PyObject_Str( obj);
	if (!serialization) THROW_ON_ERROR( "failed to convert atomic type to string (no serialization to string available)");
	const char* str = PyBytes_AsString( serialization);
	Py_ssize_t size = PyBytes_Size( serialization);
	std::string rt = std::string( str, size);
	Py_DECREF( serialization);
	return rt;
}

types::Variant Object::value( PyObject* obj)
{
	if (!obj) throw std::runtime_error( "accessing undefined object");

	if (PyLong_Check( obj))
	{
		return types::Variant( (types::Variant::Data::Int) PyLong_AsLong( obj));
	}
	else if (PyFloat_Check( obj))
	{
		return types::Variant( PyFloat_AsDouble( obj));
	}
	else if (PyBool_Check( obj))
	{
		if (PyObject_IsTrue( obj))
		{
			return types::Variant( true);
		}
		else
		{
			THROW_ON_ERROR( "failed to check boolean");
			return types::Variant( false);
		}
	}
	else if (PyBytes_Check( obj))
	{
		const char* str = PyBytes_AsString( obj);
		Py_ssize_t size = PyBytes_Size( obj);
		return types::Variant( str, size);
	}
	else if (PyUnicode_Check( obj))
	{
		PyObject* strUTF8 = PyUnicode_AsUTF8String( obj);
		if (!strUTF8) THROW_ON_ERROR( "error in conversion to UTF-8");
		const char* str = PyBytes_AsString( strUTF8);
		if (!str)
		{
			Py_DECREF( strUTF8);
			THROW_ON_ERROR( "failed to convert atomic type");
		}
		Py_ssize_t size = PyBytes_Size( strUTF8);
		types::Variant rt = types::Variant( str, size);
		Py_DECREF( strUTF8);
		return rt;
	}
	else if (obj == Py_None)
	{
		return types::Variant();
	}
	else if (PyByteArray_Check( obj))
	{
		const char* str = PyByteArray_AsString( obj);
		Py_ssize_t size = PyByteArray_Size( obj);
		return types::Variant( str, size);
	}
	else
	{
		PyObject* serialization = PyObject_Str( obj);
		if (!serialization) THROW_ON_ERROR( "failed to convert atomic type (no serialization to string available)");
		const char* str = PyBytes_AsString( serialization);
		Py_ssize_t size = PyBytes_Size( serialization);
		types::Variant rt = types::Variant( str, size);
		Py_DECREF( serialization);
		return rt;
	}
}

types::Variant Object::value() const
{
	return Object::value( const_cast<PyObject*>(m_obj));
}

std::string Object::tostring() const
{
	return Object::tostring( const_cast<PyObject*>(m_obj));
}

void Object::constructor( const types::Variant& val)
{
	switch (val.type())
	{
		case types::Variant::Custom:
		{
			types::Variant baseval;
			try
			{
				if (val.customref()->getBaseTypeValue( baseval)
				&&  baseval.type() != types::Variant::Custom)
				{
					constructor( baseval);
					break;
				}
			}
			catch (const std::runtime_error& e)
			{
				throw std::runtime_error( std::string("cannot convert value to base type for binding: ") + e.what());
			}
			constructor( val.tostring());
		}
		case types::Variant::Timestamp:
		{
			types::DateTime dt( val.totimestamp());
			if (dt.subtype() == types::DateTime::YYYYMMDD)
			{
				m_obj = PyDate_FromDate( dt.year(), dt.month(), dt.day());
				if (!m_obj) THROW_ON_ERROR( "failed to convert to python Date");
			}
			else
			{
				m_obj = PyDateTime_FromDateAndTime( dt.year(), dt.month(), dt.day(), dt.hour(), dt.minute(), dt.second(), dt.usecond());
				if (!m_obj) THROW_ON_ERROR( "failed to convert to python DateTime");
			}
			break;
		}
		case types::Variant::BigNumber:
		{
			std::string strval = val.tostring();
			if (val.bignumref()->scale() <= 0)
			{
				char* end = 0;
				m_obj = PyLong_FromString( const_cast<char*>(strval.c_str()), &end, 10);
				if (!m_obj) THROW_ON_ERROR( "failed to convert to big number (Long)");
				else if (end != strval.c_str()+strval.size())
				{
					Py_DECREF( m_obj);
					throw std::runtime_error( "superfluous characters at end of big number string");
				}
			}
			else
			{
				m_obj = PyBytes_FromStringAndSize( strval.c_str(), strval.size());
				if (!m_obj) THROW_ON_ERROR( "failed to convert to big number (fixed point number) as string");
			}
			break;
		}
		case types::Variant::Null:
			m_obj = Py_None;
			Py_INCREF( m_obj);
		break;
		case types::Variant::Int:
			m_obj = PyLong_FromLong( val.toint());
			if (!m_obj) THROW_ON_ERROR( "failed to convert to python long integer");
		break;
		case types::Variant::UInt:
			m_obj = PyLong_FromUnsignedLong( val.touint());
			if (!m_obj) THROW_ON_ERROR( "failed to convert to python unsigned long integer");
		break;
		case types::Variant::Bool:
			m_obj = PyBool_FromLong( val.tobool()?1:0);
		break;
		case types::Variant::Double:
			m_obj = PyFloat_FromDouble( val.todouble());
			if (!m_obj) THROW_ON_ERROR( "failed to convert to python double precision floating point value");
		break;
		case types::Variant::String:
			m_obj = PyUnicode_FromStringAndSize( val.charptr(), val.charsize());
			if (!m_obj) THROW_ON_ERROR( "failed to convert to python unicode string");
		break;
		default:
			throw std::runtime_error("try to get object for non atomic value");
	}
}

Object::Object( const types::Variant& val)
{
	constructor( val);
}

Object::Object( PyObject* obj_, bool isborrowed)
	:m_obj(obj_)
{
	if (isborrowed) Py_INCREF( m_obj);
}

Object::Object( const Object& o)
	:m_obj(o.m_obj)
{
	if (m_obj) Py_INCREF( m_obj);
}

Object::~Object()
{
	if (m_obj) Py_DECREF( m_obj);
}

const PyObject* Object::reference() const
{
	Py_INCREF( const_cast<PyObject*>(m_obj));
	return m_obj;
}

PyObject* Object::reference()
{
	Py_INCREF( m_obj);
	return m_obj;
}

Object::Type Object::type() const
{
	PyObject* obj = const_cast<PyObject*>(m_obj);
	Object::Type rt = Object::Nil;
	if (obj)
	{
		if (!PyDict_Check( obj))
		{
			rt = Object::Map;
		}
		else if (PySequence_Check( obj))
		{
			rt = Object::Array;
		}
		else
		{
			rt = Object::Atomic;
		}
	}
	return rt;
}

Object::Object( Type type_)
	:m_obj(0)
{
	switch (type_)
	{
		case Nil: return;
		case Array: m_obj = PyList_New( 0); return;
		case Map: m_obj = PyDict_New(); return;
		case Atomic: m_obj = Py_None; Py_INCREF(m_obj); return;
	}
	throw std::logic_error( "internal: illegal object type");
}

void Object::clear()
{
	if (m_obj)
	{
		Py_DECREF( m_obj);
		m_obj = 0;
	}
}

Object::Object( const char* s, bool unicode)
{
	if (unicode)
	{
		m_obj = PyBytes_FromString( s);
	}
	else
	{
		m_obj = PyUnicode_FromString( s);
	}
	if (!m_obj) THROW_ON_ERROR( "failed to convert to python string");
}

Object::Object( const char* s, std::size_t n, bool unicode)
{
	if (unicode)
	{
		m_obj = PyBytes_FromStringAndSize( s, n);
	}
	else
	{
		m_obj = PyUnicode_FromStringAndSize( s, n);
	}
	if (!m_obj) THROW_ON_ERROR( "failed to convert to python string");
}

void Object::constructor( const std::string& val, bool unicode)
{
	if (unicode)
	{
		m_obj = PyBytes_FromStringAndSize( val.c_str(), val.size());
	}
	else
	{
		m_obj = PyUnicode_FromStringAndSize( val.c_str(), val.size());
	}
	if (!m_obj) THROW_ON_ERROR( "failed to convert to python string");
}

Object::Object( const std::string& val, bool unicode)
{
	constructor( val, unicode);
}

Object& Object::operator=( const Object& o)
{
	clear();
	m_obj = o.m_obj;
	if (m_obj) Py_INCREF( m_obj);
	return *this;
}


