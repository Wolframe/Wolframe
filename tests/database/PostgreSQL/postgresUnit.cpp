//
// testing the Module Loader
//

#include "logger-v1.hpp"
#include "processor/moduleDirectory.hpp"
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

TEST_F( PQmoduleFixture, CreatePostgreSQLunit )
{
	PostgreSQLdbUnit db( "testDB", "localhost", 0, "wolframe",
			     "wolfusr", "wolfpwd", "", "", "", "", "",
			     3, 4, 3, 10, std::list<std::string>());
	ASSERT_STREQ( "PostgreSQL", db.className());
	ASSERT_STREQ( "testDB", db.ID().c_str());
	ASSERT_STREQ( "testDB", db.database()->ID().c_str());
}

TEST_F( PQmoduleFixture, WrongHost )
{
	// Aba: questionable! should't this be an exception?
	PostgreSQLdbUnit db( "testDB", "blabla", 0, "wolframe",
			     "wolfusr", "wolfpwd", "", "", "", "", "",
			     3, 4, 3, 10, std::list<std::string>());
	ASSERT_STREQ( "PostgreSQL", db.className());
	ASSERT_STREQ( "testDB", db.ID().c_str());
	ASSERT_STREQ( "testDB", db.database()->ID().c_str());
}

TEST_F( PQmoduleFixture, WrongPassword )
{
	ASSERT_THROW( PostgreSQLdbUnit db( "testDB", "localhost", 0, "wolframe",
					   "wolfusr", "wolfpwdd", "", "", "", "", "",
					   3, 4, 3, 10, std::list<std::string>()), std::runtime_error );
}

TEST_F( PQmoduleFixture, WrongUser )
{
	ASSERT_THROW( PostgreSQLdbUnit db( "testDB", "localhost", 0, "wolframe",
					   "wolfusrr", "wolfpwd", "", "", "", "", "",
					   3, 4, 3, 10, std::list<std::string>()), std::runtime_error );
}

TEST_F( PQmoduleFixture, WrongDatabase )
{
	ASSERT_THROW( PostgreSQLdbUnit db( "testDB", "localhost", 0, "wolframee",
					   "wolfusr", "wolfpwd", "", "", "", "", "",
					   3, 4, 3, 10, std::list<std::string>()), std::runtime_error );
}

int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS( );
}
