//
// testing the Module Loader
//

#include "logger-v1.hpp"
#include "moduleInterface.hpp"
#include <gtest/gtest.h>

#include "tests/mod_test/common.hpp"
#include "tests/mod_test_containers/common.hpp"

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
	
	ContainerBuilder* container = modDir.getContainer( "TestObject" );
	ASSERT_TRUE( container != NULL );
	
	config::ObjectConfiguration* configuration = container->configuration( "TestObject" );
	ASSERT_TRUE( configuration != NULL );

	test::TestModuleContainer* obj = dynamic_cast<test::TestModuleContainer *>( container->container( *configuration ) );
	test::TestUnit* unit = obj->object( );

	string s = unit->hello( );
	ASSERT_EQ( s, "hello" );
	
	delete unit;
}

TEST_F( ModuleFixture, LoadingModuleWithMultipleContainers )
{
	ModulesDirectory modDir;
	list<string> modFiles;

#ifndef _WIN32
	modFiles.push_back( "./tests/mod_test_containers/mod_test_containers.so" );
#else
	modFiles.push_back( ".\\tests\\mod_test_containers\\mod_test_containers.dll" );
#endif
	bool res = LoadModules( modDir, modFiles );
	ASSERT_TRUE( res );

	ContainerBuilder* container1 = modDir.getContainer( "TestObject1" );
	ASSERT_TRUE( container1 != NULL );

	ContainerBuilder* container2 = modDir.getContainer( "TestObject2" );
	ASSERT_TRUE( container2 != NULL );

	config::ObjectConfiguration* configuration1 = container1->configuration( "TestObject1" );
	ASSERT_TRUE( configuration1 != NULL );

	config::ObjectConfiguration* configuration2 = container2->configuration( "TestObject2" );
	ASSERT_TRUE( configuration2 != NULL );

	test_containers::TestModuleContainer1* obj1 = dynamic_cast<test_containers::TestModuleContainer1 *>( container1->container( *configuration1 ) );
	test_containers::TestUnit1* unit1 = obj1->object( );

	test_containers::TestModuleContainer2* obj2 = dynamic_cast<test_containers::TestModuleContainer2 *>( container2->container( *configuration2 ) );
	test_containers::TestUnit2* unit2 = obj2->object( );

	string s1 = unit1->hello( );
	ASSERT_EQ( s1, "hello" );

	string s2 = unit2->hullo( );
	ASSERT_EQ( s2, "hullo" );

	delete unit1;	
	delete unit2;
}

int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS( );
}
