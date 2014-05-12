//
// testing the Module Loader
//

#include "logger-v1.hpp"
#include "module/moduleDirectory.hpp"
#include "gtest/gtest.h"
#include "wtest/testReport.hpp"

#include "PostgreSQL.hpp"
#include "types/variant.hpp"
#include <stdexcept>

using namespace _Wolframe;
using namespace _Wolframe::db;
using namespace _Wolframe::log;

// The fixture for testing Wolframe module that log
class PQmoduleFixture : public ::testing::Test
{
	LogBackend& logBack;

	protected:
		PQmoduleFixture( ) :
			logBack( LogBackend::instance( ) )
		{
			logBack.setConsoleLevel( LogLevel::LOGLEVEL_DATA );
		}
};

TEST_F( PQmoduleFixture, CreatePostgreSQLunit )
{
	PostgreSQLdbUnit db( "testDB", "localhost", 0, "wolframe",
				"wolfusr", "wolfpwd", "", "", "", "", "",
				3, 4, 3, 30000, std::vector<std::string>());
	ASSERT_STREQ( "PostgreSQL", db.className());
	ASSERT_STREQ( "testDB", db.ID().c_str());
	ASSERT_STREQ( "testDB", db.database()->ID().c_str());
}

TEST_F( PQmoduleFixture, WrongHost )
{
	// Aba: questionable! should't this be an exception?
	PostgreSQLdbUnit db( "testDB", "blabla", 0, "wolframe",
				"wolfusr", "wolfpwd", "", "", "", "", "",
				3, 4, 3, 30000, std::vector<std::string>());
	ASSERT_STREQ( "PostgreSQL", db.className());
	ASSERT_STREQ( "testDB", db.ID().c_str());
	ASSERT_STREQ( "testDB", db.database()->ID().c_str());
}

TEST_F( PQmoduleFixture, WrongPassword )
{
	ASSERT_THROW( PostgreSQLdbUnit db( "testDB", "localhost", 0, "wolframe",
						"wolfusr", "wolfpwdd", "", "", "", "", "",
						3, 4, 3, 30000, std::vector<std::string>()), std::runtime_error );
}

TEST_F( PQmoduleFixture, WrongUser )
{
	ASSERT_THROW( PostgreSQLdbUnit db( "testDB", "localhost", 0, "wolframe",
						"wolfusrr", "wolfpwd", "", "", "", "", "",
						3, 4, 3, 30000, std::vector<std::string>()), std::runtime_error );
}

TEST_F( PQmoduleFixture, WrongDatabase )
{
	ASSERT_THROW( PostgreSQLdbUnit db( "testDB", "localhost", 0, "wolframee",
						"wolfusr", "wolfpwd", "", "", "", "", "",
						3, 4, 3, 30000, std::vector<std::string>()), std::runtime_error );
}

TEST_F( PQmoduleFixture, Transaction )
{
	PostgreSQLdbUnit dbUnit( "testDB", "localhost", 0, "wolframe",
				"wolfusr", "wolfpwd", "", "", "", "", "",
				3, 4, 3, 30000, std::vector<std::string>());

	Database* db = dbUnit.database( );
	TransactionR trans( db->transaction( "test" ));

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
}

static std::string dberror_string( const TransactionR& trans)
{
	const DatabaseError* err = trans->getLastError();
	return std::string("error in database: '") + err->dbname + "' error class '" + err->errorclass + ": " + err->errormsg + " " + err->errordetail;
}

