/************************************************************************

 Copyright (C) 2011, 2012 Project Wolframe.
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
#include "AAAA/HMAC.hpp"

using namespace _Wolframe::AAAA;

TEST( HMAC, TestVectors )
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
	HMAC_SHA256	test2( "key", "The quick brown fox jumps over the lazy dog" );

	unsigned char	key3[ 20 ];
	memset( key3, 0x0b, 20 );
	HMAC_SHA256	test3( key3, 20, "Hi There" );

	HMAC_SHA256	test4( "Jefe", "what do ya want for nothing?" );

	unsigned char	key5[ 20 ];
	memset( key5, 0xaa, 20 );
	unsigned char	data5[ 50 ];
	memset( data5, 0xdd, 50 );
	HMAC_SHA256	test5( key5, 20, data5, 50 );

	unsigned char	key6[ 25 ];
	for ( unsigned char i = 0; i < 25; i++ ) key6[ i ] = i + 1;
	unsigned char	data6[ 50 ];
	memset( data6, 0xcd, 50 );
	HMAC_SHA256	test6( key6, 25, data6, 50 );

	unsigned char	key7[ 131 ];
	memset( key7, 0xaa, 131 );
	HMAC_SHA256	test7( key7, 131, "Test Using Larger Than Block-Size Key - Hash Key First" );

	unsigned char	key8[ 131 ];
	memset( key8, 0xaa, 131 );
	HMAC_SHA256	test8( key8, 131, "This is a test using a larger than block-size key and a larger than block-size data. The key needs to be hashed before being used by the HMAC algorithm." );

	EXPECT_STRCASEEQ( testVec1, test1.toBCD().c_str() );
	EXPECT_TRUE( test1 == testVec1 );

	EXPECT_STRCASEEQ( testVec2, test2.toBCD().c_str() );
	EXPECT_TRUE( test2 == testVec2 );

	EXPECT_STRCASEEQ( testVec3, test3.toBCD().c_str() );
	EXPECT_TRUE( test3 == testVec3 );

	EXPECT_STRCASEEQ( testVec4, test4.toBCD().c_str() );
	EXPECT_TRUE( test4 == testVec4 );

	EXPECT_STRCASEEQ( testVec5, test5.toBCD().c_str() );
	EXPECT_TRUE( test5 == testVec5 );

	EXPECT_STRCASEEQ( testVec6, test6.toBCD().c_str() );
	EXPECT_TRUE( test5 == testVec5 );

	EXPECT_STRCASEEQ( testVec7, test7.toBCD().c_str() );
	EXPECT_TRUE( test5 == testVec5 );

	EXPECT_STRCASEEQ( testVec8, test8.toBCD().c_str() );
	EXPECT_TRUE( test5 == testVec5 );
}

int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}


