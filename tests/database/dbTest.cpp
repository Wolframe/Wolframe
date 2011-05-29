#include <gtest/gtest.h>

#ifdef WITH_SQLITE3
#include "database/SQLite.hpp"
#endif

using namespace _Wolframe::db;

class DbFixture : public ::testing::Test	{
protected:

	DbFixture( )
	{
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

int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
