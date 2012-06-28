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
	if ( ret >= 0 )	{
		int	retEnd;
		if (( retEnd = E.encodeEnd( encoded + ret, codeSize - ret - 1 )) >= 0 )
			ret += retEnd;
		else
			return retEnd;
	}
	if ( ret >= 0 )
		encoded[ ret ] = 0;
	return ret;
}

static int strDecode( const char* encoded, size_t codeSize, char* plain, size_t plainSize )
{
	base64::Decoder D;
	int ret = D.decode( encoded, codeSize, (unsigned char *)plain, plainSize - 1 );
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

TEST( Base64, InsufficientBuffer )
{
	char	buffer[ bufSize ];

	EXPECT_EQ( -1, strEncode( vector1, buffer, 1 ));
	EXPECT_EQ( -1, strEncode( vector1, buffer, 2 ));
	EXPECT_EQ( -1, strEncode( vector1, buffer, 3 ));
	EXPECT_EQ( -1, strEncode( vector1, buffer, 4 ));
	EXPECT_EQ( 4, strEncode( vector1, buffer, 5 ));

	EXPECT_EQ( -1, strEncode( vector2, buffer, 1 ));
	EXPECT_EQ( -1, strEncode( vector2, buffer, 2 ));
	EXPECT_EQ( -1, strEncode( vector2, buffer, 3 ));
	EXPECT_EQ( -1, strEncode( vector2, buffer, 4 ));
	EXPECT_EQ( 4, strEncode( vector2, buffer, 5 ));

	EXPECT_EQ( -1, strEncode( vector3, buffer, 1 ));
	EXPECT_EQ( -1, strEncode( vector3, buffer, 2 ));
	EXPECT_EQ( -1, strEncode( vector3, buffer, 3 ));
	EXPECT_EQ( -1, strEncode( vector3, buffer, 4 ));
	EXPECT_EQ( 4, strEncode( vector3, buffer, 5 ));

	EXPECT_EQ( -1, strDecode( result1, buffer, 1 ));
	EXPECT_EQ( 1, strDecode( result1, buffer, 2 ));

	EXPECT_EQ( -1, strDecode( result2, buffer, 1 ));
	EXPECT_EQ( -1, strDecode( result2, buffer, 2 ));
	EXPECT_EQ( 2, strDecode( result2, buffer, 3 ));

	EXPECT_EQ( -1, strDecode( result3, buffer, 1 ));
	EXPECT_EQ( -1, strDecode( result3, buffer, 2 ));
	EXPECT_EQ( -1, strDecode( result3, buffer, 3 ));
	EXPECT_EQ( 3, strDecode( result3, buffer, 4 ));

	EXPECT_EQ( -1, strDecode( result4, buffer, 1 ));
	EXPECT_EQ( -1, strDecode( result4, buffer, 2 ));
	EXPECT_EQ( -1, strDecode( result4, buffer, 3 ));
	EXPECT_EQ( -1, strDecode( result4, buffer, 4 ));
	EXPECT_EQ( 4, strDecode( result4, buffer, 5 ));
}

// The decoder doesn't detect invalid codes for now
//TEST( Base64, InvalidCode )
//{
//	static const char* invalid = "Zm9?YmFy==";
//	char	buffer[ bufSize ];
//
//	EXPECT_EQ( -2, strDecode( invalid, buffer, bufSize ));
//	EXPECT_STREQ( vector6, buffer );
//}


TEST( Base64, RandomData )
{
	size_t		dataSize, encodedSize;
	unsigned short	lineLength;
	unsigned char*	data;
	char*		encoded;
	unsigned char*	decoded;

// Data size: 16815, line length: 7, encoded estimated size: 28024
// Data size: 12879, line length: 212, encoded estimated size: 17252

	srand( time( NULL ) );
	dataSize = 1 + rand() % 32768;
	lineLength = rand() % 512;
	encodedSize = (( dataSize + 2 ) / 3 ) * 4;
	if ( lineLength / 4 )	{
		unsigned short effLineLength = ( lineLength / 4 ) * 4;
		encodedSize += encodedSize / effLineLength - (( encodedSize % effLineLength ) ? 0 : 1 );
	}

	data = new unsigned char[ dataSize ];
	encoded = new char[ encodedSize ];
	decoded = new unsigned char[ dataSize ];

	std::cout << "Data size: " << dataSize << ", line length: " << lineLength
		  << ", encoded estimated size: " << encodedSize << std::endl;
	for ( size_t i = 0; i < dataSize; i++ )
		data[ i ] = rand() % 256;

	base64::Encoder E( 0, lineLength );
	int encodeResult = E.encode( data, dataSize, encoded, encodedSize );
	int encodeEndResult = E.encodeEnd( encoded + encodeResult, encodedSize - encodeResult );
		std::cout << "Encode result: " << encodeResult << ", end result: " << encodeEndResult << std::endl;
	encodeResult += encodeEndResult;
	EXPECT_EQ( encodeResult, encodedSize );

	base64::Decoder D;
	int decodeResult = D.decode( encoded, encodeResult, decoded, dataSize );
	EXPECT_EQ( dataSize, decodeResult );
	EXPECT_EQ( 0, memcmp( data, decoded, dataSize ));

	delete[] data;
	delete[] encoded;
	delete[] decoded;
}

int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}

