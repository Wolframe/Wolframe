//
// testing the Module Loader
//

#include "logger-v1.hpp"
#include "processor/moduleDirectory.hpp"
#include "gtest/gtest.h"

#include "Oracle.hpp"
#include "types/variant.hpp"

using namespace _Wolframe;
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

TEST_F( OracleFixture, CreateOracleUnit )
{
	OracleDbUnit db( "testDB", "andreasbaumann.dyndns.org", 0, "orcl",
			     "wolfusr", "wolfpwd", "", "", "", "", "",
			     3, 4, 3, 10, std::list<std::string>());
	ASSERT_STREQ( "Oracle", db.className());
	ASSERT_STREQ( "testDB", db.ID().c_str());
	ASSERT_STREQ( "testDB", db.database()->ID().c_str());
}

TEST_F( OracleFixture, WrongHost )
{
	ASSERT_THROW( OracleDbUnit db( "testDB", "blabla", 0, "orcl",
	                     "wolfusr", "wolfpwd", "", "", "", "", "",
			     3, 4, 3, 10, std::list<std::string>()), std::runtime_error );
}

TEST_F( OracleFixture, WrongPassword )
{
	ASSERT_THROW( OracleDbUnit db( "testDB", "andreasbaumann.dyndns.org", 0, "orcl",
					   "wolfusr", "wolfpwdd", "", "", "", "", "",
					   3, 4, 3, 10, std::list<std::string>()), std::runtime_error );
}

TEST_F( OracleFixture, WrongUser )
{
	ASSERT_THROW( OracleDbUnit db( "testDB", "andreasbaumann.dyndns.org", 0, "orcl",
					   "wolfusrr", "wolfpwd", "", "", "", "", "",
					   3, 4, 3, 10, std::list<std::string>()), std::runtime_error );
}

TEST_F( OracleFixture, WrongDatabase )
{
	ASSERT_THROW( OracleDbUnit db( "testDB", "andreasbaumann.dyndns.org", 0, "orcle",
					   "wolfusr", "wolfpwd", "", "", "", "", "",
					   3, 4, 3, 10, std::list<std::string>()), std::runtime_error );
}

static void executeInsertStatements( Transaction* trans)
{
	{
		std::vector<types::Variant> values;
		values.push_back( 1);
		values.push_back( "xyz");
		trans->executeStatement( "INSERT INTO TestTest (id, name) VALUES ($1,$2)", values);
	}
	{
		std::vector<types::Variant> values;
		values.push_back( 2);
		values.push_back( "abc");
		trans->executeStatement( "INSERT INTO TestTest (id, name) VALUES ($1,$2)", values);
	}
}

TEST_F( OracleFixture, Transaction )
{
	OracleDbUnit dbUnit( "testDB", "andreasbaumann.dyndns.org", 0, "orcl",
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
	trans->executeStatement( "begin execute immediate 'drop table TestTest'; exception when others then null; end;");
	trans->executeStatement( "CREATE TABLE TestTest (id INTEGER, name VARCHAR(64))");
	trans->commit( );

	// ok transaction with statements with rollback
	trans->begin( );
	executeInsertStatements( trans);
	trans->rollback( );

	// ok select result that must not exist because of the rollback in the previous transaction
	trans->begin( );
	Transaction::Result emptyres = trans->executeStatement( "SELECT * FROM TestTest");
	trans->commit( );
	EXPECT_EQ( emptyres.size(), 0);

	// ok transaction with statements with commit
	trans->begin( );
	executeInsertStatements( trans);
	trans->commit( );

	// ok select result that must contain the elements inserted in the previous transaction
	trans->begin( );
	Transaction::Result res = trans->executeStatement( "SELECT * FROM TestTest ORDER BY id ASC");
	trans->commit( );
	EXPECT_EQ( res.size(), 2);
	EXPECT_EQ( res.colnames().size(), 2);
	EXPECT_STREQ( "ID", res.colnames().at(0).c_str());
	EXPECT_STREQ( "NAME", res.colnames().at(1).c_str());
	std::vector<Transaction::Result::Row>::const_iterator ri = res.begin(), re = res.end();
	for (types::Variant::Data::Int idx=1; ri!= re; ++ri,++idx)
	{
		EXPECT_EQ( idx, ri->at(0).toint());
		std::string name( ri->at(1).tostring());
		EXPECT_STREQ( (idx==2?"abc":"xyz"), name.c_str());
	}

	trans->close( );
}

int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS( );
}
