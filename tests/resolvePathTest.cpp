//
// resolvePath unit tests
//

#include "miscUtils.hpp"
#include <gtest/gtest.h>


// Tests the Version constructors and members
TEST( ResolvePathFixture, tests )	{
	ASSERT_STREQ( resolvePath( "/./bla/./bla" ).c_str(), "/bla/bla" );
}


int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
