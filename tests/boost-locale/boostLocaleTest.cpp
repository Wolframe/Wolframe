//
// testing the boost-locale module with ICU
//

#include "gtest/gtest.h"

#include <boost/locale/generator.hpp>
#include <boost/locale.hpp>

#include <string>

// The fixture for testing class _Wolframe::module
class BoostLocaleFixture : public ::testing::Test
{
	protected:
		BoostLocaleFixture( )
		{
			// Get global backend:
			boost::locale::localization_backend_manager my = boost::locale::localization_backend_manager::global();

			// select std backend as default:
			my.select("icu");

			// create a generator that uses this backend:
			boost::locale::generator gen(my);

			// set this backend globally:
			boost::locale::localization_backend_manager::global(my);

			// initialize a standard locale
			std::locale loc = boost::locale::generator().generate("");
			std::locale::global( loc );
		}
};

TEST_F( BoostLocaleFixture, SimpleTest )
{
//	const std::string input = "gr""\xC3\xBC""\xC3\x9F""en";		//Isolatin: grüßen
//	std::string output = boost::locale::to_upper( input );
//	const std::string must = "GR""\xC3\x9C""SSEN";			//Isolatin: GRÜSSEN (ICU converts #S to SS)
	const std::string input = "gr""\xC3\xBC""bel";			//Isolatin: grübel
	std::string output = boost::locale::to_upper( input );
	const std::string must = "GR""\xC3\x9C""BEL";			//Isolatin: GRÜBEL
	EXPECT_EQ( output, must );
}

int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS( );
}
