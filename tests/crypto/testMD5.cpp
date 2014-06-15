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
// MD5 unit tests
//

#include "gtest/gtest.h"
#include "wtest/testReport.hpp"
#include <cstring>
#include "crypto/md5.h"

TEST( MD5fixture, md5 )
{
	unsigned char digest[ MD5_DIGEST_SIZE ];
	char output[ 2 * MD5_DIGEST_SIZE + 1 ];
	output[ 2 * MD5_DIGEST_SIZE ] = '\0';
	const char* message;

//	message="" (empty string), hash=D41D8CD98F00B204E9800998ECF8427E
	message = "";
	md5((const unsigned char *)message, strlen( message ), digest );
	for ( int i = 0; i < MD5_DIGEST_SIZE; i++ )
		sprintf( output + 2 * i, "%02x", digest[i] );
	EXPECT_STRCASEEQ( "D41D8CD98F00B204E9800998ECF8427E", output );

// message="a", hash=0CC175B9C0F1B6A831C399E269772661
	message = "a";
	md5((const unsigned char *)message, strlen( message ), digest );
	for ( int i = 0; i < MD5_DIGEST_SIZE; i++ )
		sprintf( output + 2 * i, "%02x", digest[i] );
	EXPECT_STRCASEEQ( "0CC175B9C0F1B6A831C399E269772661", output );

// message="abc", hash=900150983CD24FB0D6963F7D28E17F72
	message = "abc";
	md5((const unsigned char *)message, strlen( message ), digest );
	for ( int i = 0; i < MD5_DIGEST_SIZE; i++ )
		sprintf( output + 2 * i, "%02x", digest[i] );
	EXPECT_STRCASEEQ( "900150983CD24FB0D6963F7D28E17F72", output );

// message="message digest", hash=F96B697D7CB7938D525A2F31AAF161D0
	message = "message digest";
	md5((const unsigned char *)message, strlen( message ), digest );
	for ( int i = 0; i < MD5_DIGEST_SIZE; i++ )
		sprintf( output + 2 * i, "%02x", digest[i] );
	EXPECT_STRCASEEQ( "F96B697D7CB7938D525A2F31AAF161D0", output );

// message="abcdefghijklmnopqrstuvwxyz", hash=C3FCD3D76192E4007DFB496CCA67E13B
	message = "abcdefghijklmnopqrstuvwxyz";
	md5((const unsigned char *)message, strlen( message ), digest );
	for ( int i = 0; i < MD5_DIGEST_SIZE; i++ )
		sprintf( output + 2 * i, "%02x", digest[i] );
	EXPECT_STRCASEEQ( "C3FCD3D76192E4007DFB496CCA67E13B", output );

// message="abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
// hash=8215EF0796A20BCAAAE116D3876C664A
	message = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
	md5((const unsigned char *)message, strlen( message ), digest );
	for ( int i = 0; i < MD5_DIGEST_SIZE; i++ )
		sprintf( output + 2 * i, "%02x", digest[i] );
	EXPECT_STRCASEEQ( "8215EF0796A20BCAAAE116D3876C664A", output );

// message="A...Za...z0...9", hash=D174AB98D277D9F5A5611C2C9F419D9F
	message = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
	md5((const unsigned char *)message, strlen( message ), digest );
	for ( int i = 0; i < MD5_DIGEST_SIZE; i++ )
		sprintf( output + 2 * i, "%02x", digest[i] );
	EXPECT_STRCASEEQ( "D174AB98D277D9F5A5611C2C9F419D9F", output );

// message=8 times "1234567890", hash=57EDF4A22BE3C955AC49DA2E2107B67A
	message = "12345678901234567890123456789012345678901234567890123456789012345678901234567890";
	md5((const unsigned char *)message, strlen( message ), digest );
	for ( int i = 0; i < MD5_DIGEST_SIZE; i++ )
		sprintf( output + 2 * i, "%02x", digest[i] );
	EXPECT_STRCASEEQ( "57EDF4A22BE3C955AC49DA2E2107B67A", output );

// message = 1 million times "a", hash=7707D6AE4E027C70EEA2A935C2296F21
	static const unsigned longMessageLen = 1000000;
	unsigned char* longMessage = (unsigned char *)malloc( longMessageLen );
	EXPECT_TRUE( longMessage != NULL );
	memset( longMessage, 'a', longMessageLen );
	md5( longMessage, longMessageLen, digest );
	for ( int i = 0; i < MD5_DIGEST_SIZE; i++ )
		sprintf( output + 2 * i, "%02x", digest[i] );
	EXPECT_STRCASEEQ( output, "7707D6AE4E027C70EEA2A935C2296F21" );

	md5_ctx ctx;
	md5_init( &ctx );
	md5_update( &ctx, longMessage, longMessageLen / 2 );
	md5_update( &ctx, longMessage + longMessageLen / 2, longMessageLen - longMessageLen / 2 );
	md5_final( digest, &ctx );
	for ( int i = 0; i < MD5_DIGEST_SIZE; i++ )
		sprintf( output + 2 * i, "%02x", digest[i] );
	EXPECT_STRCASEEQ( "7707D6AE4E027C70EEA2A935C2296F21", output );

// message=128 zero bits, hash=4AE71336E44BF9BF79D2752E234818A5
	memset( longMessage, 0, 128 / 8 );
	md5( longMessage, 128 / 8, digest );
	for ( int i = 0; i < MD5_DIGEST_SIZE; i++ )
		sprintf( output + 2 * i, "%02x", digest[i] );
	EXPECT_STRCASEEQ( output, "4AE71336E44BF9BF79D2752E234818A5" );

//// iterated 100000 times, hash=1A83F51285E4D89403D00C46EF8508FE
//	MD5_Init( &ctx );
//	for ( int i = 0; i < 100000; i++ )
//		MD5_Update( &ctx, longMessage, 128 / 8 );
//	MD5_Final( digest, &ctx );
//	for ( int i = 0; i < MD5_DIGEST_SIZE; i++ )
//		sprintf( output + 2 * i, "%02x", digest[i] );
//	EXPECT_STRCASEEQ( "1A83F51285E4D89403D00C46EF8508FE", output );
}


int main( int argc, char **argv )
{
	WOLFRAME_GTEST_REPORT( argv[0], refpath.string());
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
