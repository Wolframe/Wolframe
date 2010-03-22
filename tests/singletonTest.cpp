//
// Singleton test program
//

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <iostream>

#include <boost/thread/thread.hpp> 

#include "singleton.hpp"


class TestClass : public Singleton<TestClass>
{

friend class Singleton<TestClass>;

public:
	void doSomething()	{ std::cout << "Doing something" << std::endl; }

protected:
private:
	TestClass()	{	std::cout << "Initializing TestClass" << std::endl;
				boost::this_thread::sleep(boost::posix_time::seconds( 3 ));
				std::cout << "TestClass initialized" << std::endl;
			}

};



void threadFunction()
{
	TestClass::instance().doSomething();
}


int main( int argc, char *argv[] )
{
	boost::thread thread1( &threadFunction );
	boost::thread thread2( &threadFunction );
	boost::thread thread3( &threadFunction );
	boost::thread thread4( &threadFunction );
	boost::thread thread5( &threadFunction );
	boost::thread thread6( &threadFunction );
	boost::thread thread7( &threadFunction );
	thread1.join();
	thread2.join();
	thread3.join();
	thread4.join();
	thread5.join();
	thread6.join();
	thread7.join();

	return 0;
}

