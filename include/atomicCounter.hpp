#ifndef _ATOMIC_COUNTER_HPP_INCLUDED
#define _ATOMIC_COUNTER_HPP_INCLUDED

#include <boost/thread/mutex.hpp>

namespace _SMERP	{

	template <typename T>
	class AtomicCounter	{
	public:
		AtomicCounter()		{}
		AtomicCounter( const T value )	{
			mtx_.lock(); val_ = value; mtx_.unlock();
		}

		const T val()	{
			T	ret;
			mtx_.lock(); ret = val_; mtx_.unlock();
			return ret;
		}

		void set( const T value )	{
			mtx_.lock(); val_ = value; mtx_.unlock();
		}

		void reset()	{
			mtx_.lock(); val_ = 0; mtx_.unlock();
		}

		bool operator== ( const T& rhs )	{
			mtx_.lock(); T ret = val_; mtx_.unlock();
			return( ret == rhs );
		}

		T operator= ( const T rhs )	{
			mtx_.lock(); val_ = rhs; T ret = val_; mtx_.unlock();
			return ret;
		}

		T operator++ ()	{
			mtx_.lock(); val_++; T ret = val_; mtx_.unlock();
			return ret;
		}

		T operator+= ( const T rhs )	{
			mtx_.lock(); val_ += rhs; T ret = val_; mtx_.unlock();
			return ret;
		}

		T operator-= ( const T rhs )	{
			mtx_.lock(); val_ -= rhs; T ret = val_; mtx_.unlock();
			return ret;
		}

		T operator-- ()	{
			mtx_.lock(); val_--; T ret = val_; mtx_.unlock();
			return ret;
		}

	private:
		T		val_;
		boost::mutex	mtx_;
	};

} // namespace _SMERP

#endif // _ATOMIC_COUNTER_HPP_INCLUDED
