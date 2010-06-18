//
// singleton.hpp
//

#ifndef _SINGLETON_HPP_INCLUDED
#define _SINGLETON_HPP_INCLUDED


#include <boost/utility.hpp>
#include <boost/thread/once.hpp>
#include <boost/scoped_ptr.hpp>

// Warning: If T's constructor throws, instance() will return a null reference.

// the initialized flag might not be needed but it looks like it increases performance

template<class T>
class Singleton : private boost::noncopyable
{

public:
	static T& instance()
	{
		if ( !initialized )
			boost::call_once( init, flag );
		return *t;
	}

protected:
	Singleton()	{}
	~Singleton()	{}
	Singleton( Singleton const& );
	Singleton& operator= ( Singleton const& );

	static void init() // never throws
	{
		t.reset( new T() );
		initialized = true;
	}

private:
	static boost::scoped_ptr<T>	t;
	static boost::once_flag		flag;
	static bool			initialized;
};


template<class T> boost::scoped_ptr<T> Singleton<T>::t(0);
template<class T> boost::once_flag Singleton<T>::flag = BOOST_ONCE_INIT;
template<class T> bool Singleton<T>::initialized = false;


#endif // _SINGLETON_HPP_INCLUDED
