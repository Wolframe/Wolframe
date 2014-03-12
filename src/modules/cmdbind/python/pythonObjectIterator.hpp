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
//\file pythonObjectIterator.hpp
//\brief Interface to C++ (STL like) iterator on a python object
#ifndef _Wolframe_python_OBJECT_ITERATOR_HPP_INCLUDED
#define _Wolframe_python_OBJECT_ITERATOR_HPP_INCLUDED
#include "pythonObject.hpp"
#include "types/variant.hpp"

//\brief Forward declaration for Python
typedef struct _object PyObject;
typedef long int Py_ssize_t;

namespace _Wolframe {
namespace langbind {
namespace python {

//\brief Iterator on structure or array elements
class ObjectIterator
{
public:
	//\brief Default constructor
	ObjectIterator();
	//\brief Constructor
	explicit ObjectIterator( const Object& obj_);
	//\brief Copy constructor
	ObjectIterator( const ObjectIterator& o);
	//\brief Destructor
	~ObjectIterator();

	//\brief Compare objects
	//\return -1 if 'this' is smaller than 'o', +1 if 'this' is bigger than 'o', 0 if 'this' and 'o' are equal
	int compare( const ObjectIterator& o) const			{return (int)m_pos - (int)o.m_pos;}

	//\brief Compare operators
	bool operator==( const ObjectIterator& o) const			{return compare(o) == 0;}
	bool operator!=( const ObjectIterator& o) const			{return compare(o) != 0;}
	bool operator<( const ObjectIterator& o) const			{return compare(o) < 0;}
	bool operator<=( const ObjectIterator& o) const			{return compare(o) <= 0;}
	bool operator>( const ObjectIterator& o) const			{return compare(o) > 0;}
	bool operator>=( const ObjectIterator& o) const			{return compare(o) >= 0;}

	//\brief Increment
	ObjectIterator& operator++()					{fetch_next(); return *this;}
	ObjectIterator operator++(int)					{ObjectIterator rt(*this); fetch_next(); return rt;}

	struct Element
	{
		PyObject* key;			//< key if iterating on a python dictionary
		PyObject* val;			//< element value

		bool atomic() const;
		bool array() const;
		types::Variant getValue() const;
	};
	//\brief Accessor
	const Element* operator->() const				{return &m_elem;}
	const Element& operator*() const				{return m_elem;}

private:
	void fetch_next();

	Object m_obj;				//< object iterated on
	PyObject* m_itr;			//< iterator in case of iterating on a python list
	Py_ssize_t m_pos;			//< iterator in case of iterating on a python dictionary
	Element m_elem;				//< currently visited element
};

}}}//namespace
#endif




