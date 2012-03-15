#include <gtest/gtest.h>

#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

TEST( BoostPathFixture, testAbsolute )
{
	std::string filename( "test.conf" );
	std::string filename2( "test.conf" );

	std::string configfile = boost::filesystem::complete( filename ).string( );
	std::string configfile2 = boost::filesystem::complete( filename2 ).string( );

	ASSERT_EQ( configfile, configfile2 );

#ifdef _WIN32
	std::string completeFilename = boost::filesystem::complete( filename, "C:\\DIR\\SOMEwhere" ).string( );
	ASSERT_EQ( completeFilename, "C:\\DIR\\SOMEwhere\\test.conf" );
#else
	std::string completeFilename = boost::filesystem::complete( filename, "/base/somewhere/" ).string( );
	ASSERT_EQ( completeFilename, "/base/somewhere/test.conf" );
#endif
}

TEST( BoostPathFixture, testIsAbsolute )
{
	using namespace boost::filesystem;

#if BOOST_VERSION >= 104300
	ASSERT_FALSE( path( "bla.log" ).is_complete( ) );
#ifdef _WIN32
	ASSERT_TRUE( path( "c:\\temp\\bla.log" ).is_complete( ) );
#else
	ASSERT_TRUE( path( "/tmp/bla.log" ).is_complete( ) );
#endif
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
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
