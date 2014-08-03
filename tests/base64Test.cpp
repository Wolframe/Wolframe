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
// base64 tests
//
#include "gtest/gtest.h"
#include "wtest/testReport.hpp"
#include "types/base64.hpp"

#ifdef _WIN32
#define WIN32_MEAN_AND_LEAN
#include <windows.h>
#else
#include <stdlib.h>
#include <time.h>
#endif

#undef _BASE64_WRITE_OUTPUT
#undef _BASE64_LOOP_TEST
#undef _BASE64_PRINT_TEST_PARAMETRS

//#define _BASE64_WRITE_OUTPUT
#define _BASE64_LOOP_TEST
#define _BASE64_NOF_LOOPS	4096
//#define _BASE64_PRINT_TEST_PARAMETRS

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
	int ret = E.encodeChunk((const unsigned char *)data, strlen((const char *)data ), encoded, codeSize );
	if ( ret >= 0 )	{
		int	retEnd;
		if (( retEnd = E.encodeEndChunk( encoded + ret, codeSize - ret )) >= 0 )
			ret += retEnd;
		else
			return retEnd;
	}
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
	EXPECT_STREQ( "", _Wolframe::base64::encode( emptyData, 0, 0 ).c_str() );

	codeLength = strEncode( vector1, encoded, bufSize );
	EXPECT_EQ( 4, codeLength );
	EXPECT_STREQ( result1, encoded );
	EXPECT_STREQ( result1, _Wolframe::base64::encode( vector1, strlen( vector1 ), 0 ).c_str() );

	codeLength = strEncode( vector2, encoded, bufSize );
	EXPECT_EQ( 4, codeLength );
	EXPECT_STREQ( result2, encoded );
	EXPECT_STREQ( result2, _Wolframe::base64::encode( vector2, strlen( vector2 ), 0 ).c_str() );

	codeLength = strEncode( vector3, encoded, bufSize );
	EXPECT_EQ( 4, codeLength );
	EXPECT_STREQ( result3, encoded );
	EXPECT_STREQ( result3, _Wolframe::base64::encode( vector3, strlen( vector3 ), 0 ).c_str() );

	codeLength = strEncode( vector4, encoded, bufSize );
	EXPECT_EQ( 8, codeLength );
	EXPECT_STREQ( result4, encoded );
	EXPECT_STREQ( result4, _Wolframe::base64::encode( vector4, strlen( vector4 ), 0 ).c_str() );

	codeLength = strEncode( vector5, encoded, bufSize );
	EXPECT_EQ( 8, codeLength );
	EXPECT_STREQ( result5, encoded );
	EXPECT_STREQ( result5, _Wolframe::base64::encode( vector5, strlen( vector5 ), 0 ).c_str() );

	codeLength = strEncode( vector6, encoded, bufSize );
	EXPECT_EQ( 8, codeLength );
	EXPECT_STREQ( result6, encoded );
	EXPECT_STREQ( result6, _Wolframe::base64::encode( vector6, strlen( vector6 ), 0 ).c_str() );
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

TEST( Base64, WhiteSpaces )
{
	static const char* whiteSpaces = "  \t\n\rZm9v  YmFy=\r= =\t=\n";
	char	decoded[ bufSize ];

	int plainLength = strDecode( whiteSpaces, decoded, bufSize );
	EXPECT_EQ( 6, plainLength );
	EXPECT_STREQ( vector6, decoded );
}

#ifdef _BASE64_WRITE_OUTPUT
#include <iostream>
#include <fstream>
#endif

