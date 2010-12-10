//
// version class unit tests using google test framework (gTest)
//

#include <climits>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <stdio.h>

#include "version.hpp"
#include <gtest/gtest.h>

// this is because the echoHandler uses the logger, should it do that?
#include "logger.hpp"
_SMERP::LogBackend logBack;

#define	MAX_STRING	128

// The fixture for testing class SMERP::Version
class VersionFixture : public ::testing::Test	{
protected:
	// You can remove any or all of the following functions if its body is empty.

	// Set-up work for each test here.
	VersionFixture()	{
		srand((unsigned)time(0));
		major = (unsigned short)( rand() % USHRT_MAX );
		minor = (unsigned short)( rand() % USHRT_MAX );
		revision = (unsigned short)( rand() % USHRT_MAX );
		build = (unsigned)( rand() % UINT_MAX );
		if ( !build )
			build++;

		ver = new _SMERP::Version( major, minor, revision, build );
		ver0 = new _SMERP::Version( major, minor, revision );
		sprintf( verStr, "%d.%d.%d.%u", major, minor, revision, build );

	}

	// Clean-up work that doesn't throw exceptions here.
	virtual ~VersionFixture()	{
		delete ver0;
		delete ver;
	}


	// If the constructor and destructor are not enough for setting up
	// and cleaning up each test, you can define the following methods:
	//   Code here will be called immediately after the constructor (right
	//   before each test).
	virtual void SetUp() {
	}

	//   Code here will be called immediately after each test (right
	//   before the destructor).
	virtual void TearDown() {
	}

	// Objects declared here can be used by all tests in the test case.
	unsigned short	major, minor, revision;
	unsigned	build;
	_SMERP::Version	*ver, *ver0;
	char		verStr[MAX_STRING];
};


// Tests the Version constructors and members
TEST_F( VersionFixture, Members )	{
	ASSERT_EQ( ver->major(), major );
	ASSERT_EQ( ver->minor(), minor );
	ASSERT_EQ( ver->revision(), revision );
	ASSERT_EQ( ver->build(), build );
	ASSERT_STREQ( ver->toString().c_str(), verStr );
}


// Tests the Version operators
TEST_F( VersionFixture, Operators )	{
	ASSERT_TRUE( *ver != *ver0 );
	ASSERT_TRUE( *ver > *ver0 );
	ASSERT_TRUE( *ver >= *ver0 );
	ASSERT_FALSE( *ver == *ver0 );
	ASSERT_FALSE( *ver < *ver0 );
	ASSERT_FALSE( *ver <= *ver0 );
}


int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
