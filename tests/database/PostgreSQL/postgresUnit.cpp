//
// testing the Module Loader
//

#include "logger-v1.hpp"
#include "processor/moduleDirectory.hpp"
#include "gtest/gtest.h"

#include "PostgreSQL.hpp"
#include "types/variant.hpp"

using namespace _Wolframe;
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

static void executeInsertStatements( Transaction* trans)
{
	{
		std::vector<types::Variant> values;
		values.push_back( 1);
		values.push_back( "xyz");
		values.push_back( true);
		trans->executeStatement( "INSERT INTO TestTest (id, name, active) VALUES ($1,$2,$3);", values);
	}
	{
		std::vector<types::Variant> values;
		values.push_back( 2);
		values.push_back( "abc");
		values.push_back( false);
		trans->executeStatement( "INSERT INTO TestTest (id, name, active) VALUES ($1,$2,$3);", values);
	}
	{
		std::vector<types::Variant> values;
		values.push_back( types::VariantConst( ));
		values.push_back( types::VariantConst( ));
		values.push_back( types::VariantConst( ));
		trans->executeStatement( "INSERT INTO TestTest (id, name, active) VALUES ($1,$2,$3);", values);
	}
}

TEST_F( PQmoduleFixture, Transaction )
{
	PostgreSQLdbUnit dbUnit( "testDB", "localhost", 0, "wolframe",
			     "wolfusr", "wolfpwd", "", "", "", "", "",
			     3, 4, 3, 10, std::list<std::string>());

	Database* db = dbUnit.database( );
	Transaction* trans = db->transaction( "test" );

	// ok transaction
	trans->begin( );
	trans->commit( );
	
	// rollback transaction
	trans->begin( );
	trans->rollback( );
	
	// error, commit without begin
	EXPECT_THROW( trans->commit( ), std::runtime_error );
	
	// error, rollback without begin
	EXPECT_THROW( trans->rollback( ), std::runtime_error );
	
	// ok transaction create table statement with commit
	trans->begin( );
	trans->executeStatement( "DROP TABLE IF EXISTS TestTest;");
	trans->executeStatement( "CREATE TABLE TestTest (id INTEGER, name TEXT, active BOOLEAN);");
	trans->commit( );

	// ok transaction with statements with rollback
	trans->begin( );
	executeInsertStatements( trans);
	trans->rollback( );

	// ok select result that must not exist because of the rollback in the previous transaction
	trans->begin( );
	Transaction::Result emptyres = trans->executeStatement( "SELECT * FROM TestTest;");
	trans->commit( );
	EXPECT_EQ( emptyres.size(), 0);

	// ok transaction with statements with commit
	trans->begin( );
	executeInsertStatements( trans);
	trans->commit( );

	// ok select result that must contain the elements inserted in the previous transaction
	trans->begin( );
	Transaction::Result res = trans->executeStatement( "SELECT * FROM TestTest ORDER BY id is null, id ASC;");
	trans->commit( );
	EXPECT_EQ( res.size(), 3);
	EXPECT_EQ( res.colnames().size(), 3);
	EXPECT_STREQ( "id", res.colnames().at(0).c_str());
	EXPECT_STREQ( "name", res.colnames().at(1).c_str());
	EXPECT_STREQ( "active", res.colnames().at(2).c_str());
	std::vector<Transaction::Result::Row>::const_iterator ri = res.begin(), re = res.end();
	for (types::Variant::Data::Int idx=1; ri!= re; ++ri,++idx)
	{
		if( idx == 3 ) {
			ASSERT_FALSE( ri->at(0).defined( ) );
			ASSERT_FALSE( ri->at(1).defined( ) );
			ASSERT_FALSE( ri->at(2).defined( ) );
		} else {
			EXPECT_EQ( idx, ri->at(0).toint());
			std::string name( ri->at(1).tostring());
			bool active( ri->at(2).tobool());
			EXPECT_STREQ( (idx==2?"abc":"xyz"), name.c_str());
			EXPECT_EQ( ( idx==2?false:true), active);
		}
	}

	trans->close( );
}

int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS( );
}
