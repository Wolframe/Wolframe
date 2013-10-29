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

#define HOST "andreasbaumann.dyndns.org"
#define SID "orcl"
#define USER "wolfusr"
#define PWD "wolfpwd"

TEST_F( OracleFixture, CreateOracleunit )
{
	OracledbUnit db( "testDB", HOST, 0, SID, USER, PWD,
			     "", "", "", "", "",
			     3, 4, 3, 10, std::list<std::string>());
	ASSERT_STREQ( "Oracle", db.className());
	ASSERT_STREQ( "testDB", db.ID().c_str());
	ASSERT_STREQ( "testDB", db.database()->ID().c_str());
}

TEST_F( OracleFixture, WrongHost )
{
	ASSERT_THROW( OracledbUnit db( "testDB", "blabla", 0, SID, USER, PWD,
			     "", "", "", "", "",
			     3, 4, 3, 10, std::list<std::string>()), std::runtime_error );
}

TEST_F( OracleFixture, WrongPassword )
{
	ASSERT_THROW( OracledbUnit db( "testDB", HOST, 0, SID, USER, PWD "d",
					   "", "", "", "", "",
					   3, 4, 3, 10, std::list<std::string>()), std::runtime_error );
}

TEST_F( OracleFixture, WrongUser )
{
	ASSERT_THROW( OracledbUnit db( "testDB", HOST, 0, SID, USER "r", PWD,
					   "", "", "", "", "",
					   3, 4, 3, 10, std::list<std::string>()), std::runtime_error );
}

TEST_F( OracleFixture, WrongDatabase )
{
	ASSERT_THROW( OracledbUnit db( "testDB", HOST, 0, SID "e", USER, PWD,
					   "", "", "", "", "",
					   3, 4, 3, 10, std::list<std::string>()), std::runtime_error );
}

int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS( );
}
