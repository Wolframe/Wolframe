//
// testing the Module Loader
//

#include "moduleInterface.hpp"
#include <gtest/gtest.h>

#include <string>
#include <list>

using namespace _Wolframe::module;
using namespace std;

// The fixture for testing class _Wolframe::module
class ModuleFixture : public ::testing::Test
{
	protected:
		
	protected:
		ModuleFixture( )
		{
		}
};

TEST_F( ModuleFixture, LoadingModuleFromDir )
{
	ModulesDirectory modDir;
	list<string> modFiles;

#ifndef _WIN32
//	modFiles.push_back( "./tests/mod_test/mod_test.so" );
#else
//	modFiles.push_back( ".\\tests\\mod_test\\mod_test.dll" );
#endif
	
	bool res = LoadModules( modDir, modFiles );
	ASSERT_EQ( res, true );

}

int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS( );
}
