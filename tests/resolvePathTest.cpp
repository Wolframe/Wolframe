
//
// resolvePath unit tests
//

#include <climits>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <stdio.h>

#include "miscUtils.hpp"
#include <gtest/gtest.h>


#define	MAX_STRING	128


class ResolvePathFixture : public ::testing::Test	{
};


// Tests the Version constructors and members
TEST_F( ResolvePathFixture, tests )	{
	ASSERT_STREQ( resolvePath( "/./bla/./bla" ).c_str(), "/bla/bla" );
}


int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