static void executeInsertStatements( const TransactionR& trans)
{
	// some normal values
	{
		std::vector<types::Variant> values;
		values.push_back( 1);
		values.push_back( "xyz");
		values.push_back( true);
		values.push_back( 4.782 );
		if (!trans->executeStatement( "INSERT INTO TestTest (id, name, active, price) VALUES ($1,$2,$3,$4)", values)) throw std::runtime_error( dberror_string( trans));
	}
	{
		std::vector<types::Variant> values;
		values.push_back( 2);
		values.push_back( "abc");
		values.push_back( false);
		values.push_back( -4.2344 );
		if (!trans->executeStatement( "INSERT INTO TestTest (id, name, active, price) VALUES ($1,$2,$3,$4)", values)) throw std::runtime_error( dberror_string( trans));
	}
	// some maxima
	{
		std::vector<types::Variant> values;
		values.push_back( _WOLFRAME_MAX_INTEGER );
		values.push_back( "");
		values.push_back( false);
		// temporary: storing max( ) in a double precision returns a value which
		// is bigger than max() when converting it from a string..
		values.push_back( std::numeric_limits<double>::max( ) / 2 );
		if (!trans->executeStatement( "INSERT INTO TestTest (id, name, active, price) VALUES ($1,$2,$3,$4)", values)) throw std::runtime_error( dberror_string( trans));
	}
	// some minima
	{
		std::vector<types::Variant> values;
		values.push_back( _WOLFRAME_MIN_INTEGER );
		values.push_back( "");
		values.push_back( false);
		values.push_back( std::numeric_limits<double>::min( ) / 2 );
		if (!trans->executeStatement( "INSERT INTO TestTest (id, name, active, price) VALUES ($1,$2,$3,$4)", values)) throw std::runtime_error( dberror_string( trans));
	}
	// null values
	{
		std::vector<types::Variant> values;
		values.push_back( types::VariantConst( ));
		values.push_back( types::VariantConst( ));
		values.push_back( types::VariantConst( ));
		values.push_back( types::VariantConst( ));
		if (!trans->executeStatement( "INSERT INTO TestTest (id, name, active, price) VALUES ($1,$2,$3,$4)", values)) throw std::runtime_error( dberror_string( trans));
	}
}

