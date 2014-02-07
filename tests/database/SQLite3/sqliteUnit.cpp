//
// testing the Module Loader
//

#include "logger-v1.hpp"
#include "processor/moduleDirectory.hpp"
#include "types/variant.hpp"
#include "types/integer.hpp"
#include "gtest/gtest.h"

#include "SQLite.hpp"
#include <vector>
#include <string>
#include <limits>

using namespace _Wolframe;
using namespace _Wolframe::db;
using namespace _Wolframe::log;
using namespace _Wolframe::UI;


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
	SQLiteDBunit db( "testDB", "test.db", true, false, 3,
			 std::list<std::string>(), std::list<std::string>() );
	EXPECT_STREQ( "SQLite", db.className());
	EXPECT_STREQ( "testDB", db.ID().c_str());
	EXPECT_STREQ( "testDB", db.database()->ID().c_str());
}

TEST_F( SQLiteModuleFixture, CreateSQLiteUnit_1 )
{
	SQLiteDBunit db( "testDB", "test.db", true, false, 3,
			 std::list<std::string>(), std::list<std::string>() );
	EXPECT_STREQ( "SQLite", db.className());
	EXPECT_STREQ( "testDB", db.ID().c_str());
	EXPECT_STREQ( "testDB", db.database()->ID().c_str());
}

TEST_F( SQLiteModuleFixture, OpenGarbage )
{
	EXPECT_THROW( SQLiteDBunit db( "testDB", "garbage.db", true, false, 3,
				       std::list<std::string>(), std::list<std::string>() ),
		      std::runtime_error );
}

TEST_F( SQLiteModuleFixture, Transaction )
{
	SQLiteDBunit dbUnit( "testDB", "test.db", true, false, 3,
			     std::list<std::string>(), std::list<std::string>() );

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
	
	trans->close( );
}

static void executeInsertStatements( Transaction* trans)
{
	// some normal values
	{
		std::vector<types::Variant> values;
		values.push_back( 1);
		values.push_back( "xyz");
		values.push_back( true);
		values.push_back( 4.782 );
		trans->executeStatement( "INSERT INTO TestTest (id, name, active, price) VALUES ($1,$2,$3,$4)", values);
	}
	{
		std::vector<types::Variant> values;
		values.push_back( 2);
		values.push_back( "abc");
		values.push_back( false);
		values.push_back( -4.2344 );
		trans->executeStatement( "INSERT INTO TestTest (id, name, active, price) VALUES ($1,$2,$3,$4)", values);
	}
	// some maxima
	{
		std::vector<types::Variant> values;
		values.push_back( _WOLFRAME_MAX_INTEGER );
		values.push_back( "");
		values.push_back( false);
		values.push_back( std::numeric_limits<double>::max( ) );
		trans->executeStatement( "INSERT INTO TestTest (id, name, active, price) VALUES ($1,$2,$3,$4)", values);
	}
	// some minima
	{
		std::vector<types::Variant> values;
		values.push_back( _WOLFRAME_MIN_INTEGER );
		values.push_back( "");
		values.push_back( false);
		values.push_back( std::numeric_limits<double>::min( ) );
		trans->executeStatement( "INSERT INTO TestTest (id, name, active, price) VALUES ($1,$2,$3,$4)", values);
	}
	// null values
	{
		std::vector<types::Variant> values;
		values.push_back( types::VariantConst( ));
		values.push_back( types::VariantConst( ));
		values.push_back( types::VariantConst( ));
		values.push_back( types::VariantConst( ));
		trans->executeStatement( "INSERT INTO TestTest (id, name, active, price) VALUES ($1,$2,$3,$4)", values);
	}
}

