//
// string normalization unit tests
//

#include "utils/miscUtils.hpp"
#include "gtest/gtest.h"

using namespace _Wolframe::utils;

TEST( StringNormalizationFixture, InPlaceNormalization )	{
	const char* testStr = "  This is   a nOt \n \tnormalized string\nwith\tüöä$éàè˝àč \t characters\n";
	const char* expectedStr = "THIS IS A NOT NORMALIZED STRING WITH üöä$éàè˝àč CHARACTERS";

	std::string str = testStr;
	normalizeString( str );
	ASSERT_STREQ( expectedStr, str.c_str());
}

TEST( StringNormalizationFixture, CopyNormalization )	{
	const char* testStr = "  This is   a nOt \n \tnormalized string\nwith\tüöä$éàè˝àč \t characters\n";
	const char* expectedStr = "THIS IS A NOT NORMALIZED STRING WITH üöä$éàè˝àč CHARACTERS";

	std::string str = testStr;
	std::string ret = normalizeString_copy( str );
	ASSERT_STREQ( testStr, str.c_str());
	ASSERT_STREQ( expectedStr, ret.c_str());
}


int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
