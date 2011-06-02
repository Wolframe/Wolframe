#include "logger.hpp"

#include <gtest/gtest.h>

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
	SQLiteConfig conf( "test.db", "test", "db" );
	SQLiteDatabase db( &conf );
	assert( db.type( ) == DBTYPE_SQLITE );
}
#endif

#ifdef WITH_PGSQL
TEST_F( DbFixture, Postgresql )
{
	PostgreSQLconfig conf( "wrong", "test", "db" );
	PostgreSQLDatabase db( &conf );
	assert( db.type( ) == DBTYPE_POSTGRESQL );
}
#endif

int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
