//
// testing the Module Loader
//

#include "logger-v1.hpp"
#include "processor/moduleDirectory.hpp"
#include "gtest/gtest.h"

#include "Oracle.hpp"

using namespace _Wolframe::db;
using namespace _Wolframe::log;

_Wolframe::log::LogBackend*	logBackendPtr;

// The fixture for testing Wolframe module that log
class OracleFixture : public ::testing::Test
{
	LogBackend& logBack;

	protected:
		OracleFixture( ) :
			logBack( LogBackend::instance( ) )
		{
			logBack.setConsoleLevel( LogLevel::LOGLEVEL_DATA );
			logBackendPtr = &logBack;
		}
};

TEST_F( OracleFixture, CreateOracleunit )
{
	OracledbUnit db( "testDB", "euroserver", 0, "orcl",
			     "wolfusr", "wolfpwd", "", "", "", "", "",
			     3, 4, 3, 10, std::list<std::string>());
	ASSERT_STREQ( "Oracle", db.className());
	ASSERT_STREQ( "testDB", db.ID().c_str());
	ASSERT_STREQ( "testDB", db.database()->ID().c_str());
}

TEST_F( OracleFixture, WrongHost )
{
	ASSERT_THROW( OracledbUnit db( "testDB", "blabla", 0, "orcl",
	                     "wolfusr", "wolfpwd", "", "", "", "", "",
			     3, 4, 3, 10, std::list<std::string>()), std::runtime_error );
}

TEST_F( OracleFixture, WrongPassword )
{
	ASSERT_THROW( OracledbUnit db( "testDB", "euroserver", 0, "orcl",
					   "wolfusr", "wolfpwdd", "", "", "", "", "",
					   3, 4, 3, 10, std::list<std::string>()), std::runtime_error );
}

TEST_F( OracleFixture, WrongUser )
{
	ASSERT_THROW( OracledbUnit db( "testDB", "euroserver", 0, "orcl",
					   "wolfusrr", "wolfpwd", "", "", "", "", "",
					   3, 4, 3, 10, std::list<std::string>()), std::runtime_error );
}

TEST_F( OracleFixture, WrongDatabase )
{
	ASSERT_THROW( OracledbUnit db( "testDB", "euroserver", 0, "orcle",
					   "wolfusr", "wolfpwd", "", "", "", "", "",
					   3, 4, 3, 10, std::list<std::string>()), std::runtime_error );
}

int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS( );
}