TEST( Base64, RandomData )
{
	size_t		dataSize, encodedSize;
	unsigned short	lineLength;
	unsigned char*	data;
	char*		encoded1;
	char*		encoded2;
	unsigned char*	decoded;
	int		chunkSize;

#ifndef _WIN32
	srand( time( NULL ) );
#else
	srand( GetTickCount( ) );
#endif
#ifdef _BASE64_LOOP_TEST
	for ( int i = 0; i < _BASE64_NOF_LOOPS; i++ )	{
#endif
		dataSize = 1 + rand() % 32768; lineLength = rand() % 2048;
//		dataSize = 1 + rand() % 64; lineLength = rand() % 32;
		chunkSize = 1 + rand() % dataSize;
// fixed data
//		dataSize = 8; lineLength = 16;	chunkSize = 8;

		encodedSize = base64::encodedSize( dataSize, lineLength );

		data = new unsigned char[ dataSize ];
		encoded1 = new char[ encodedSize + 1 ];
		encoded2 = new char[ encodedSize + 1 ];
		decoded = new unsigned char[ dataSize + 1 ];

		for ( size_t j = 0; j < dataSize; j++ )
			data[ j ] = rand() % 256;

#ifdef _BASE64_WRITE_OUTPUT
		// data file
		std::ofstream fdata( "b64data", std::ios_base::out | std::ios_base::binary );
		fdata.write(( const char *)data, dataSize );
		fdata.close();
#endif
		base64::Encoder E( 0, lineLength );
		int encodeResult = 0;
		int partialResult = 0;
		size_t dataUsed = 0;
#ifdef _BASE64_PRINT_TEST_PARAMETRS
		std::cout << "Data: " << dataSize << " bytes, line length: " << lineLength
			  << ", encoded computed size: " << encodedSize;
		std::cout << ", chunk size: " << chunkSize;
#endif
		while ( dataUsed <= dataSize - chunkSize )	{
			partialResult = E.encodeChunk( data + dataUsed, chunkSize,
						       encoded1 + encodeResult, encodedSize - encodeResult );
			EXPECT_GE( partialResult, 0 );
			encodeResult += partialResult;
			EXPECT_LE( encodeResult, (long int)encodedSize );
			dataUsed += chunkSize;
		}
		partialResult = E.encodeChunk( data + dataUsed, dataSize - dataUsed,
					       encoded1 + encodeResult, encodedSize - encodeResult + 1 );
		EXPECT_GE( partialResult, 0 );
		encodeResult += partialResult;
		EXPECT_LE( encodeResult, (long int)encodedSize );
		int encodeEndResult = E.encodeEndChunk( encoded1 + encodeResult, encodedSize - encodeResult + 1 );
#ifdef _BASE64_PRINT_TEST_PARAMETRS
		std::cout << ", encoded size: " << encodeResult << " + " << encodeEndResult << std::endl;
#endif
		encodeResult += encodeEndResult;
#ifdef _BASE64_WRITE_OUTPUT
		// encoded file
		std::ofstream edata1( "b64encoded1", std::ios_base::out | std::ios_base::binary );
		edata1.write(( const char *)encoded1, encodeResult );
		edata1.close();
#endif
		EXPECT_EQ( encodeResult, (long int)encodedSize );

		encodeResult = base64::encode( data, dataSize, encoded2, encodedSize + 1, lineLength );
#ifdef _BASE64_WRITE_OUTPUT
		// encoded file
		std::ofstream edata2( "b64encoded2", std::ios_base::out | std::ios_base::binary );
		edata2.write(( const char *)encoded2, encodeResult );
		edata2.close();
#endif
		EXPECT_EQ( encodeResult, (long int)encodedSize );
		EXPECT_EQ( 0, memcmp( encoded1, encoded2, encodedSize ));

		decoded[ dataSize ] = 0x5a;
		base64::Decoder D;
		int decodeResult = 0;
		dataUsed = 0;
		partialResult = 0;
		while ( dataUsed <= (size_t)( encodeResult - chunkSize ))	{
			partialResult = D.decode( encoded1 + dataUsed, chunkSize,
						  decoded + decodeResult, dataSize - decodeResult + 1 );
			EXPECT_GE( partialResult, 0 );
			decodeResult += partialResult;
			EXPECT_LE( decodeResult, (long int)dataSize );
			dataUsed += chunkSize;
		}
		partialResult = D.decode( encoded1 + dataUsed, encodeResult - dataUsed,
					  decoded + decodeResult, dataSize - decodeResult + 1 );
		EXPECT_GE( partialResult, 0 );
		decodeResult += partialResult;
#ifdef _BASE64_WRITE_OUTPUT
		// decoded file
		std::ofstream ddata1( "b64decoded1", std::ios_base::out | std::ios_base::binary );
		ddata1.write(( const char *)decoded, decodeResult );
		ddata1.close();
#endif
		EXPECT_EQ( dataSize, (size_t)decodeResult );
		EXPECT_EQ( 0, memcmp( data, decoded, dataSize ));
		EXPECT_EQ( decoded[ dataSize ], 0x5a );

		decoded[ dataSize ] = 0xa5;
		decodeResult = base64::decode( encoded1, encodeResult, decoded, dataSize + 1 );
#ifdef _BASE64_WRITE_OUTPUT
		// decoded file
		std::ofstream ddata2( "b64decoded2", std::ios_base::out | std::ios_base::binary );
		ddata2.write(( const char *)decoded, decodeResult );
		ddata2.close();
#endif
		EXPECT_EQ( (long int)dataSize, decodeResult );
		EXPECT_EQ( 0, memcmp( data, decoded, dataSize ));
		EXPECT_EQ( decoded[ dataSize ], 0xa5 );

		delete[] data;
		delete[] encoded1;
		delete[] encoded2;
		delete[] decoded;
#ifdef _BASE64_LOOP_TEST
	}
#endif
}

int main( int argc, char **argv )
{
	WOLFRAME_GTEST_REPORT( argv[0], refpath.string());
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}

