//
// testing the Module Loader
//

#include "logger-v1.hpp"
#include "processor/moduleDirectory.hpp"
#include "gtest/gtest.h"

#include "SQLite.hpp"

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
