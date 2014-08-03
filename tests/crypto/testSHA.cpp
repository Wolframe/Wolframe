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
// SHA-1 and SHA-2 unit tests
//

#include "gtest/gtest.h"
#include "wtest/testReport.hpp"
#include <cstring>
#include "crypto/sha2.h"
#include "crypto/sha1.h"


/* FIPS 180-2 Validation tests */

static const char message1[] =	"abc";

static const char message2a[] =	"abcdbcdecdefdefgefghfghighijhi"
				"jkijkljklmklmnlmnomnopnopq";

static const char message2b[] =	"abcdefghbcdefghicdefghijdefghijkefghij"
				"klfghijklmghijklmnhijklmnoijklmnopjklm"
				"nopqklmnopqrlmnopqrsmnopqrstnopqrstu";

static const unsigned message3len = 1000000;

/* SHA-1 */
TEST( SHA1fixture, SHA1 )
{
	static const char *testVectors[3] = {
				"A9993E364706816ABA3E25717850C26C9CD0D89D",
				"84983E441C3BD26EBAAE4AA1F95129E5E54670F1",
				"34AA973CD4C4DAA4F61EEB2BDBAD27316534016F"
	};

	unsigned char digest[ SHA1_DIGEST_SIZE ];
	char output[ 2 * SHA1_DIGEST_SIZE + 1 ];
	output[ 2 * SHA1_DIGEST_SIZE ] = '\0';

	unsigned char digest2[ SHA1_DIGEST_SIZE ];
	sha1_ctx ctx;
	char output2[ 2 * SHA1_DIGEST_SIZE + 1 ];
	output2[ 2 * SHA1_DIGEST_SIZE ] = '\0';

	sha1((const unsigned char *)message1, strlen( message1 ), digest );
	for ( int i = 0; i < SHA1_DIGEST_SIZE; i++ )
		sprintf( output + 2 * i, "%02X", digest[i] );
	EXPECT_STREQ( output, testVectors[0] );

	sha1((const unsigned char *)message2a, strlen( message2a ), digest );
	for ( int i = 0; i < SHA1_DIGEST_SIZE; i++ )
		sprintf( output + 2 * i, "%02X", digest[i] );
	EXPECT_STREQ( output, testVectors[1] );

	unsigned char* message3 = (unsigned char *)malloc( message3len );
	ASSERT_TRUE( message3 != NULL );
	memset( message3, 'a', message3len );
	sha1( message3, message3len, digest );
	for ( int i = 0; i < SHA1_DIGEST_SIZE; i++ )
		sprintf( output + 2 * i, "%02X", digest[i] );
	EXPECT_STREQ( output, testVectors[2] );

	sha1_init( &ctx );
	sha1_update( &ctx, message3, message3len / 2 );
	sha1_update( &ctx, message3 + message3len / 2, message3len - message3len / 2 );
	sha1_final( &ctx, digest2 );
	for ( int i = 0; i < SHA1_DIGEST_SIZE; i++ )
		sprintf( output2 + 2 * i, "%02X", digest2[i] );
	EXPECT_STREQ( output, output2 );

	free( message3 );
}

/* SHA-224 */
TEST( SHA2fixture, SHA224 )
{
	static const char *testVectors[3] = {
		"23097d223405d8228642a477bda255b32aadbce4bda0b3f7e36c9da7",
		"75388b16512776cc5dba5da1fd890150b0c6455cb4f58b1952522525",
		"20794655980c91d8bbb4c1ea97618a4bf03f42581948b2ee4ee7ad67"
	};

	unsigned char digest[ SHA224_DIGEST_SIZE ];
	char output[ 2 * SHA224_DIGEST_SIZE + 1 ];
	output[ 2 * SHA224_DIGEST_SIZE ] = '\0';

	unsigned char digest2[ SHA224_DIGEST_SIZE ];
	sha224_ctx ctx;
	char output2[ 2 * SHA224_DIGEST_SIZE + 1 ];
	output2[ 2 * SHA224_DIGEST_SIZE ] = '\0';

	sha224((const unsigned char *)message1, strlen( message1 ), digest );
	for ( int i = 0; i < SHA224_DIGEST_SIZE; i++ )
		sprintf( output + 2 * i, "%02x", digest[i] );
	EXPECT_STREQ( output, testVectors[0] );

	sha224((const unsigned char *)message2a, strlen( message2a ), digest );
	for ( int i = 0; i < SHA224_DIGEST_SIZE; i++ )
		sprintf( output + 2 * i, "%02x", digest[i] );
	EXPECT_STREQ( output, testVectors[1] );

	unsigned char* message3 = (unsigned char *)malloc( message3len );
	ASSERT_TRUE( message3 != NULL );
	memset( message3, 'a', message3len );
	sha224( message3, message3len, digest );
	for ( int i = 0; i < SHA224_DIGEST_SIZE; i++ )
		sprintf( output + 2 * i, "%02x", digest[i] );
	EXPECT_STREQ( output, testVectors[2] );

	sha224_init( &ctx );
	sha224_update( &ctx, message3, message3len / 2 );
	sha224_update( &ctx, message3 + message3len / 2, message3len - message3len / 2 );
	sha224_final( &ctx, digest2 );
	for ( int i = 0; i < SHA224_DIGEST_SIZE; i++ )
		sprintf( output2 + 2 * i, "%02x", digest2[i] );
	EXPECT_STREQ( output, output2 );

	free( message3 );
}

