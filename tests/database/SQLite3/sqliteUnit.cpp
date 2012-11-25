//
// testing the Module Loader
//

#include "logger-v1.hpp"
#include "moduleDirectory.hpp"
#include "gtest/gtest.h"

#include "SQLite.hpp"

using namespace _Wolframe::db;
using namespace _Wolframe::log;

_Wolframe::log::LogBackend*	logBackendPtr;

// The fixture for testing Wolframe module that log
class SQLiteModuleFixture : public ::testing::Test
{
	LogBackend& logBack;

	protected:
		SQLiteModuleFixture( ) :
			logBack( LogBackend::instance( ) )
		{
			logBack.setConsoleLevel( LogLevel::LOGLEVEL_DATA );
			logBackendPtr = &logBack;
		}
};

TEST_F( SQLiteModuleFixture, CreateSQLiteUnit_0 )
{
	SQLiteDBunit db( "testDB", "test.db", 3 );
	ASSERT_STREQ( "SQLite", db.className());
	ASSERT_STREQ( "testDB", db.ID().c_str());
	ASSERT_STREQ( "testDB", db.database()->ID().c_str());
}

TEST_F( SQLiteModuleFixture, CreateSQLiteUnit_1 )
{
	SQLiteDBunit db( "testDB", "test.db", 3 );
	ASSERT_STREQ( "SQLite", db.className());
	ASSERT_STREQ( "testDB", db.ID().c_str());
	ASSERT_STREQ( "testDB", db.database()->ID().c_str());
}

TEST_F( SQLiteModuleFixture, OpenGarbage )
{
	ASSERT_THROW( SQLiteDBunit db( "testDB", "garbage.db", 3 ), std::runtime_error );
}

int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS( );
}