TEST_F( PQmoduleFixture, ExecuteInstruction )
{
	PostgreSQLdbUnit dbUnit( "testDB", "localhost", 0, "wolframe",
				"wolfusr", "wolfpwd", "", "", "", "", "",
				3, 4, 3, 30000, std::vector<std::string>());
	Database* db = dbUnit.database( );
	TransactionR trans( db->transaction( "test" ));

	// ok transaction create table statement with commit
	trans->begin( );
	if (!trans->executeStatement( "DROP TABLE IF EXISTS TestTest")) throw std::runtime_error( dberror_string( trans));
	if (!trans->executeStatement( "CREATE TABLE TestTest (id BIGINT, name TEXT, active BOOLEAN, price DOUBLE PRECISION)")) throw std::runtime_error( dberror_string( trans));
	trans->commit( );

	// ok transaction with statements with rollback
	trans->begin( );
	executeInsertStatements( trans);
	trans->rollback( );

	// ok select result that must not exist because of the rollback in the previous transaction
	trans->begin( );
	Transaction::Result emptyres;
	if (!trans->executeStatement( emptyres, "SELECT * FROM TestTest")) throw std::runtime_error( dberror_string( trans));
	trans->commit( );
	EXPECT_EQ( emptyres.size(), 0U);

	// ok transaction with statements with commit
	trans->begin( );
	executeInsertStatements( trans);
	trans->commit( );

	// ok select result that must contain the elements inserted in the previous transaction
	trans->begin( );
	Transaction::Result res;
	if (!trans->executeStatement( res, "SELECT * FROM TestTest ORDER BY id IS NULL, id ASC")) throw std::runtime_error( dberror_string( trans));
	trans->commit( );
	EXPECT_EQ( res.size(), 5U);
	EXPECT_EQ( res.colnames().size(), 4U);
	EXPECT_STREQ( "id", res.colnames().at(0).c_str());
	EXPECT_STREQ( "name", res.colnames().at(1).c_str());
	EXPECT_STREQ( "active", res.colnames().at(2).c_str());
	EXPECT_STREQ( "price", res.colnames().at(3).c_str());
	std::vector<Transaction::Result::Row>::const_iterator ri = res.begin(), re = res.end();
	for (types::Variant::Data::Int idx=1; ri!= re; ++ri,++idx)
	{
		switch( idx ) {
			case 1: {
				ASSERT_EQ( ri->at(0).type(), types::Variant::Int);
				ASSERT_EQ( ri->at(1).type(), types::Variant::String);
				ASSERT_EQ( ri->at(2).type(), types::Variant::Bool);
				ASSERT_EQ( ri->at(3).type(), types::Variant::Double);
				EXPECT_EQ( _WOLFRAME_MIN_INTEGER, ri->at(0).toint());
				double price( ri->at(3).todouble());
				ASSERT_DOUBLE_EQ( std::numeric_limits<double>::min( ) / 2, price );
				break;
			}

			case 2:
			case 3: {
				ASSERT_EQ( ri->at(0).type(), types::Variant::Int);
				ASSERT_EQ( ri->at(1).type(), types::Variant::String);
				ASSERT_EQ( ri->at(2).type(), types::Variant::Bool);
				ASSERT_EQ( ri->at(3).type(), types::Variant::Double);
				EXPECT_EQ( idx-1, ri->at(0).toint());
				std::string name( ri->at(1).tostring());
				bool active( ri->at(2).tobool());
				double price( ri->at(3).todouble());
				EXPECT_STREQ( (idx==3?"abc":"xyz"), name.c_str());
				EXPECT_EQ( ( idx==3?false:true), active);
				ASSERT_DOUBLE_EQ( ( idx==3?-4.2344:4.782), price);
				break;
			}

			case 4: {
				ASSERT_EQ( ri->at(0).type(), types::Variant::Int);
				ASSERT_EQ( ri->at(1).type(), types::Variant::String);
				ASSERT_EQ( ri->at(2).type(), types::Variant::Bool);
				ASSERT_EQ( ri->at(3).type(), types::Variant::Double);
				EXPECT_EQ( (_WOLFRAME_UINTEGER)_WOLFRAME_MAX_INTEGER, ri->at(0).touint());
				double price( ri->at(3).todouble());
				ASSERT_DOUBLE_EQ( std::numeric_limits<double>::max( ) / 2, price );
				break;
			}

			case 5: {
				ASSERT_EQ( ri->at(0).type(), types::Variant::Null);
				ASSERT_EQ( ri->at(1).type(), types::Variant::Null);
				ASSERT_EQ( ri->at(2).type(), types::Variant::Null);
				ASSERT_EQ( ri->at(3).type(), types::Variant::Null);
				ASSERT_FALSE( ri->at(0).defined( ) );
				ASSERT_FALSE( ri->at(1).defined( ) );
				ASSERT_FALSE( ri->at(2).defined( ) );
				ASSERT_FALSE( ri->at(3).defined( ) );
				break;
			}
		}
	}
}

TEST_F( PQmoduleFixture, ExceptionSyntaxError )
{
	PostgreSQLdbUnit dbUnit( "testDB", "localhost", 0, "wolframe",
				"wolfusr", "wolfpwd", "", "", "", "", "",
				3, 4, 3, 30000, std::vector<std::string>());
	Database* db = dbUnit.database( );
	TransactionR trans( db->transaction( "test" ));

	trans->begin( );

	// execute an illegal SQL statement, must throw
	if (!trans->executeStatement( "SELCT 1" ))
	{
		const DatabaseError* err = trans->getLastError();
		ASSERT_EQ( err->errorclass, "SYNTAX" );
		std::cout << dberror_string( trans) << std::endl;
	}
	else
	{
		FAIL( ) << "Statement with illegal syntax should fail but doesn't!";
	}
	//... auto rollback
}

