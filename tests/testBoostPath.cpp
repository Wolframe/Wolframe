#include "gtest/gtest.h"
#include "wtest/testReport.hpp"

#include <boost/filesystem.hpp>

TEST( BoostPathFixture, testAbsolute )
{
	std::string filename = "test.conf";

#ifdef _WIN32
	std::string completeFilename = boost::filesystem::absolute( filename, "C:\\DIR\\SOMEwhere" ).string( );
	ASSERT_EQ( completeFilename, "C:\\DIR\\SOMEwhere\\test.conf" );
#else
	std::string completeFilename = boost::filesystem::absolute( filename, "/base/somewhere/" ).string( );
	ASSERT_EQ( completeFilename, "/base/somewhere/test.conf" );
#endif
}

TEST( BoostPathFixture, testIsAbsolute )
{
	using namespace boost::filesystem;

	ASSERT_FALSE( path( "bla.log" ).is_complete( ) );
#ifdef _WIN32
	ASSERT_TRUE( path( "c:\\temp\\bla.log" ).is_complete( ) );
#else
	ASSERT_TRUE( path( "/tmp/bla.log" ).is_complete( ) );
#endif
	ASSERT_FALSE( path( "bla.log" ).is_complete( ) );
#ifdef _WIN32
	ASSERT_TRUE( path( "C:\\temp\\bla.log" ).is_complete( ) );
#else
	ASSERT_TRUE( path( "/tmp/bla.log" ).is_complete( ) );
#endif
}

int main( int argc, char **argv )
{
	WOLFRAME_GTEST_REPORT( argv[0], refpath.string());
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
