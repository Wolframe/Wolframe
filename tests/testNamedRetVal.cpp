//
// test if the compiler does the named return value optimization
//
// If the compiler does the optimization, the output should be just:
// foobar::foobar()
// foobar::~foobar()
//
// Most of the compilers should do that these days, but let's test...

#include <iostream>
#include <string>

#include "gtest/gtest.h"
#include "wtest/testReport.hpp"

std::string	outStr;

class foobar	{
public:
	foobar()		{ outStr += "foobar::foobar()\n"; }
	~foobar()		{ outStr += "foobar::~foobar()\n"; }

	foobar( const foobar& )
				{ outStr += "foobar::foobar( const foobar & )\n"; }
	foobar& operator= ( const foobar& )
				{ outStr += "foobar::operator=( const foobar & )\n"; return *this; }

	void ival( int nval )	{ _ival = nval; }
private:
	int _ival;
};

foobar f( int val )
{
	foobar local;
	local.ival( val );
	return local;
}

void testFunc()
{
	foobar ml = f( 1024 );
}

TEST( NamedRetVal, test )	{
	testFunc();
	ASSERT_STREQ( outStr.c_str(), "foobar::foobar()\nfoobar::~foobar()\n" );
}


int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	WOLFRAME_GTEST_REPORT( argv[0], refpath.string());
	return RUN_ALL_TESTS();
}
