//
// atomic counter template unit tests using google test framework (gTest)
//

#include <climits>
#include <cstdlib>

#include <boost/thread/thread.hpp>

#include "types/atomicCounter.hpp"
#include "gtest/gtest.h"


_Wolframe::AtomicCounter< unsigned long >	ulCounter0;
_Wolframe::AtomicCounter< int >		iCounter0;


// The fixture for testing class Wolframe::AtomicCounter
class CounterFixture : public ::testing::Test	{
protected:
	// Set-up work for each test here.
	// You can remove any or all of the following functions if its body is empty.
	CounterFixture()	{
		srand((unsigned)time(0));
		ulVal0 = (unsigned long)( rand() % ULONG_MAX );
		iVal0 = (int)( rand() % INT_MAX );
		noThreads = (int)( rand() % 256 );
		times = (unsigned long)( rand() % 100000 );
//		std::cout << noThreads << " threads, " << times << " iterations";
	}

	// Clean-up work that doesn't throw exceptions here.
	virtual ~CounterFixture()	{
	}


	// If the constructor and destructor are not enough for setting up
	// and cleaning up each test, you can define the following methods:
	//   Code here will be called immediately after the constructor (right
	//   before each test).
	virtual void SetUp() {
		ulCounter0 = ulVal0;
		iCounter0 = iVal0;
	}

	//   Code here will be called immediately after each test (right
	//   before the destructor).
	virtual void TearDown() {
	}

	// Objects declared here can be used by all tests in the test case.
	int                             noThreads;
	std::vector<boost::thread *>	threads;
	unsigned long                   ulVal0;
	int				iVal0;
	unsigned long			times;

public:
	static void incrementThread( unsigned long count )
	{
		for ( unsigned long i = 0; i < count; i++ )	{
			++ulCounter0;
			++iCounter0;
		}
	}

	static void decrementThread( unsigned long count )
	{
		for ( unsigned long i = 0; i < count; i++ )	{
			--ulCounter0;
			--iCounter0;
		}
	}
};


// Tests the AtomicCounter constructors and members
TEST_F( CounterFixture, Assignment )	{
	ASSERT_EQ( ulCounter0.value(), ulVal0 );
	ASSERT_EQ( iCounter0.value(), iVal0 );
}


// Tests the AtomicCounter operators
TEST_F( CounterFixture, Increment )	{
	ASSERT_EQ( ulCounter0.value(), ulVal0 );
	ASSERT_EQ( iCounter0.value(), iVal0 );

	for ( int i = 0; i < noThreads; i++ )   {
		boost::thread* thread = new boost::thread( &CounterFixture::incrementThread, times );
		threads.push_back( thread );
	}
	for ( int i = 0; i < noThreads; i++ )   {
		threads[i]->join();
		delete threads[i];
	}

	ASSERT_EQ( ulCounter0.value(), ulVal0 + noThreads * times );
	ASSERT_EQ( iCounter0.value(), (int)( iVal0 + noThreads * times ));

	ulCounter0 -= noThreads * times;
	iCounter0 -= noThreads * times;
	ASSERT_EQ( ulCounter0.value(), ulVal0 );
	ASSERT_EQ( iCounter0.value(), iVal0 );
}

TEST_F( CounterFixture, Decrement )	{
	ASSERT_EQ( ulCounter0.value(), ulVal0 );
	ASSERT_EQ( iCounter0.value(), iVal0 );

	ulCounter0 += noThreads * times;
	iCounter0 += noThreads * times;
	ASSERT_EQ( ulCounter0.value(), ulVal0 + noThreads * times );
	ASSERT_EQ( iCounter0.value(), (int)( iVal0 + noThreads * times ));

	for ( int i = 0; i < noThreads; i++ )   {
		boost::thread* thread = new boost::thread( &CounterFixture::decrementThread, times );
		threads.push_back( thread );
	}
	for ( int i = 0; i < noThreads; i++ )   {
		threads[i]->join();
		delete threads[i];
	}

	ASSERT_EQ( ulCounter0.value(), ulVal0 );
	ASSERT_EQ( iCounter0.value(), iVal0 );
}

TEST_F( CounterFixture, Prefix )	{
	ASSERT_EQ( ulCounter0.value(), ulVal0 );
	ASSERT_EQ( iCounter0.value(), iVal0 );

	unsigned long a = ++ulCounter0;
	int b = ++iCounter0;
	ASSERT_EQ( ulCounter0.value(), ulVal0 + 1 );
	ASSERT_EQ( a, ulVal0 + 1 );
	ASSERT_EQ( iCounter0.value(), iVal0 + 1 );
	ASSERT_EQ( b, iVal0 + 1 );

	a = --ulCounter0;
	b = --iCounter0;
	ASSERT_EQ( ulCounter0.value(), ulVal0 );
	ASSERT_EQ( a, ulVal0 );
	ASSERT_EQ( iCounter0.value(), iVal0 );
	ASSERT_EQ( b, iVal0 );
}

TEST_F( CounterFixture, Postfix )	{
	ASSERT_EQ( ulCounter0.value(), ulVal0 );
	ASSERT_EQ( iCounter0.value(), iVal0 );

	unsigned long a = ulCounter0++;
	int b = iCounter0++;
	ASSERT_EQ( ulCounter0.value(), ulVal0 + 1 );
	ASSERT_EQ( a, ulVal0 );
	ASSERT_EQ( iCounter0.value(), iVal0 + 1 );
	ASSERT_EQ( b, iVal0 );

	a = ulCounter0--;
	b = iCounter0--;
	ASSERT_EQ( ulCounter0.value(), ulVal0 );
	ASSERT_EQ( a, ulVal0 + 1 );
	ASSERT_EQ( iCounter0.value(), iVal0 );
	ASSERT_EQ( b, iVal0 + 1 );
}


int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
