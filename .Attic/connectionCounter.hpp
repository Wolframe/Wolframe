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
///\file connectionCounter.hpp
///\brief Alternative atomic (lockfree) counter implementation for handling the number of connections

#ifndef _CONNECTION_COUNTER_HPP_INCLUDED
#define _CONNECTION_COUNTER_HPP_INCLUDED
#include <boost/atomic/atomic.hpp>

namespace _Wolframe {
namespace types {

///\class Atomic limit counter for number of connections
class ConnectionCounter
{
public:
	typedef unsigned int CounterType;
	typedef boost::atomic<CounterType> Parent;

	ConnectionCounter( CounterType initialValue, CounterType limitValue)
		:boost::atomic<CounterType>(initialValue){}

	CounterType aquire()
	{
		for (;;)
		{
			CounterType val = Parent::load( boost::memory_order_acquire);
			if (val >= limitValue) return 0;
			if (compare_exchange_strong( val, val+1, boost::memory_order_acq_rel)) return val+1;
			if (val >= limitValue) return 0;
		}
	}

	void release()
	{
		Parent::fetch_sub( 1, boost::memory_order_relaxed);
	}

	///\brief Aquire a counter in a limit in an exception save scope
	// Example:
	//	ConnectionCounter globalCnt;
	//	....
	//	ScopedAquire cntscope( globalCnt);
	//	if (cntscope.entered())
	//	{
	//		... do something that might throw
	//		cntscope.done();
	//	}
	//
	class ScopedAquire
	{
		ScopedAquire( ConnectionCounter& cc_)
			:m_cc(&cc_)
		{
			if (!m_cc->aquire()) m_cc = 0;
		}

		~Scope()
		{
			if (m_cc) m_cc->release();
		}

		bool entered() const
		{
			return m_cc;
		}

		void done()
		{
			m_cc = 0;
		}

	private:
		ConnectionCounter* m_cc;
	};

private:
	boost::memory_order m_order;
};

}}//namespace
#endif


