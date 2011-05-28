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
