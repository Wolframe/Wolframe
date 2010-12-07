//
// byte <-> ASCII hex conversion tests

#define BOOST_TEST_MODULE "ASCII hex conversion"

#include <boost/test/unit_test.hpp>

#include "byte2hex.h"


BOOST_AUTO_TEST_CASE( _byte_to_hex_ )
{
	unsigned char	bytes[16];
	char		output[33];
	int		i;

	for ( i = 0; i < 16; i++ )
		bytes[i] = 0;
	BOOST_CHECK_EQUAL( byte2hex( bytes, 11, output, 33 ), "0000000000000000000000" );

	for ( i = 0; i < 16; i++ )
		bytes[i] = 0xff;
	BOOST_CHECK_EQUAL( byte2hex( bytes, 8, output, 33 ), "FFFFFFFFFFFFFFFF" );

	BOOST_CHECK( byte2hex( bytes, 78, output, 33 ) == NULL );

	BOOST_CHECK( byte2hex( bytes, 0, output, 0 ) == NULL );
}

