#include "gtest/gtest.h"

#define BOOST_FILESYSTEM_VERSION 3
#include <boost/version.hpp>
#include <boost/filesystem.hpp>

TEST( BoostPathFixture, testAbsolute )
{
	std::string filename = "test.conf";

#ifdef _WIN32
#if BOOST_VERSION < 104300
	std::string completeFilename = boost::filesystem::complete( filename, "C:\\DIR\\SOMEwhere" ).string( );
#else
	std::string completeFilename = boost::filesystem::absolute( filename, "C:\\DIR\\SOMEwhere" ).string( );
#endif
	ASSERT_EQ( completeFilename, "C:\\DIR\\SOMEwhere\\test.conf" );
#else
#if BOOST_VERSION < 104300
	std::string completeFilename = boost::filesystem::complete( filename, "/base/somewhere/" ).string( );
#else
	std::string completeFilename = boost::filesystem::absolute( filename, "/base/somewhere/" ).string( );
#endif
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
