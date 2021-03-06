//
// Test if the compilers template resolver allows pointer to members as template parameters
// This construct is important for calling classes that are not yet known and neither the control flow (defined hooks in a STM)
//

#include <iostream>
#include <string>

#include "gtest/gtest.h"
#include "wtest/testReport.hpp"

static std::string outStr;

template <class T, int (T::*f)( char arg)>
struct Wrapper
{
	static int function( void* this_, char arg)
	{
		return (((T*)this_)->*f)( arg);
	}
};

struct C1
{
	int a( char arg)
	{
		outStr.append( " C1:");
		outStr.push_back( arg);
		return 1;
	}
};

struct C2
{
	int b( char arg)
	{
		outStr.append( " C2:");
		outStr.push_back( arg);
		return 1;
	}
};


static C1 c1;
static C2 c2;
int (*func1)( void*, char ) = &Wrapper<C1,&C1::a>::function;
int (*func2)( void*, char ) = &Wrapper<C2,&C2::b>::function;


TEST( MethodMemberPointerTemplate, test )	{
	func1( &c1, '1');
	func2( &c2, '2');
	ASSERT_STREQ( outStr.c_str(), " C1:1 C2:2");
}


int main( int argc, char **argv )
{
	WOLFRAME_GTEST_REPORT( argv[0], refpath.string());
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}

