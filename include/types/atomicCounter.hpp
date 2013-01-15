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

/// Simple thread safe counter class template
///
/// The implementation should be based on atomic primitives
/// specific to the platform where possible
template < typename T > class AtomicCounter	{
public:
	/// Create an atomic counter and initialize it to 0
	inline AtomicCounter()	{
		boost::lock_guard<boost::mutex>	lock( m_mtx );
		m_val = 0;
	}

	/// Create an atomic counter and initialize it to value
	inline explicit AtomicCounter( const T initialValue )	{
		boost::lock_guard<boost::mutex>	lock( m_mtx );
		m_val = initialValue;
	}

	/// Copy constructor
	inline AtomicCounter( const AtomicCounter<T>& counter )	{
		boost::lock_guard<boost::mutex>	lock( m_mtx );
		m_val = counter.m_val;
	}


	/// Print counter value
	friend std::ostream& operator<< ( std::ostream& out, const AtomicCounter<T>& x )	{
		out << x.m_val;
		return out;
	}


	/// Assign value to the counter
	inline AtomicCounter<T>& operator = ( const T newValue )	{
		boost::lock_guard<boost::mutex>	lock( m_mtx );
		m_val = newValue;
		return *this;
	}

	/// Assign counter value to the counter
	inline AtomicCounter<T>& operator = ( const AtomicCounter<T>& counter )	{
		boost::lock_guard<boost::mutex>	lock( m_mtx );
		m_val = value.m_val;
		return *this;
	}


	/// Counter value operator
	inline operator T () const	{ return m_val; }

	/// Counter value function
	inline T value() const		{ return m_val; }


	/// Prefix increment operator
	inline T operator ++ ()	{
		T	ret;
		{
			boost::lock_guard<boost::mutex>	lock( m_mtx );
			ret = ++m_val;
		}
		return ret;
	}

	/// Postfix increment operator
	inline T operator ++ ( int )	{
		T	ret;
		{
			boost::lock_guard<boost::mutex>	lock( m_mtx );
			ret = m_val++;
		}
		return ret;
	}


	/// Prefix decrement operator
	inline T operator -- ()	{
		T	ret;
		{
			boost::lock_guard<boost::mutex>	lock( m_mtx );
			ret = --m_val;
		}
		return ret;
	}

	/// Postfix decrement operator
	inline T operator -- ( int )	{
		T	ret;
		{
			boost::lock_guard<boost::mutex>	lock( m_mtx );
			ret = m_val--;
		}
		return ret;
	}

	inline T operator += ( const T difference )	{
		T	ret;
		{
			boost::lock_guard<boost::mutex>	lock( m_mtx );
			m_val += difference;
			ret = m_val;
		}
		return ret;
	}

	inline T operator -= ( const T difference )	{
		T	ret;
		{
			boost::lock_guard<boost::mutex>	lock( m_mtx );
			m_val -= difference;
			ret = m_val;
		}
		return ret;
	}

private:
	volatile T		m_val;
	mutable boost::mutex	m_mtx;
};

} // namespace _Wolframe

#endif // _ATOMIC_COUNTER_HPP_INCLUDED
