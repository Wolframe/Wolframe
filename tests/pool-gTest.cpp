//
// atomic counter template unit tests using google test framework (gTest)
//

#include <climits>
#include <cstdlib>
#include <ctime>
#include <iostream>

#include <boost/thread/thread.hpp>

#include "objectPool.hpp"
#include <gtest/gtest.h>

#include <unistd.h>


class testObject	{
public:
	testObject()		{ threads = 0, uses = 0; }
	void doSomething()	{ threads++, uses++; usleep( 100 ); threads--; }
	bool unused()		{ return threads == 0; }
	unsigned long used()	{ return uses; }
private:
	int		threads;
	unsigned long	uses;
};


// The fixture for testing SMERP::ObjectPool template
class ObjectPoolFixture : public ::testing::Test	{
protected:
	// Set-up work for each test here.
	// You can remove any or all of the following functions if its body is empty.
	ObjectPoolFixture()	{
		srand((unsigned)time(0));
		poolSize = (int)( rand() % 256 );
		noThreads = (int)( rand() % 256 );
		times = (unsigned long)( rand() % 100000 );
	}

	// Clean-up work that doesn't throw exceptions here.
	virtual ~ObjectPoolFixture()	{
	}


	// If the constructor and destructor are not enough for setting up
	// and cleaning up each test, you can define the following methods:
	//   Code here will be called immediately after the constructor (right
	//   before each test).
	virtual void SetUp() {
		for ( unsigned long i = 0; i < poolSize; i++ )	{
			testObject *tstObj = new testObject();
			tstObjs.push_back( tstObj );
			objPool.add( tstObj );
		}
	}

	//   Code here will be called immediately after each test (right
	//   before the destructor).
	virtual void TearDown() {
		for ( unsigned long i = 0; i < poolSize; i++ )
			objPool.get();
		for ( unsigned long i = 0; i < poolSize; i++ )
			delete tstObjs[i];
	}

	// Objects declared here can be used by all tests in the test case.
	int					noThreads;
	std::vector< boost::thread* >		threads;
	std::vector< testObject* >		tstObjs;
	_SMERP::ObjectPool< testObject >	objPool;
	unsigned				poolSize;
	unsigned long				times;

public:
	static void testThread( _SMERP::ObjectPool< testObject > *pool, unsigned count )
	{
		for ( std::size_t i = 0; i < count; i++ )	{
			testObject *tstObj = pool->get();
			if ( tstObj != NULL )	{
				tstObj->doSomething();
				pool->add( tstObj );
			}
		}
	}

};


// Tests the ObjectPool get & release
TEST_F( ObjectPoolFixture, doSomething )	{
	for ( unsigned long i = 0; i < poolSize; i++ )	{
		ASSERT_EQ( tstObjs[i]->used(), 0 );
	}

	for ( int i = 0; i < noThreads; i++ )   {
		boost::thread* thread = new boost::thread( &ObjectPoolFixture::testThread, &objPool, times );
		threads.push_back( thread );
	}
	for ( int i = 0; i < noThreads; i++ )   {
		threads[i]->join();
		delete threads[i];
	}

	unsigned long used = 0;
	for ( unsigned long i = 0; i < poolSize; i++ )	{
		ASSERT_TRUE( tstObjs[i]->unused() );
		used += tstObjs[i]->used();
	}

	ASSERT_EQ( used, noThreads * times );
}


int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
