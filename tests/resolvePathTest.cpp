//
// resolvePath unit tests
//

#include "utils/fileUtils.hpp"
#include "gtest/gtest.h"
#include "wtest/testReport.hpp"

using namespace _Wolframe::utils;

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
	WOLFRAME_GTEST_REPORT( argv[0], refpath.string());
	return RUN_ALL_TESTS();
}
