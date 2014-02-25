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
#ifndef _Wolframe_COUNTED_REFERENCE_HPP_INCLUDED
#define _Wolframe_COUNTED_REFERENCE_HPP_INCLUDED
//\file types/countedReference.hpp
//\brief Shared reference to an object exchangeable in a single thread context

#include <cstddef>
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace types {

//\class CountedReference
//\brief Shared reference to an object exchangeable in a single thread context
//\tparam OBJ
template <class OBJ>
class CountedReference
{
public:
	//\class Pointer
	//\brief Pointer for reference with another indirection to make the exchange of the object possible
	struct Pointer
	{
		OBJ* m_ptr;	//< object pointer

		//\brief Constructor
		explicit Pointer( OBJ* ptr_=0)
			:m_ptr(ptr_){}
		//\brief Destructor
		~Pointer()
		{
			if (m_ptr) delete m_ptr;
		}
	};

	//\brief Copy constructor
	//\param[in] o shared reference to copy
	CountedReference( const CountedReference& o)
		:m_ref(o.m_ref){}

	//\brief Constructor
	//\param[in] ptr object pointer
	explicit CountedReference( OBJ* ptr=0)
		:m_ref(boost::shared_ptr<Pointer>( new Pointer( ptr))){}

	//\brief Destructor
	virtual ~CountedReference(){}

	//\brief Assignment
	CountedReference& operator =( const CountedReference& o)
	{
		m_ref = o.m_ref;
		return *this;
	}

	//\brief Get the pointer to the object
	//\return the pointer
	OBJ* get() const
	{
		return m_ref->m_ptr;
	}

	//\brief Exchange the object (not thread safe access)
	//\param[in] ptr object pointer
	void reset( OBJ* ptr=0)
	{
		if (m_ref->m_ptr && m_ref->m_ptr != ptr) delete m_ref->m_ptr;
		m_ref->m_ptr = ptr;
	}

	//\brief Pointer access operator
	//\return the pointer
	OBJ* operator -> () const
	{
		return m_ref->m_ptr;
	}

	//\brief Reference access operator
	//\return the object reference
	OBJ& operator *() const
	{
		return *m_ref->m_ptr;
	}

private:
	boost::shared_ptr<Pointer> m_ref;	//< shared reference
};

}}//namespace
#endif