TEST_F( SQLiteModuleFixture, ExecuteInstruction )
{	
	SQLiteDBunit dbUnit( "testDB", "test.db", true, false, 3,
			     std::list<std::string>(), std::list<std::string>() );
	Database* db = dbUnit.database( );
	Transaction* trans = db->transaction( "test" );

	// ok transaction create table statement with commit
	trans->begin( );
	trans->executeStatement( "DROP TABLE IF EXISTS TestTest");
	trans->executeStatement( "CREATE TABLE TestTest (id INTEGER, name TEXT, active BOOLEAN, price REAL)");
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
	Transaction::Result res = trans->executeStatement( "SELECT * FROM TestTest ORDER BY id IS NULL, id ASC");
	trans->commit( );
	EXPECT_EQ( res.size(), 5);
	EXPECT_EQ( res.colnames().size(), 4);
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
				ASSERT_DOUBLE_EQ( std::numeric_limits<double>::min( ), price );
				break;
			}

			case 2:
			case 3:	{
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
				EXPECT_EQ( _WOLFRAME_MAX_INTEGER, ri->at(0).touint());
				double price( ri->at(3).todouble());
				ASSERT_DOUBLE_EQ( std::numeric_limits<double>::max( ), price );
				break;
			}
			
			case 5: {
				ASSERT_FALSE( ri->at(0).defined( ) );
				ASSERT_FALSE( ri->at(1).defined( ) );
				ASSERT_FALSE( ri->at(2).defined( ) );
				ASSERT_FALSE( ri->at(3).defined( ) );
				break;
			}				
		}
	}

	trans->close( );
}

TEST_F( SQLiteModuleFixture, ExceptionSyntaxError )
{
	SQLiteDBunit dbUnit( "testDB", "test.db", true, false, 3,
			     std::list<std::string>(), std::list<std::string>() );
	Database* db = dbUnit.database( );
	Transaction* trans = db->transaction( "test" );
	
	trans->begin( );

	// execute an illegal SQL statement, must throw
	try {
		trans->executeStatement( "SELCT 1" );
		FAIL( ) << "Statement with illegal syntax should fail but doesn't!";
	} catch( const DatabaseTransactionErrorException &e ) {
		std::cout << e.what( ) << std::endl;
		ASSERT_EQ( e.statement, "SELCT 1" );
		ASSERT_EQ( e.errorclass, "SYNTAX" );
	} catch( ... ) {
		FAIL( ) << "Wrong exception class seen in database error!";
	}
	
	// auto rollback
	// auto close transaction
}

TEST_F( SQLiteModuleFixture, TooFewBindParameter )
{
	SQLiteDBunit dbUnit( "testDB", "test.db", true, false, 3,
			     std::list<std::string>(), std::list<std::string>() );
	Database* db = dbUnit.database( );
	Transaction* trans = db->transaction( "test" );
	
	trans->begin( );
	trans->executeStatement( "DROP TABLE IF EXISTS TestTest");
	trans->executeStatement( "CREATE TABLE TestTest (id INTEGER, name TEXT, active BOOLEAN, price REAL)");
	std::vector<types::Variant> values;
	values.push_back( 1);
	values.push_back( "xyz");
	values.push_back( true);
	// intentionally ommiting values here, must throw an error
	try {
		trans->executeStatement( "INSERT INTO TestTest (id, name, active, price) VALUES ($1,$2,$3,$4)", values);
		// we should not get here, just in case we close the transaction properly
		trans->commit( );
		trans->close( );
		FAIL( ) << "Reached success state, but should fail!";
// why is this another exception?
	} catch( const std::runtime_error &e ) {
		std::cout << e.what( ) << std::endl;
//	} catch( const DatabaseTransactionErrorException &e ) {
//		std::cout << e.what( );
//		ASSERT_EQ( e.errorclass, "INTERNAL" );		
	} catch( ... ) {
		FAIL( ) << "Wrong exception class seen in database error!";
	}

	// auto rollback?
	// auto close transaction?
}

TEST_F( SQLiteModuleFixture, TooManyBindParameter )
{
	SQLiteDBunit dbUnit( "testDB", "test.db", true, false, 3,
			     std::list<std::string>(), std::list<std::string>() );
	Database* db = dbUnit.database( );
	Transaction* trans = db->transaction( "test" );
	
	trans->begin( );
	trans->executeStatement( "DROP TABLE IF EXISTS TestTest");
	trans->executeStatement( "CREATE TABLE TestTest (id INTEGER, name TEXT, active BOOLEAN, price REAL)");
	std::vector<types::Variant> values;
	values.push_back( 1);
	values.push_back( "xyz");
	values.push_back( true);
	values.push_back( 4.782);
	values.push_back( "too much");
	// intentionally adding too many values here, must throw an error
	try {
		trans->executeStatement( "INSERT INTO TestTest (id, name, active, price) VALUES ($1,$2,$3,$4)", values);
		// we should not get here, just in case we close the transaction properly
		trans->commit( );
		trans->close( );
		FAIL( ) << "Reached success state, but should fail!";
	} catch( const DatabaseTransactionErrorException &e ) {
		std::cout << e.what( ) << std::endl;
		ASSERT_EQ( e.errorclass, "INTERNAL" );		
	} catch( ... ) {
		FAIL( ) << "Wrong exception class seen in database error!";
	}

	// auto rollback?
	// auto close transaction?
}

