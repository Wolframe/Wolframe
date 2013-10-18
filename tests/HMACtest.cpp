/************************************************************************

 Copyright (C) 2011 - 2013 Project Wolframe.
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
// HMAC tests
//

#include "gtest/gtest.h"
#include "crypto/HMAC.hpp"

using namespace _Wolframe::AAAA;

TEST( _HMAC_SHA1_, TestVectors )
{
	// Wikipedia test vectors
	const char *testVec1 = "fbdb1d1b18aa6c08324b7d64b71fb76370690e1d";
	const char *testVec2 = "de7c9b85b8b78aa6bc8a7a36f70a90701c9db4d9";
	// RFC 2202 test vectors
	const char* testVec3 = "b617318655057264e28bc0b6fb378c8ef146be00";
	const char* testVec4 = "effcdf6ae5eb2fa2d27416d5f184df9c259a7c79";
	const char* testVec5 = "125d7342b9ac11cd91a39af48aa17b4f63f175d3";
	const char* testVec6 = "4c9007f4026250c6bc8414f9bf50c86c2d7235da";
	const char* testVec7 = "4c1a03424b55e07fe7f27be1d58bb9324a9a5a04";
	const char* testVec8 = "aa4ae5e15272d00e95705637ce8a3b55ed402112";
	const char* testVec9 = "e8e99d0f45237d786d6bbaa7965c7808bbff1a91";

	HMAC_SHA1	test1( "", "" );
	HMAC_SHA1	test1_0( test1.toString());

	HMAC_SHA1	test2( "key", "The quick brown fox jumps over the lazy dog" );
	HMAC_SHA1	test2_0( test2.toString());

	unsigned char	key3[ 20 ];
	memset( key3, 0x0b, 20 );
	HMAC_SHA1	test3( key3, 20, "Hi There" );
	HMAC_SHA1	test3_0( test3.toString());

	HMAC_SHA1	test4( "Jefe", "what do ya want for nothing?" );
	HMAC_SHA1	test4_0( test4.toString());

	unsigned char	key5[ 20 ];
	memset( key5, 0xaa, 20 );
	unsigned char	data5[ 50 ];
	memset( data5, 0xdd, 50 );
	HMAC_SHA1	test5( key5, 20, data5, 50 );
	HMAC_SHA1	test5_0( test5.toString());

	unsigned char	key6[ 25 ];
	for ( unsigned char i = 0; i < 25; i++ ) key6[ i ] = i + 1;
	unsigned char	data6[ 50 ];
	memset( data6, 0xcd, 50 );
	HMAC_SHA1	test6( key6, 25, data6, 50 );
	HMAC_SHA1	test6_0( test6.toString());

	unsigned char	key7[ 20 ];
	memset( key7, 0x0c, 20 );
	HMAC_SHA1	test7( key7, 20, "Test With Truncation" );
	HMAC_SHA1	test7_0( test7.toString());

	unsigned char	key8[ 80 ];
	memset( key8, 0xaa, 80 );
	HMAC_SHA1	test8( key8, 80, "Test Using Larger Than Block-Size Key - Hash Key First" );
	HMAC_SHA1	test8_0( test8.toString());

	unsigned char	key9[ 80 ];
	memset( key9, 0xaa, 80 );
	HMAC_SHA1	test9( key9, 80, "Test Using Larger Than Block-Size Key and Larger Than One Block-Size Data" );
	HMAC_SHA1	test9_0( test9.toString());

	EXPECT_STRCASEEQ( testVec1, test1.toBCD().c_str() );
	EXPECT_TRUE( test1 == test1_0.toString() );
	EXPECT_TRUE( test1 == test1_0 );

	EXPECT_STRCASEEQ( testVec2, test2.toBCD().c_str() );
	EXPECT_TRUE( test2 == test2_0.toString() );
	EXPECT_TRUE( test2 == test2_0 );

	EXPECT_STRCASEEQ( testVec3, test3.toBCD().c_str() );
	EXPECT_TRUE( test3 == test3_0.toString() );
	EXPECT_TRUE( test3 == test3_0 );

	EXPECT_STRCASEEQ( testVec4, test4.toBCD().c_str() );
	EXPECT_TRUE( test4 == test4_0.toString() );
	EXPECT_TRUE( test4 == test4_0 );

	EXPECT_STRCASEEQ( testVec5, test5.toBCD().c_str() );
	EXPECT_TRUE( test5 == test5_0.toString() );
	EXPECT_TRUE( test5 == test5_0 );

	EXPECT_STRCASEEQ( testVec6, test6.toBCD().c_str() );
	EXPECT_TRUE( test6 == test6_0.toString() );
	EXPECT_TRUE( test6 == test6_0 );

	EXPECT_STRCASEEQ( testVec7, test7.toBCD().c_str() );
	EXPECT_TRUE( test7 == test7_0.toString() );
	EXPECT_TRUE( test7 == test7_0 );

	EXPECT_STRCASEEQ( testVec8, test8.toBCD().c_str() );
	EXPECT_TRUE( test8 == test8_0.toString() );
	EXPECT_TRUE( test8 == test8_0 );

	EXPECT_STRCASEEQ( testVec9, test9.toBCD().c_str() );
	EXPECT_TRUE( test9 == test9_0.toString() );
	EXPECT_TRUE( test9 == test9_0 );
}

TEST( _HMAC_SHA256_, TestVectors )
{
	// Wikipedia test vectors
	const char *testVec1 = "b613679a0814d9ec772f95d778c35fc5ff1697c493715653c6c712144292c5ad";
	const char *testVec2 = "f7bc83f430538424b13298e6aa6fb143ef4d59a14946175997479dbc2d1a3cd8";
	// RFC 4231 test vectors
	const char* testVec3 = "b0344c61d8db38535ca8afceaf0bf12b881dc200c9833da726e9376c2e32cff7";
	const char* testVec4 = "5bdcc146bf60754e6a042426089575c75a003f089d2739839dec58b964ec3843";
	const char* testVec5 = "773ea91e36800e46854db8ebd09181a72959098b3ef8c122d9635514ced565fe";
	const char* testVec6 = "82558a389a443c0ea4cc819899f2083a85f0faa3e578f8077a2e3ff46729665b";
	const char* testVec7 = "60e431591ee0b67f0d8a26aacbf5b77f8e0bc6213728c5140546040f0ee37f54";
	const char* testVec8 = "9b09ffa71b942fcb27635fbcd5b0e944bfdc63644f0713938a7f51535c3a35e2";

	HMAC_SHA256	test1( "", "" );
	HMAC_SHA256	test1_0( test1.toString());

	HMAC_SHA256	test2( "key", "The quick brown fox jumps over the lazy dog" );
	HMAC_SHA256	test2_0( test2.toString());

	unsigned char	key3[ 20 ];
	memset( key3, 0x0b, 20 );
	HMAC_SHA256	test3( key3, 20, "Hi There" );
	HMAC_SHA256	test3_0( test3.toString());

	HMAC_SHA256	test4( "Jefe", "what do ya want for nothing?" );
	HMAC_SHA256	test4_0( test4.toString());

	unsigned char	key5[ 20 ];
	memset( key5, 0xaa, 20 );
	unsigned char	data5[ 50 ];
	memset( data5, 0xdd, 50 );
	HMAC_SHA256	test5( key5, 20, data5, 50 );
	HMAC_SHA256	test5_0( test5.toString());

	unsigned char	key6[ 25 ];
	for ( unsigned char i = 0; i < 25; i++ ) key6[ i ] = i + 1;
	unsigned char	data6[ 50 ];
	memset( data6, 0xcd, 50 );
	HMAC_SHA256	test6( key6, 25, data6, 50 );
	HMAC_SHA256	test6_0( test6.toString());

	unsigned char	key7[ 131 ];
	memset( key7, 0xaa, 131 );
	HMAC_SHA256	test7( key7, 131, "Test Using Larger Than Block-Size Key - Hash Key First" );
	HMAC_SHA256	test7_0( test7.toString());

	unsigned char	key8[ 131 ];
	memset( key8, 0xaa, 131 );
	HMAC_SHA256	test8( key8, 131, "This is a test using a larger than block-size key and a larger than block-size data. The key needs to be hashed before being used by the HMAC algorithm." );
	HMAC_SHA256	test8_0( test8.toString());

	EXPECT_STRCASEEQ( testVec1, test1.toBCD().c_str() );
	EXPECT_TRUE( test1 == test1_0.toString() );
	EXPECT_TRUE( test1 == test1_0 );

	EXPECT_STRCASEEQ( testVec2, test2.toBCD().c_str() );
	EXPECT_TRUE( test2 == test2_0.toString() );
	EXPECT_TRUE( test2 == test2_0 );

	EXPECT_STRCASEEQ( testVec3, test3.toBCD().c_str() );
	EXPECT_TRUE( test3 == test3_0.toString() );
	EXPECT_TRUE( test3 == test3_0 );

	EXPECT_STRCASEEQ( testVec4, test4.toBCD().c_str() );
	EXPECT_TRUE( test4 == test4_0.toString() );
	EXPECT_TRUE( test4 == test4_0 );

	EXPECT_STRCASEEQ( testVec5, test5.toBCD().c_str() );
	EXPECT_TRUE( test5 == test5_0.toString() );
	EXPECT_TRUE( test5 == test5_0 );

	EXPECT_STRCASEEQ( testVec6, test6.toBCD().c_str() );
	EXPECT_TRUE( test6 == test6_0.toString() );
	EXPECT_TRUE( test6 == test6_0 );

	EXPECT_STRCASEEQ( testVec7, test7.toBCD().c_str() );
	EXPECT_TRUE( test7 == test7_0.toString() );
	EXPECT_TRUE( test7 == test7_0 );

	EXPECT_STRCASEEQ( testVec8, test8.toBCD().c_str() );
	EXPECT_TRUE( test8 == test8_0.toString() );
	EXPECT_TRUE( test8 == test8_0 );
}

int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}


