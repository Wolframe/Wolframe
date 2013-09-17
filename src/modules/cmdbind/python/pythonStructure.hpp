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
///\file pythonStructure.hpp
///\brief Interface to python data structure representing input/output of a python form function
#ifndef _Wolframe_python_STRUCTURE_HPP_INCLUDED
#define _Wolframe_python_STRUCTURE_HPP_INCLUDED
#include "types/countedReference.hpp"
#include "types/variant.hpp"
#include "utils/printFormats.hpp"
#include <vector>
#include <string>
#include <ostream>
#include <Python.h>

namespace _Wolframe {
namespace langbind {
namespace python {

///\class Structure
///\brief Data structure for input and output of a 'python' function call
class Structure
{
public:
	///\brief Constructor
	Structure();
	explicit Structure( PyObject* obj_);
	Structure( const Structure& o);
	virtual ~Structure();

	///\brief Create an element in a structure and get a reference pointer to it
	///\param[in] elemid_ Id of the created element
	///\remark Throws on error
	///\remark Only a reference is returned; the disposal of the structure (ownership) is up to 'this'
	Structure* addStructElement( const std::string& elemid_);

	///\brief Create an element in an array and get a reference pointer to it
	///\param[in] elemid_ Id of the array
	///\remark Throws on error
	///\remark Only a reference is returned; the disposal of the structure (ownership) is up to 'this'
	Structure* addArrayElement();

	///\brief Setter for element value in case of this representing an atom
	///\param[in] value value or content element of 'this'
	void setValue( const types::Variant& value_);
	///\brief Getter for element value in case of this representing an atom
	const types::Variant& getValue() const;

	///\brief Find out if 'this' represents an atomic value
	///\return true, if yes
	bool atomic() const;
	///\brief Find out if 'this' represents an array of 'Structure'
	///\return true, if yes
	bool array() const;
	///\brief Find out how many elements are defined in this structure
	///\return the count

	///\brief Get the index of the last element in case of an array
	unsigned int lastArrayIndex() const;

	///\brief Iterator on structure or array elements
	class const_iterator
	{
	public:
		const_iterator();
		explicit const_iterator( const Structure* st_);
		const_iterator( const const_iterator& o);
		~const_iterator();

		int compare( const const_iterator& o) const			{return (int)m_pos - (int)o.m_pos;}

		bool operator==( const const_iterator& o) const			{return compare(o) == 0;}
		bool operator!=( const const_iterator& o) const			{return compare(o) != 0;}
		bool operator<( const const_iterator& o) const			{return compare(o) < 0;}
		bool operator<=( const const_iterator& o) const			{return compare(o) <= 0;}
		bool operator>( const const_iterator& o) const			{return compare(o) > 0;}
		bool operator>=( const const_iterator& o) const			{return compare(o) >= 0;}

		const_iterator& operator++()					{fetch_next(); return *this;}
		const_iterator operator++(int)					{const_iterator rt(*this); fetch_next(); return rt;}

		struct Element
		{
			PyObject* key;
			PyObject* val;

			bool atomic() const;
			bool array() const;
			types::Variant getValue() const;
		};
		const Element* operator->() const				{return &m_elem;}
		const Element& operator*() const				{return m_elem;}

	private:
		void fetch_next();

		const Structure* m_st;
		PyObject* m_itr;
		Py_ssize_t m_pos;
		Element m_elem;
	};

	///\brief Get the start iterator on structure or array elements
	const_iterator begin() const						{return const_iterator(this);}
	///\brief Get the end marker for a structure or and array
	const_iterator end() const						{return const_iterator();}

	///\brief Print the structure serialized as string to out
	void print( std::ostream& out, const utils::PrintFormat* pformat, std::size_t level) const;
	///\brief Get the structure serialized as string for output
	std::string tostring( const utils::PrintFormat* pformat=utils::logPrintFormat()) const;

	enum Type
	{	Nil,
		Array,
		Map,
		Atomic
	};
	Type type() const;
	PyObject* obj() const							{return m_obj;}

	void clear();

private:
	friend class Structure::const_iterator;
	PyObject* m_obj;			//< python data structure representation
};

///\brief Reference with ownership to a structure
typedef types::CountedReference<Structure> StructureR;

}}}//namespace
#endif

