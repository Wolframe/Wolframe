/************************************************************************
Copyright (C) 2011, 2012 Project Wolframe.
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
///\file types/countedReference.hpp
///\brief Multiple references to an object that is freed with its last reference.
///\remark The reference template introduced here should be replaced by an appropriate boost smart pointer. The existence of this module is due easier injection of tracing and debugging aspects

#include <cstddef>
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace types {

///\class CountedReference
///\brief Reference to an object that is freed when not referenced anymore.
///\remark Designed to be substitutable by 'shared_ptr' except that the reset function that is different (BUG)
///\tparam OBJ
template <class OBJ>
class CountedReference
{
public:
	struct Pointer
	{
		OBJ* m_ptr;
		int m_cnt;
	};
	
	CountedReference( const CountedReference& o)
		:m_ref(o.m_ref)
	{
		m_ref->m_cnt++;
	}

	explicit CountedReference( OBJ* ptr)
		:m_ref( new Pointer())
	{
		m_ref->m_cnt = 1;
		m_ref->m_ptr = ptr;
	}

	CountedReference()
		:m_ref( new Pointer())
	{
		m_ref->m_cnt = 1;
		m_ref->m_ptr = 0;
	}

	~CountedReference()
	{
		if (--m_ref->m_cnt == 0)
		{
			if (m_ref->m_ptr) delete m_ref->m_ptr;
			delete m_ref;
		}
	}
	
	CountedReference& operator =( const CountedReference& o)
	{
		if (--m_ref->m_cnt == 0)
		{
			if (m_ref->m_ptr) delete m_ref->m_ptr;
			delete m_ref;
		}
		m_ref = o.m_ref;
		m_ref->m_cnt++;
		return *this;
	}

	OBJ* get() const
	{
		return m_ref->m_ptr;
	}
	
	void reset( OBJ* ptr=0)
	{
		if (m_ref->m_ptr && m_ref->m_ptr != ptr) delete m_ref->m_ptr;
		m_ref->m_ptr = ptr; 
	}
	
	int cnt() const
	{
		return m_ref->m_cnt;
	}
	
	OBJ* operator -> () const
	{
		return m_ref->m_ptr;
	}
	
	OBJ& operator *() const
	{
		return *m_ref->m_ptr;
	}

private:
	Pointer* m_ref;
};

}}//namespace
#endif



