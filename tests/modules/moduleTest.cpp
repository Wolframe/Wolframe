//
// testing the Module Loader
//

#include "logger-v1.hpp"
#include "processor/moduleDirectory.hpp"
#include "gtest/gtest.h"

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

	ConfiguredBuilder* builder = modDir.getBuilder( "TestObject" );
	ASSERT_TRUE( builder != NULL );

	config::NamedConfiguration* configuration = builder->configuration( "TestObject" );
	ASSERT_TRUE( configuration != NULL );

	ConfiguredObjectConstructor< test::TestUnit >* cnstrctr = dynamic_cast< ConfiguredObjectConstructor< test::TestUnit >* >( builder->constructor( ) );
	ASSERT_TRUE( cnstrctr != NULL );
	test::TestUnit* unit = cnstrctr->object( *configuration );

	string s = unit->hello( );
	ASSERT_EQ( s, "hello" );

	delete configuration;
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

	ConfiguredBuilder* builder1 = modDir.getBuilder( "TestObject1" );
	ASSERT_TRUE( builder1 != NULL );

	ConfiguredBuilder* builder2 = modDir.getBuilder( "TestObject2" );
	ASSERT_TRUE( builder2 != NULL );

	config::NamedConfiguration* configuration1 = builder1->configuration( "TestObject1" );
	ASSERT_TRUE( configuration1 != NULL );

	config::NamedConfiguration* configuration2 = builder2->configuration( "TestObject2" );
	ASSERT_TRUE( configuration2 != NULL );

	ConfiguredObjectConstructor< test_containers::TestUnit1 >* cnstrctr1 = dynamic_cast< ConfiguredObjectConstructor< test_containers::TestUnit1 >* >( builder1->constructor( ) );
	ASSERT_TRUE( cnstrctr1 != NULL );
	test_containers::TestUnit1* unit1 = cnstrctr1->object( *configuration1 );

	ConfiguredObjectConstructor< test_containers::TestUnit2 >* cnstrctr2 = dynamic_cast< ConfiguredObjectConstructor< test_containers::TestUnit2 >* >( builder2->constructor( ) );
	ASSERT_TRUE( cnstrctr2 != NULL );
	test_containers::TestUnit2* unit2 = cnstrctr2->object( *configuration2 );

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
