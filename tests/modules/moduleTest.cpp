//
// testing the Module Loader
//

#include "logger-v1.hpp"
#include "moduleInterface.hpp"
#include <gtest/gtest.h>

#include "tests/mod_test/common.hpp"

#include <string>
#include <list>

using namespace _Wolframe::module;
using namespace _Wolframe::log;
using namespace _Wolframe;
using namespace std;

// The fixture for testing class _Wolframe::module
class ModuleFixture : public ::testing::Test
{
	protected:
                LogBackend& logBack;
		
	protected:
		ModuleFixture( ) :
			logBack( LogBackend::instance( ) )
		{
			logBack.setConsoleLevel( LogLevel::LOGLEVEL_DATA );
		}
};

TEST_F( ModuleFixture, LoadingModuleFromDir )
{
	ModulesDirectory modDir;
	list<string> modFiles;

#ifndef _WIN32
	modFiles.push_back( "./tests/mod_test/mod_test.so" );
#else
	modFiles.push_back( ".\\tests\\mod_test\\mod_test.dll" );
#endif
	bool res = LoadModules( modDir, modFiles );
	ASSERT_TRUE( res );
	
	ModuleContainer* container = modDir.getContainer( "TestObject" );
	ASSERT_TRUE( container != NULL );
	
	config::ObjectConfiguration* configuration = container->configuration( "TestObject" );
	ASSERT_TRUE( configuration != NULL );

	test::TestModuleContainer* obj = dynamic_cast<test::TestModuleContainer *>( container->container( *configuration ) );
	test::TestUnit* unit = obj->object( );

	string s = unit->hello( );
	ASSERT_EQ( s, "hello" );
}

int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS( );
}
