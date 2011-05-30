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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
//
// atomic (mutex protected) counters

#ifndef _ATOMIC_COUNTER_HPP_INCLUDED
#define _ATOMIC_COUNTER_HPP_INCLUDED

#include <boost/thread/mutex.hpp>

namespace _Wolframe	{

template <typename T>
class AtomicCounter	{
public:
	AtomicCounter()		{
		m_mtx.lock(); m_val = 0; m_mtx.unlock();
	}
	AtomicCounter( const T value )	{
		m_mtx.lock(); m_val = value; m_mtx.unlock();
	}

	friend std::ostream& operator<< ( std::ostream& out, const AtomicCounter<T>& x )	{
		out << x.m_val;
		return out;
	}

	const T val()	{
		T	ret;
		m_mtx.lock(); ret = m_val; m_mtx.unlock();
		return ret;
	}

	void set( const T value )	{
		m_mtx.lock(); m_val = value; m_mtx.unlock();
	}

	void reset()	{
		m_mtx.lock(); m_val = 0; m_mtx.unlock();
	}

	bool operator== ( const T& rhs )	{
		m_mtx.lock(); T ret = m_val; m_mtx.unlock();
		return( ret == rhs );
	}

	bool operator!= ( const T& rhs )	{
		m_mtx.lock(); T ret = m_val; m_mtx.unlock();
		return( ret != rhs );
	}

	T operator= ( const T rhs )	{
		m_mtx.lock(); m_val = rhs; T ret = m_val; m_mtx.unlock();
		return ret;
	}

	T operator++ ()	{
		m_mtx.lock(); m_val++; T ret = m_val; m_mtx.unlock();
		return ret;
	}

	T operator+= ( const T rhs )	{
		m_mtx.lock(); m_val += rhs; T ret = m_val; m_mtx.unlock();
		return ret;
	}

	T operator-= ( const T rhs )	{
		m_mtx.lock(); m_val -= rhs; T ret = m_val; m_mtx.unlock();
		return ret;
	}

	T operator-- ()	{
		m_mtx.lock(); m_val--; T ret = m_val; m_mtx.unlock();
		return ret;
	}

	///
	bool operator> ( const T rhs )	{ return m_val > rhs; }
	bool operator>= ( const T rhs )	{ return m_val >= rhs; }
	bool operator< ( const T rhs )	{ return m_val < rhs; }
	bool operator<= ( const T rhs )	{ return m_val <= rhs; }

private:
	T		m_val;
	boost::mutex	m_mtx;
};

} // namespace _Wolframe

#endif // _ATOMIC_COUNTER_HPP_INCLUDED