/* SHA-256 */
TEST( SHA2fixture, SHA256 )
{
	static const char *testVectors[3] = {
		"ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad",
		"248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1",
		"cdc76e5c9914fb9281a1c7e284d73e67f1809a48a497200e046d39ccc7112cd0"
	};

	unsigned char digest[ SHA256_DIGEST_SIZE ];
	char output[ 2 * SHA256_DIGEST_SIZE + 1 ];
	output[ 2 * SHA256_DIGEST_SIZE ] = '\0';

	unsigned char digest2[ SHA256_DIGEST_SIZE ];
	sha256_ctx ctx;
	char output2[ 2 * SHA256_DIGEST_SIZE + 1 ];
	output2[ 2 * SHA256_DIGEST_SIZE ] = '\0';

	sha256((const unsigned char *)message1, strlen( message1 ), digest );
	for ( int i = 0; i < SHA256_DIGEST_SIZE; i++ )
		sprintf( output + 2 * i, "%02x", digest[i] );
	EXPECT_STREQ( output, testVectors[0] );

	sha256((const unsigned char *)message2a, strlen( message2a ), digest );
	for ( int i = 0; i < SHA256_DIGEST_SIZE; i++ )
		sprintf( output + 2 * i, "%02x", digest[i] );
	EXPECT_STREQ( output, testVectors[1] );

	unsigned char* message3 = (unsigned char *)malloc( message3len );
	ASSERT_TRUE( message3 != NULL );
	memset( message3, 'a', message3len );
	sha256( message3, message3len, digest );
	for ( int i = 0; i < SHA256_DIGEST_SIZE; i++ )
		sprintf( output + 2 * i, "%02x", digest[i] );
	EXPECT_STREQ( output, testVectors[2] );

	sha256_init( &ctx );
	sha256_update( &ctx, message3, message3len / 2 );
	sha256_update( &ctx, message3 + message3len / 2, message3len - message3len / 2 );
	sha256_final( &ctx, digest2 );
	for ( int i = 0; i < SHA256_DIGEST_SIZE; i++ )
		sprintf( output2 + 2 * i, "%02x", digest2[i] );
	EXPECT_STREQ( output, output2 );

	free( message3 );
}