TEST_F( SQLiteModuleFixture, IllegalBindParameter )
{
	SQLiteDBunit dbUnit( "testDB", "test.db", true, false, 3,
			     std::list<std::string>(), std::list<std::string>() );
	Database* db = dbUnit.database( );
	Transaction* trans = db->transaction( "test" );
	
	trans->begin( );
	trans->executeStatement( "DROP TABLE IF EXISTS TestTest");
	trans->executeStatement( "CREATE TABLE TestTest (id INTEGER, name TEXT, active BOOLEAN, price REAL)");
	std::vector<types::Variant> values;
	values.push_back( 1);
	values.push_back( "xyz");
	values.push_back( "not used");
	values.push_back( true);
	values.push_back( 4.782);
	try {
		trans->executeStatement( "INSERT INTO TestTest (id, name, active, price) VALUES ($1,$2,$4,$5)", values);
		// should actually not work
		trans->commit( );
		trans->close( );
		FAIL( ) << "Reached success state, but should fail!";
	} catch( const DatabaseTransactionErrorException &e ) {
		std::cout << e.what( ) << std::endl;
		ASSERT_EQ( e.errorclass, "INTERNAL" );		
	} catch( ... ) {
		FAIL( ) << "Wrong exception class seen in database error!";
	}
	// auto rollback?
	// auto close transaction?	
}

TEST_F( SQLiteModuleFixture, ReusedBindParameter )
{
	SQLiteDBunit dbUnit( "testDB", "test.db", true, false, 3,
			     std::list<std::string>(), std::list<std::string>() );
	Database* db = dbUnit.database( );
	Transaction* trans = db->transaction( "test" );
	
	trans->begin( );
	trans->executeStatement( "DROP TABLE IF EXISTS TestTest");
	trans->executeStatement( "CREATE TABLE TestTest (id INTEGER, id2 INTEGER, id3 INTEGER)");
	std::vector<types::Variant> values;
	values.push_back( 47);
	trans->executeStatement( "INSERT INTO TestTest (id, id2, id3) VALUES ($1,$1,$1)", values);
	trans->commit( );

	trans->begin( );
	Transaction::Result res = trans->executeStatement( "SELECT * FROM TestTest");
	EXPECT_EQ( res.size(), 1);
	EXPECT_EQ( res.colnames().size(), 3);
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
	trans->close( );
}

TEST_F( SQLiteModuleFixture, ExpressionWithParametersAndTypeCoercion )
{
	SQLiteDBunit dbUnit( "testDB", "test.db", true, false, 3,
			     std::list<std::string>(), std::list<std::string>() );
	Database* db = dbUnit.database( );
	Transaction* trans = db->transaction( "test" );
	
	trans->begin( );
	trans->executeStatement( "DROP TABLE IF EXISTS TestTest");
	trans->executeStatement( "CREATE TABLE TestTest (id INTEGER, id2 INTEGER, id3 INTEGER)");
	std::vector<types::Variant> values;
	values.push_back( std::string("47"));
	values.push_back( 47);
	values.push_back( 47);
	trans->executeStatement( "INSERT INTO TestTest (id, id2, id3) VALUES ($1,$2+1,$3+2)", values);
	trans->commit( );

	trans->begin( );
	Transaction::Result res = trans->executeStatement( "SELECT * FROM TestTest");
	EXPECT_EQ( res.size(), 1);
	EXPECT_EQ( res.colnames().size(), 3);
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
	trans->close( );
}

TEST_F( SQLiteModuleFixture, DISABLED_UserInterface )
{
	SQLiteDBunit dbUnit( "testDB", "test.db", true, false, 3,
			     std::list<std::string>(), std::list<std::string>() );

	Database* db = dbUnit.database();
	const UserInterfaceLibrary* lib = db->UIlibrary();

	std::list< InterfaceObject::Info > objs = lib->userInterface( "", "", "" );
	EXPECT_TRUE( objs.empty() );
	delete lib;
}

int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS( );
}
