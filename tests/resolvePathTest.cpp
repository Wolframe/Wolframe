//
// resolvePath unit tests
//

#include "miscUtils.hpp"
#include "gtest/gtest.h"


// Tests the Version constructors and members
TEST( ResolvePathFixture, tests )	{
#ifndef _WIN32
	ASSERT_STREQ( resolvePath( "/./bla/./bla" ).c_str(), "/bla/bla" );
#else
	// Aba: something is wrong here or in boost::filesystem!
	ASSERT_STREQ( resolvePath( "\\.\\bla\\.\\bla" ).c_str(), "/bla\\bla" );
#endif
}


int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