/* SHA-384 */
TEST( SHA2fixture, SHA384 )
{
	static const char *testVectors[3] = {
		"cb00753f45a35e8bb5a03d699ac65007272c32ab0eded1631a8b605a43ff5bed"
		"8086072ba1e7cc2358baeca134c825a7",
		"09330c33f71147e83d192fc782cd1b4753111b173b3b05d22fa08086e3b0f712"
		"fcc7c71a557e2db966c3e9fa91746039",
		"9d0e1809716474cb086e834e310a4a1ced149e9c00f248527972cec5704c2a5b"
		"07b8b3dc38ecc4ebae97ddd87f3d8985"
	};

	unsigned char digest[ SHA384_DIGEST_SIZE ];
	char output[ 2 * SHA384_DIGEST_SIZE + 1 ];
	output[ 2 * SHA384_DIGEST_SIZE ] = '\0';

	unsigned char digest2[ SHA384_DIGEST_SIZE ];
	sha384_ctx ctx;
	char output2[ 2 * SHA384_DIGEST_SIZE + 1 ];
	output2[ 2 * SHA384_DIGEST_SIZE ] = '\0';

	sha384((const unsigned char *)message1, strlen( message1 ), digest );
	for ( int i = 0; i < SHA384_DIGEST_SIZE; i++ )
		sprintf( output + 2 * i, "%02x", digest[i] );
	EXPECT_STREQ( output, testVectors[0] );

	sha384((const unsigned char *)message2b, strlen( message2b ), digest );
	for ( int i = 0; i < SHA384_DIGEST_SIZE; i++ )
		sprintf( output + 2 * i, "%02x", digest[i] );
	EXPECT_STREQ( output, testVectors[1] );

	unsigned char* message3 = (unsigned char *)malloc( message3len );
	ASSERT_TRUE( message3 != NULL );
	memset( message3, 'a', message3len );
	sha384( message3, message3len, digest );
	for ( int i = 0; i < SHA384_DIGEST_SIZE; i++ )
		sprintf( output + 2 * i, "%02x", digest[i] );
	EXPECT_STREQ( output, testVectors[2] );

	sha384_init( &ctx );
	sha384_update( &ctx, message3, message3len / 2 );
	sha384_update( &ctx, message3 + message3len / 2, message3len - message3len / 2 );
	sha384_final( &ctx, digest2 );
	for ( int i = 0; i < SHA384_DIGEST_SIZE; i++ )
		sprintf( output2 + 2 * i, "%02x", digest2[i] );
	EXPECT_STREQ( output, output2 );

	free( message3 );
}

/* SHA-512 */
TEST( SHA2fixture, SHA512 )
{
	static const char *testVectors[3] = {
		"ddaf35a193617abacc417349ae20413112e6fa4e89a97ea20a9eeee64b55d39a"
		"2192992a274fc1a836ba3c23a3feebbd454d4423643ce80e2a9ac94fa54ca49f",
		"8e959b75dae313da8cf4f72814fc143f8f7779c6eb9f7fa17299aeadb6889018"
		"501d289e4900f7e4331b99dec4b5433ac7d329eeb6dd26545e96e55b874be909",
		"e718483d0ce769644e2e42c7bc15b4638e1f98b13b2044285632a803afa973eb"
		"de0ff244877ea60a4cb0432ce577c31beb009c5c2c49aa2e4eadb217ad8cc09b"
	};

	unsigned char digest[ SHA512_DIGEST_SIZE ];
	char output[ 2 * SHA512_DIGEST_SIZE + 1 ];
	output[ 2 * SHA512_DIGEST_SIZE ] = '\0';

	unsigned char digest2[ SHA512_DIGEST_SIZE ];
	sha512_ctx ctx;
	char output2[ 2 * SHA512_DIGEST_SIZE + 1 ];
	output2[ 2 * SHA512_DIGEST_SIZE ] = '\0';

	sha512((const unsigned char *)message1, strlen( message1 ), digest );
	for ( int i = 0; i < SHA512_DIGEST_SIZE; i++ )
		sprintf( output + 2 * i, "%02x", digest[i] );
	EXPECT_STREQ( output, testVectors[0] );

	sha512((const unsigned char *)message2b, strlen( message2b ), digest );
	for ( int i = 0; i < SHA512_DIGEST_SIZE; i++ )
		sprintf( output + 2 * i, "%02x", digest[i] );
	EXPECT_STREQ( output, testVectors[1] );

	unsigned char* message3 = (unsigned char *)malloc( message3len );
	ASSERT_TRUE( message3 != NULL );
	memset( message3, 'a', message3len );
	sha512( message3, message3len, digest );
	for ( int i = 0; i < SHA512_DIGEST_SIZE; i++ )
		sprintf( output + 2 * i, "%02x", digest[i] );
	EXPECT_STREQ( output, testVectors[2] );

	sha512_init( &ctx );
	sha512_update( &ctx, message3, message3len / 2 );
	sha512_update( &ctx, message3 + message3len / 2, message3len - message3len / 2 );
	sha512_final( &ctx, digest2 );
	for ( int i = 0; i < SHA512_DIGEST_SIZE; i++ )
		sprintf( output2 + 2 * i, "%02x", digest2[i] );
	EXPECT_STREQ( output, output2 );

	free( message3 );
}

int main( int argc, char **argv )
{
	WOLFRAME_GTEST_REPORT( argv[0], refpath.string());
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
