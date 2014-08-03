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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
//\file types/syncCounter.hpp
//\brief Atomic (lockfree) counter implementation for a synchronized counting of objects
#ifndef _Wolframe_SYNCHRONIZED_COUNTER_HPP_INCLUDED
#define _Wolframe_SYNCHRONIZED_COUNTER_HPP_INCLUDED
#include <boost/atomic/atomic.hpp>

namespace _Wolframe {
namespace types {

//\class SyncCounter
//\brief Atomic counter with upper value limit for a synchronized counting of objects
class SyncCounter
	:protected boost::atomic<unsigned int>
{
public:
	typedef unsigned int CounterType;
	typedef boost::atomic<CounterType> Parent;
	enum {NoLimit=0};

	//\brief Constructor
	explicit SyncCounter( CounterType limitValue_=NoLimit)
		:boost::atomic<CounterType>(0)
		,m_limitValue(limitValue_)
	{}

	//\brief Aquire the counter
	//\return 0 in case of error, the counter instance auqired (>0) in case of success
	CounterType aquire()
	{
		if (m_limitValue)
		{
			for (;;)
			{
				CounterType val = Parent::load( boost::memory_order_relaxed);
				if (val >= m_limitValue) return 0;
				if (Parent::compare_exchange_strong( val, val+1, boost::memory_order_acquire))
				{
					return val+1;
				}
			}
		}
		else
		{
			return Parent::fetch_add( 1, boost::memory_order_acquire)+1;
		}
	}

	//\brief Release the counter
	void release()
	{
		Parent::fetch_sub( 1, boost::memory_order_release);
	}

	CounterType operator*() const
	{
		return load();
	}

	///\brief Aquire a counter in a limit in an exception save scope
	// Example:
	//	SyncCounter globalCnt;
	//	....
	//	SyncCounter::ScopedAquire cntscope( globalCnt);
	//	if (cntscope.entered())
	//	{
	//		... do something that might throw
	//		cntscope.done();
	//	}
	//
	class ScopedAquire
	{
	public:
		ScopedAquire( SyncCounter& cc_)
			:m_cc(&cc_)
		{
			if (!m_cc->aquire()) m_cc = 0;
		}

		~ScopedAquire()
		{
			if (m_cc) m_cc->release();
		}

		//\brief Evaluate if the counter could be aquired in the given limits
		//\return true if yes
		bool entered() const
		{
			return m_cc;
		}

		//\brief Complete aquiring of the counter: All operations belonging to the counter aquiring that might throw have been completed with success.
		void done()
		{
			m_cc = 0;
		}

	private:
		SyncCounter* m_cc;		//< global counter reference
	};

	void setLimit( const CounterType& limitValue_)
	{
		m_limitValue = limitValue_;
	}

private:
	CounterType m_limitValue;		//< upper counter limit
};

}}//namespace
#endif