TEST_F( PQmoduleFixture, TooFewBindParameter )
{
	PostgreSQLdbUnit dbUnit( "testDB", "localhost", 0, "wolframe",
				"wolfusr", "wolfpwd", "", "", "", "", "",
				3, 4, 3, 30000, std::vector<std::string>());
	Database* db = dbUnit.database( );
	TransactionR trans( db->transaction( "test" ));

	trans->begin( );
	trans->executeStatement( "DROP TABLE IF EXISTS TestTest");
	trans->executeStatement( "CREATE TABLE TestTest (id INTEGER, name TEXT, active BOOLEAN, price REAL)");
	std::vector<types::Variant> values;
	values.push_back( 1);
	values.push_back( "xyz");
	values.push_back( true);

	// intentionally ommiting values here, must throw an error
	if (!trans->executeStatement( "INSERT INTO TestTest (id, name, active, price) VALUES ($1,$2,$3,$4)", values))
	{
		std::cout << dberror_string( trans) << std::endl;
	}
	else
	{
		// should actually not work
		trans->rollback();
		FAIL( ) << "Reached success state, but should fail!";
	}

	//... auto rollback
}

TEST_F( PQmoduleFixture, TooManyBindParameter )
{
	PostgreSQLdbUnit dbUnit( "testDB", "localhost", 0, "wolframe",
				"wolfusr", "wolfpwd", "", "", "", "", "",
				3, 4, 3, 30000, std::vector<std::string>());
	Database* db = dbUnit.database( );
	TransactionR trans( db->transaction( "test" ));

	trans->begin( );
	trans->executeStatement( "DROP TABLE IF EXISTS TestTest");
	trans->executeStatement( "CREATE TABLE TestTest (id INTEGER, name TEXT, active BOOLEAN, price REAL)");
	std::vector<types::Variant> values;
	values.push_back( 1);
	values.push_back( "xyz");
	values.push_back( true);
	values.push_back( 4.782);
	values.push_back( "too much");
	// intentionally adding too many values here, must return an error

	if (trans->executeStatement( "INSERT INTO TestTest (id, name, active, price) VALUES ($1,$2,$3,$4)", values))
	{
		// we should not get here, just in case we close the transaction properly
		trans->rollback();
		FAIL( ) << "Reached success state, but should fail!";
	}
	else
	{
		std::cout << dberror_string( trans) << std::endl;
	}
	//... auto rollback
}

TEST_F( PQmoduleFixture, IllegalBindParameter )
{
	PostgreSQLdbUnit dbUnit( "testDB", "localhost", 0, "wolframe",
				"wolfusr", "wolfpwd", "", "", "", "", "",
				3, 4, 3, 30000, std::vector<std::string>());
	Database* db = dbUnit.database( );
	TransactionR trans( db->transaction( "test" ));

	trans->begin( );
	std::vector<types::Variant> values;
	trans->executeStatement( "DROP TABLE IF EXISTS TestTest");
	trans->executeStatement( "CREATE TABLE TestTest (id INTEGER, name TEXT, active BOOLEAN, price REAL)");
	values.push_back( 1);
	values.push_back( "xyz");
	values.push_back( "not used");
	values.push_back( true);
	values.push_back( 4.782);

	if (trans->executeStatement( "INSERT INTO TestTest (id, name, active, price) VALUES ($1,$2,$4,$5)", values))
	{
		// should actually not work
		trans->rollback( );
		FAIL( ) << "Reached success state, but should fail!";
	}
	else
	{
		std::cout << dberror_string( trans) << std::endl;
	}
	//... auto rollback
}

