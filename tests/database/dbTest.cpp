#include <gtest/gtest.h>

#include "logger-v1.hpp"
#include "database/database.hpp"

// not happy here!
#ifdef WITH_SQLITE3
#include "database/SQLite.hpp"
#endif

#ifdef WITH_PGSQL
#include "database/PostgreSQL.hpp"
#endif

using namespace _Wolframe::log;
using namespace _Wolframe::db;

class DbFixture : public ::testing::Test	{
protected:
	LogBackend& logBack;

	DbFixture( )
		: logBack( LogBackend::instance( ) )
	{
		logBack.setLogfileLevel( LogLevel::LOGLEVEL_DATA );
		logBack.setLogfileName( "dbTest.log" );
	}

	virtual ~DbFixture( )
	{
	}

	virtual void SetUp( )
	{
	}

	virtual void TearDown( )
	{
	}
};


#ifdef WITH_SQLITE3
TEST_F( DbFixture, Sqlite3 )
{
	SQLiteDatabase db( "test", "test.db", 1, true );
	ASSERT_STREQ( db.typeName( ), "SQLite" );
}
#endif

#ifdef WITH_PGSQL
TEST_F( DbFixture, Postgresql )
{
	PostgreSQLdatabase db( "test", "localhost", 5432, "test", "test", "xx", 10, 10, 10 );
	ASSERT_STREQ( db.typeName( ), "PostgreSQL" );
}
#endif

int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
