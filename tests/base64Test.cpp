/************************************************************************

 Copyright (C) 2011 Project Wolframe.
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
// base64 tests
//
#include "gtest/gtest.h"
#include <stdexcept>
#include "base64.hpp"

using namespace _Wolframe;

static const char* emptyData = "";

static const char* vector1 = "f";
static const char* result1 = "Zg==";

static const char* vector2 = "fo";
static const char* result2 = "Zm8=";

static const char* vector3 = "foo";
static const char* result3 = "Zm9v";

static const char* vector4 = "foob";
static const char* result4 = "Zm9vYg==";

static const char* vector5 = "fooba";
static const char* result5 = "Zm9vYmE=";

static const char* vector6 = "foobar";
static const char* result6 = "Zm9vYmFy";
static const char* result6_bad = "Zm9vYmFy==";

static const size_t bufSize = 512 + 1;

static int strEncode( const char* data, char* encoded, size_t codeSize )
{
	base64::Encoder E;
	int ret = E.encode((const unsigned char *)data, strlen((const char *)data ), encoded, codeSize - 1 );
	if ( ret >= 0 )
		ret += E.encodeEnd( encoded + ret, codeSize - ret - 1 );
	if ( ret >= 0 )
		encoded[ ret ] = 0;
	return ret;
}

static int strDecode( const char* encoded, size_t codeSize, char* plain, size_t plainSize )
{
	base64::Decoder D;
	int ret = D.decode( encoded, codeSize, (unsigned char *)plain, plainSize );
	if ( ret >= 0 )
		plain[ ret ] = 0;
	return ret;
}

static int strDecode( const char* encoded, char* plain, size_t plainSize )
{
	return strDecode( encoded, strlen( encoded ), plain, plainSize );
}


TEST( Base64, encoding )
{
	char	encoded[ bufSize ];
	int	codeLength;

	codeLength = strEncode( emptyData, encoded, bufSize );
	EXPECT_EQ( 0, codeLength );
	EXPECT_STREQ( "", encoded );

	codeLength = strEncode( vector1, encoded, bufSize );
	EXPECT_EQ( 4, codeLength );
	EXPECT_STREQ( result1, encoded );

	codeLength = strEncode( vector2, encoded, bufSize );
	EXPECT_EQ( 4, codeLength );
	EXPECT_STREQ( result2, encoded );

	codeLength = strEncode( vector3, encoded, bufSize );
	EXPECT_EQ( 4, codeLength );
	EXPECT_STREQ( result3, encoded );

	codeLength = strEncode( vector4, encoded, bufSize );
	EXPECT_EQ( 8, codeLength );
	EXPECT_STREQ( result4, encoded );

	codeLength = strEncode( vector5, encoded, bufSize );
	EXPECT_EQ( 8, codeLength );
	EXPECT_STREQ( result5, encoded );

	codeLength = strEncode( vector6, encoded, bufSize );
	EXPECT_EQ( 8, codeLength );
	EXPECT_STREQ( result6, encoded );
}

TEST( Base64, decoding )
{
	char	decoded[ bufSize ];
	int	plainLength;

	plainLength = strDecode( result1, decoded, bufSize );
	EXPECT_EQ( 1, plainLength );
	EXPECT_STREQ( vector1, decoded );

	plainLength = strDecode( result2, decoded, bufSize );
	EXPECT_EQ( 2, plainLength );
	EXPECT_STREQ( vector2, decoded );

	plainLength = strDecode( result3, decoded, bufSize );
	EXPECT_EQ( 3, plainLength );
	EXPECT_STREQ( vector3, decoded );

	plainLength = strDecode( result4, decoded, bufSize );
	EXPECT_EQ( 4, plainLength );
	EXPECT_STREQ( vector4, decoded );

	plainLength = strDecode( result5, decoded, bufSize );
	EXPECT_EQ( 5, plainLength );
	EXPECT_STREQ( vector5, decoded );

	plainLength = strDecode( result6, decoded, bufSize );
	EXPECT_EQ( 6, plainLength );
	EXPECT_STREQ( vector6, decoded );
}

TEST( Base64, PaddingTolerance )
{
	char	decoded[ bufSize ];
	int	plainLength;

	plainLength = strDecode( result1, 2, decoded, bufSize );
	EXPECT_EQ( 1, plainLength );
	EXPECT_STREQ( vector1, decoded );

	plainLength = strDecode( result1, 3, decoded, bufSize );
	EXPECT_EQ( 1, plainLength );
	EXPECT_STREQ( vector1, decoded );

	plainLength = strDecode( result2, 3, decoded, bufSize );
	EXPECT_EQ( 2, plainLength );
	EXPECT_STREQ( vector2, decoded );

	plainLength = strDecode( result4, 6, decoded, bufSize );
	EXPECT_EQ( 4, plainLength );
	EXPECT_STREQ( vector4, decoded );

	plainLength = strDecode( result4, 7, decoded, bufSize );
	EXPECT_EQ( 4, plainLength );
	EXPECT_STREQ( vector4, decoded );

	plainLength = strDecode( result5, 7, decoded, bufSize );
	EXPECT_EQ( 5, plainLength );
	EXPECT_STREQ( vector5, decoded );

	plainLength = strDecode( result6_bad, 9, decoded, bufSize );
	EXPECT_EQ( 6, plainLength );
	EXPECT_STREQ( vector6, decoded );

	plainLength = strDecode( result6_bad, 10, decoded, bufSize );
	EXPECT_EQ( 6, plainLength );
	EXPECT_STREQ( vector6, decoded );
}

int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}