TEST_F( PQmoduleFixture, ReusedBindParameter )
{
	PostgreSQLdbUnit dbUnit( "testDB", "localhost", 0, "wolframe",
				"wolfusr", "wolfpwd", "", "", "", "", "",
				3, 4, 3, 30000, std::vector<std::string>());
	Database* db = dbUnit.database( );
	TransactionR trans( db->transaction( "test" ));

	trans->begin( );
	if (!trans->executeStatement( "DROP TABLE IF EXISTS TestTest")) throw std::runtime_error( dberror_string( trans));
	if (!trans->executeStatement( "CREATE TABLE TestTest (id INTEGER, id2 INTEGER, id3 INTEGER)")) throw std::runtime_error( dberror_string( trans));
	std::vector<types::Variant> values;
	values.push_back( 47);
	if (!trans->executeStatement( "INSERT INTO TestTest (id, id2, id3) VALUES ($1,$1,$1)", values)) throw std::runtime_error( dberror_string( trans));
	trans->commit( );

	trans->begin( );
	Transaction::Result res;
	if (!trans->executeStatement( res, "SELECT * FROM TestTest")) throw std::runtime_error( dberror_string( trans));
	EXPECT_EQ( res.size(), 1U);
	EXPECT_EQ( res.colnames().size(), 3U);
	EXPECT_STREQ( "id", res.colnames().at(0).c_str());
	EXPECT_STREQ( "id2", res.colnames().at(1).c_str());
	EXPECT_STREQ( "id3", res.colnames().at(2).c_str());
	std::vector<Transaction::Result::Row>::const_iterator ri = res.begin(), re = res.end();
	for (types::Variant::Data::Int idx=1; ri!= re; ++ri,++idx)
	{
		ASSERT_EQ( ri->at(0).type(), types::Variant::Int);
		ASSERT_EQ( ri->at(1).type(), types::Variant::Int);
		ASSERT_EQ( ri->at(2).type(), types::Variant::Int);
		EXPECT_EQ( 47, ri->at(0).toint());
		EXPECT_EQ( 47, ri->at(1).toint());
		EXPECT_EQ( 47, ri->at(2).toint());
	}
	trans->commit( );
}

TEST_F( PQmoduleFixture, ExpressionWithParametersAndTypeCoercion )
{
	PostgreSQLdbUnit dbUnit( "testDB", "localhost", 0, "wolframe",
				"wolfusr", "wolfpwd", "", "", "", "", "",
				3, 4, 3, 30000, std::vector<std::string>());
	Database* db = dbUnit.database( );
	TransactionR trans( db->transaction( "test" ));

	trans->begin( );
	if (!trans->executeStatement( "DROP TABLE IF EXISTS TestTest")) throw std::runtime_error( dberror_string( trans));
	if (!trans->executeStatement( "CREATE TABLE TestTest (id INTEGER, id2 INTEGER, id3 INTEGER)")) throw std::runtime_error( dberror_string( trans));
	std::vector<types::Variant> values;
	values.push_back( std::string("47"));
	values.push_back( 47);
	values.push_back( 47);
	if (!trans->executeStatement( "INSERT INTO TestTest (id, id2, id3) VALUES ($1,$2+1,$3+2)", values)) throw std::runtime_error( dberror_string( trans));
	trans->commit( );

	trans->begin( );
	Transaction::Result res;
	if (!trans->executeStatement( res, "SELECT * FROM TestTest")) throw std::runtime_error( dberror_string( trans));
	EXPECT_EQ( res.size(), 1U);
	EXPECT_EQ( res.colnames().size(), 3U);
	EXPECT_STREQ( "id", res.colnames().at(0).c_str());
	EXPECT_STREQ( "id2", res.colnames().at(1).c_str());
	EXPECT_STREQ( "id3", res.colnames().at(2).c_str());
	std::vector<Transaction::Result::Row>::const_iterator ri = res.begin(), re = res.end();
	for (types::Variant::Data::Int idx=1; ri!= re; ++ri,++idx)
	{
		ASSERT_EQ( ri->at(0).type(), types::Variant::Int);
		ASSERT_EQ( ri->at(1).type(), types::Variant::Int);
		ASSERT_EQ( ri->at(2).type(), types::Variant::Int);
		EXPECT_EQ( 47, ri->at(0).toint());
		EXPECT_EQ( 48, ri->at(1).toint());
		EXPECT_EQ( 49, ri->at(2).toint());
	}
	trans->commit( );
}

int main( int argc, char **argv )
{
	WOLFRAME_GTEST_REPORT( argv[0], refpath.string());
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS( );
}
