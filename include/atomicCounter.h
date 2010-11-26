#ifndef _ATOMIC_COUNTER_HPP_INCLUDED
#define _ATOMIC_COUNTER_HPP_INCLUDED

#include <boost/thread/mutex.hpp>

namespace _SMERP	{

	template <typename T>
	class AtomicCounter	{
	public:

	private:
		T		counter_;
		boost::mutex	mtx_;
	};

} // namespace _SMERP

#endif // _ATOMIC_COUNTER_HPP_INCLUDED
