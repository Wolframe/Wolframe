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
		boost::locale::generator m_gen;

	protected:
		BoostLocaleFixture( )
		{
			m_gen.locale_cache_enabled( true );
			boost::locale::generator gen;
			
			// initialize a standard locale
			std::locale loc = gen( "" );
			std::locale::global( loc );
			
			// switch localization backend to ICU
			boost::locale::localization_backend_manager manager =
				boost::locale::localization_backend_manager::global( );
			manager.select( "icu" );
		}
};

TEST_F( BoostLocaleFixture, SimpleTest )
{
	const std::string input = "grüßen";
	std::string output = boost::locale::to_upper( input );
	const std::string must = "GRÜSSEN";
	EXPECT_EQ( output, must );
}

int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS( );
}
