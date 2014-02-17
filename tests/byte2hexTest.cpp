/************************************************************************

 Copyright (C) 2011 - 2014 Project Wolframe.
 All rights reserved.

 This file is part of Project Wolframe.

 Commercial Usage
    Licensees holding valid Project Wolframe Commercial licenses may
    use this file in accordance with the Project Wolframe
    Commercial License Agreement provided with the Software or,
    alternatively, in accordance with the terms contained
    in a written agreement between the licensee and Project Wolframe.

 GNU General Public License Usage
    Alternatively, you can redistribute this file and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Wolframe is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
//
// byte <-> ASCII hex conversion tests

#include "gtest/gtest.h"
#include "types/byte2hex.h"


TEST( byte2hex, Bytes_to_HEX )
{
	unsigned char	bytes[16];
	char		output[33];
	int		i;

	for ( i = 0; i < 16; i++ )
		bytes[i] = 0;
	ASSERT_TRUE( byte2hex( bytes, 11, output, 33 ) == 22 );
	ASSERT_STREQ( "0000000000000000000000", output );

	for ( i = 0; i < 16; i++ )
		bytes[i] = 0xff;
	ASSERT_TRUE( byte2hex( bytes, 8, output, 33 ) == 16 );
	ASSERT_STREQ( "FFFFFFFFFFFFFFFF", output );

	for ( i = 0; i < 16; i++ )
		bytes[i] = i;
	ASSERT_TRUE( byte2hex( bytes, 16, output, 33 ) == 32 );
	ASSERT_STREQ( "000102030405060708090A0B0C0D0E0F", output );
}

TEST( byte2hex, Bytes_to_HEX_Overflow )
{
	unsigned char	bytes[16];
	char		output[33];

	ASSERT_TRUE( byte2hex( bytes, 16, output, 33 ) == 32 );
	ASSERT_TRUE( byte2hex( bytes, 17, output, 34 ) == -1 );
	ASSERT_TRUE( byte2hex( bytes, 0, output, 0 ) == -1 );
	ASSERT_TRUE( byte2hex( bytes, 0, output, 1 ) == 0 );
}

TEST( byte2hex, HEX_to_bytes_Invalid_char )
{
	unsigned char	bytes[16];

	ASSERT_TRUE( hex2byte( "ghijklmnopqrstuvwx", bytes, 16 ) == -2 );
	ASSERT_TRUE( hex2byte( "GHIJKLMNOPQRSTUVWX", bytes, 16 ) == -2 );
	ASSERT_TRUE( hex2byte( " \t \n ", bytes, 16 ) == 0 );
}

TEST( byte2hex, HEX_to_bytes_Overflow )
{
	unsigned char	bytes[4];

	ASSERT_TRUE( hex2byte( "00 00 00 00", bytes, 4 ) == 4 );
	ASSERT_TRUE( hex2byte( "00 00 00 00 00", bytes, 4 ) == -1 );
}

TEST( byte2hex, HEX_to_bytes )
{
	unsigned char	bytes[16];
	char		output[33];
	int		i;

	for ( i = 0; i < 16; i++ )
		bytes[i] = 0xff;
	ASSERT_TRUE( hex2byte( "0000000000000000000000", bytes, 16 ) == 11 );
	ASSERT_TRUE( byte2hex( bytes, 11, output, 33 ) == 22 );
	ASSERT_STREQ( "0000000000000000000000", output );

	for ( i = 0; i < 16; i++ )
		bytes[i] = 0xff;
	ASSERT_TRUE( hex2byte( "00 00 00 00 00 00 00 00 00 00 00", bytes, 16 ) == 11 );
	ASSERT_TRUE( byte2hex( bytes, 11, output, 33 ) == 22 );
	ASSERT_STREQ( "0000000000000000000000", output );

	for ( i = 0; i < 16; i++ )
		bytes[i] = 0xff;
	ASSERT_TRUE( hex2byte( "0 00 00 00 00 00 00 00 00 00 00", bytes, 16 ) == 11 );
	ASSERT_TRUE( byte2hex( bytes, 11, output, 33 ) == 22 );
	ASSERT_STREQ( "0000000000000000000000", output );

	for ( i = 0; i < 16; i++ )
		bytes[i] = 0xff;
	ASSERT_TRUE( hex2byte( " 0 00 00 00 00 00 00 00 00 00 00 ", bytes, 16 ) == 11 );
	ASSERT_TRUE( byte2hex( bytes, 11, output, 33 ) == 22 );
	ASSERT_STREQ( "0000000000000000000000", output );

	for ( i = 0; i < 16; i++ )
		bytes[i] = 0x00;
	ASSERT_TRUE( hex2byte( "FFFFFFFFFFFFFFFF", bytes, 16 ) == 8 );
	ASSERT_TRUE( byte2hex( bytes, 8, output, 33 ) == 16 );
	ASSERT_STREQ( "FFFFFFFFFFFFFFFF", output );

	for ( i = 0; i < 16; i++ )
		bytes[i] = 0x00;
	ASSERT_TRUE( hex2byte( "FF FF FF FF FF FF FF FF", bytes, 16 ) == 8 );
	ASSERT_TRUE( byte2hex( bytes, 8, output, 33 ) == 16 );
	ASSERT_STREQ( "FFFFFFFFFFFFFFFF", output );

	for ( i = 0; i < 16; i++ )
		bytes[i] = 0x00;
	ASSERT_TRUE( hex2byte( "F FF FF FF FF FF FF FF", bytes, 16 ) == 8 );
	ASSERT_TRUE( byte2hex( bytes, 8, output, 33 ) == 16 );
	ASSERT_STREQ( "0FFFFFFFFFFFFFFF", output );

	for ( i = 0; i < 16; i++ )
		bytes[i] = 0x00;
	ASSERT_TRUE( hex2byte( " F FF FF FF FF FF FF FF", bytes, 16 ) == 8 );
	ASSERT_TRUE( byte2hex( bytes, 8, output, 33 ) == 16 );
	ASSERT_STREQ( "0FFFFFFFFFFFFFFF", output );
}

int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}

