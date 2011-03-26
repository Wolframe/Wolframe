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
// thread capable pool of objects
//

#ifndef _OBJECT_POOL_HPP_INCLUDED
#define _OBJECT_POOL_HPP_INCLUDED

#include <vector>

#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>

namespace _Wolframe	{

	// the object pool
	template < typename objectType >
	class ObjectPool	{
	public:
		ObjectPool( const unsigned to )	{ timeout_ = to; }
		ObjectPool()			{ timeout_ = 0; }
		~ObjectPool()			{}

		std::size_t available()		{ return available_.size(); }

		objectType* get()	{
			while( true )	{
				boost::unique_lock<boost::mutex> lock( mutex_ );
				if ( !available_.empty())	{
					objectType* obj = available_.back();
					available_.pop_back();
					return obj;
				}
				else	{
					if ( timeout_ == 0 )	{
						while( available_.empty() )
							cond_.wait( lock );
					}
					else {
						boost::system_time absTime = boost::get_system_time() + boost::posix_time::seconds( timeout_ );
						while( available_.empty() )
							if ( ! cond_.timed_wait( lock, absTime ))
								return NULL;
					}
				}
			}
			return NULL;
		}

		void add ( objectType* obj )	{
			{
				boost::lock_guard<boost::mutex> lock( mutex_ );
				available_.push_back( obj );
			}
			cond_.notify_one();
		}

		unsigned timeout()		{ return timeout_; }
		void timeout( unsigned to )	{ timeout_ = to; }
	private:
		std::vector< objectType* >	available_;
		boost::mutex			mutex_;
		boost::condition_variable	cond_;
		unsigned			timeout_;
	};

} // namespace _Wolframe

#endif // _OBJECT_POOL_HPP_INCLUDED
