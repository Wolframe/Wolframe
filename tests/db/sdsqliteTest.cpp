//
// db sqlite3 unit tests
//

#include <climits>
#include <cstdlib>
#include <cstdio>
#include <ctime>

#include "version.hpp"
#include <gtest/gtest.h>

// test needs 'unlink' from 'unistd.h'
#if !defined _WIN32
#include <unistd.h>
#endif // !defined _WIN32

#include "sdsqlite.h"

#include <iostream>

// The fixture for testing class sdsqlite
class DbSqliteFixture : public ::testing::Test {
protected:
	// Set-up work for each test here.
	DbSqliteFixture( )
	{
	}

	// Clean-up work that doesn't throw exceptions here.
	virtual ~DbSqliteFixture( )
	{
	}

	// If the constructor and destructor are not enough for setting up
	// and cleaning up each test, you can define the following methods:
	//   Code here will be called immediately after the constructor (right
	//   before each test).
	virtual void SetUp( )
	{
		(void)unlink( "test.db" );
		db.open( "test.db" );
	}

	//   Code here will be called immediately after each test (right
	//   before the destructor).
	virtual void TearDown( )
	{
		db.close( );
		(void)unlink( "test.db" );
	}

	// Objects declared here can be used by all tests in the test case.	
	sd::sqlite db;
};

// Tests the Version constructors and members
TEST_F( DbSqliteFixture, BasicOps )
{
	db << "create table a(b integer)";

	sd::sql ins( db );
	ins << "insert into a(b) values(?)";
	int data[4] = { 34, 22, 5, 54 };
	db << "begin transaction";
	for( int i = 0; i < 4; i++ ) {
		ins << data[i];
	}
	db << "commit transaction";

	sd::sql q( db );
	q << "select b from a where b >? order by b" << 7;
	int count = 0;
	int b[5] = { 0, 0, 0, 0, 0};
	while( count < 5 && q.step( ) ) {
		q >> b[count];
		count++;
	}

	ASSERT_EQ( count, 3 );
	if( count > 0 ) {
		ASSERT_EQ( b[0], 22 );
		ASSERT_EQ( b[1], 34 );
		ASSERT_EQ( b[2], 54 );
	}
}

int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );

	return RUN_ALL_TESTS();
}

