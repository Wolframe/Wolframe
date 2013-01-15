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
#ifndef _Wolframe_UNIQUE_REFERENCE_HPP_INCLUDED
#define _Wolframe_UNIQUE_REFERENCE_HPP_INCLUDED
///\file types/uniqueReference.hpp
///\brief Unique reference to an object that is freed by the destructor.

#include <cstddef>

namespace _Wolframe {
namespace types {

///\class UniqueReference
///\brief Unique reference to an object that is freed by the destructor.
///\remark Substitutable by a 'unique_ptr' when it will exist as announced by the standard
///\tparam OBJ
template <class OBJ>
class UniqueReference
{
public:
	///\brief Move constructor
	UniqueReference( const UniqueReference&& o)
		:m_ref(o.m_ref)
	{}

	explicit UniqueReference( OBJ* ptr)
		:m_ref(ptr)
	{}

	UniqueReference()
		:m_ref(0)
	{}

	~UniqueReference()
	{
		if (m_ref)
		{
			delete m_ref;
		}
	}
	
	const OBJ* get() const
	{
		return m_ref;
	}

	OBJ* get()
	{
		return m_ref;
	}

	void reset( OBJ* ptr=0)
	{
		if (m_ref) delete m_ref;
		m_ref = ptr;
	}
	
	OBJ* operator -> () const
	{
		return m_ref;
	}
	
	OBJ& operator *() const
	{
		return *m_ref;
	}

private:
	UniqueReference& operator =( const UniqueReference&){} //non copyable
	UniqueReference( const UniqueReference&){} //non copyable

private:
	OBJ* m_ref;
};

}}//namespace
#endif



