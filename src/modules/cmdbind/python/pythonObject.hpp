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
///\file pythonObject.hpp
///\brief Interface to python object for using python reference counting with C++ scope
#ifndef _Wolframe_python_OBJECT_HPP_INCLUDED
#define _Wolframe_python_OBJECT_HPP_INCLUDED
#include <Python.h>
#include "types/variant.hpp"

namespace _Wolframe {
namespace langbind {
namespace python {

class Object
{
public:
	enum Type
	{	Nil,				//< undefined object
		Array,				//< PyList object
		Map,				//< PyDict object
		Atomic				//< Atomic type object
	};
	///\brief Default constructor
	Object()				:m_obj(0){}
	///\brief Constructor
	Object( Type type_);
	///\brief Constructor
	Object( PyObject* obj_, bool isborrowed=true);
	///\brief Copy constructor
	Object( const Object& o);
	///\brief Constructor
	Object( const types::Variant& v);
	///\brief Constructor
	Object( const char* s, bool unicode=false);
	///\brief Constructor
	Object( const char* s, std::size_t n, bool unicode=false);
	///\brief Constructor
	Object( const std::string& s, bool unicode=false);

	///\brief Destructor
	~Object();

	///\brief Assign object
	Object& operator=( const Object& o);

	///\brief Get the type of the objects
	Type type() const;

	///\brief Get the object value as variant
	types::Variant value() const;

	///\brief Find out if 'this' represents an atomic value
	///\return true, if yes
	bool atomic() const		{return type() == Object::Atomic;}
	///\brief Find out if 'this' represents an array of 'Structure'
	///\return true, if yes
	bool array() const		{return type() == Object::Array;}
	///\brief Find out if this object is defined
	///\return true if yes
	bool defined() const			{return m_obj != 0;}

	///\brief Get a borrowed reference of a python object (without incrementing reference count)
	const PyObject* borrowed() const	{return m_obj;}
	PyObject* borrowed()			{return m_obj;}

	///\brief Get a reference of a python object (with proper incrementing reference count, if the object is defined)
	const PyObject* reference() const;
	PyObject* reference();

	///\brief Release the object and set it to undefined
	void clear();

private:
	PyObject* m_obj;
};

}}}//namespace
#endif

