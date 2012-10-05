//
// testing the Module Loader
//

#include "logger-v1.hpp"
#include "moduleDirectory.hpp"
#include "gtest/gtest.h"

#include "PostgreSQL.hpp"

using namespace _Wolframe::db;
using namespace _Wolframe::log;

_Wolframe::log::LogBackend*	logBackendPtr;

// The fixture for testing Wolframe module that log
class PQmoduleFixture : public ::testing::Test
{
	LogBackend& logBack;

	protected:
		PQmoduleFixture( ) :
			logBack( LogBackend::instance( ) )
		{
			logBack.setConsoleLevel( LogLevel::LOGLEVEL_DATA );
			logBackendPtr = &logBack;
		}
};

TEST_F( PQmoduleFixture, CreatePostgreSQLunit_0 )
{
	PostgreSQLdbUnit db( "testDB", "localhost", 0, "wolframe",
			     "wolfusr", "wolfpwd", "", "", "", "", "",
			     3, 4, 3, 10, "program" );
	ASSERT_STREQ( "PostgreSQL", db.className());
	ASSERT_STREQ( "testDB", db.ID().c_str());
	ASSERT_FALSE( db.loadProgram());
}

TEST_F( PQmoduleFixture, CreatePostgreSQLunit_1 )
{
	PostgreSQLdbUnit db( "testDB", "blabla", 0, "wolframe",
			     "wolfusr", "wolfpwd", "", "", "", "", "",
			     3, 4, 3, 10, "" );
	ASSERT_STREQ( "PostgreSQL", db.className());
	ASSERT_STREQ( "testDB", db.ID().c_str());
	ASSERT_TRUE( db.loadProgram());
}

TEST_F( PQmoduleFixture, WrongPassword )
{
	PostgreSQLdbUnit db( "testDB", "localhost", 0, "wolframe",
			     "wolfusr", "wolfpwdd", "", "", "", "", "",
			     3, 4, 3, 10, "" );
	ASSERT_STREQ( "PostgreSQL", db.className());
	ASSERT_STREQ( "testDB", db.ID().c_str());
	ASSERT_TRUE( db.loadProgram());
}

TEST_F( PQmoduleFixture, WrongUser )
{
	PostgreSQLdbUnit db( "testDB", "localhost", 0, "wolframe",
			     "wolfusrr", "wolfpwd", "", "", "", "", "",
			     3, 4, 3, 10, "" );
	ASSERT_STREQ( "PostgreSQL", db.className());
	ASSERT_STREQ( "testDB", db.ID().c_str());
	ASSERT_TRUE( db.loadProgram());
}

TEST_F( PQmoduleFixture, WrongDatabase )
{
	PostgreSQLdbUnit db( "testDB", "localhost", 0, "wolframee",
			     "wolfusr", "wolfpwd", "", "", "", "", "",
			     3, 4, 3, 10, "" );
	ASSERT_STREQ( "PostgreSQL", db.className());
	ASSERT_STREQ( "testDB", db.ID().c_str());
	ASSERT_TRUE( db.loadProgram());
}

int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS( );
}
