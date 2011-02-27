//
// thread capable pool of objects
//

#ifndef _OBJECT_POOL_HPP_INCLUDED
#define _OBJECT_POOL_HPP_INCLUDED


#include <list>

#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>


namespace _Wolframe	{

	// the object pool
	template < typename objectType >
	class ObjectPool	{
	public:
		ObjectPool( const unsigned to )	{ timeout_ = to; }
		ObjectPool( )			{ timeout_ = 0; }
		~ObjectPool()			{}

		std::size_t available()		{ return available_.size(); }

		objectType* get()	{
			while( true )	{
				boost::unique_lock<boost::mutex> lock( mutex_ );
				if ( !available_.empty())	{
					objectType* obj = available_.front();
					available_.pop_front();
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
		std::list< objectType* >		available_;
		boost::mutex				mutex_;
		boost::condition_variable		cond_;
		unsigned				timeout_;
	};

} // namespace _Wolframe

#endif // _OBJECT_POOL_HPP_INCLUDED
