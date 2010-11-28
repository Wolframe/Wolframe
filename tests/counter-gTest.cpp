//
// version class unit tests using google test framework (gTest)
//

#include <climits>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <stdio.h>

#include "atomicCounter.hpp"
#include <gtest/gtest.h>


#define	MAX_STRING	128

// The fixture for testing class SMERP::AtomicCounter
class CounterFixture : public ::testing::Test	{
protected:
	// Set-up work for each test here.
	// You can remove any or all of the following functions if its body is empty.
	CounterFixture()	{
		srand((unsigned)time(0));
		ulVal0 = (unsigned)( rand() % ULONG_MAX );
		iVal0 = (int)( rand() % INT_MAX );
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
	unsigned long	ulVal0;
	int		iVal0;
	_SMERP::AtomicCounter< unsigned long >	ulCounter0;
	_SMERP::AtomicCounter< int >		iCounter0;
};


// Tests the Version constructors and members
TEST_F( CounterFixture, Assignment )	{
	ASSERT_EQ( ulCounter0.val(), ulVal0 );
	ASSERT_EQ( iCounter0.val(), iVal0 );
}


// Tests the Version operators
TEST_F( CounterFixture, Operators )	{
}


int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
