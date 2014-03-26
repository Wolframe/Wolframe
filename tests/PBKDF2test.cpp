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
// PBKDF2 tests
//

#include "gtest/gtest.h"
#include "crypto/PBKDF2.hpp"

using namespace _Wolframe::crypto;

TEST( PBKDF2, TestVectors )
{
	// RFC 6070 test vectors
	const char *testVec1 = "0c60c80f961f0e71f3a9b524af6012062fe037a6";
	const char *testVec2 = "ea6c014dc72d6f8ccd1ed92ace1d41f0d8de8957";
	const char* testVec3 = "4b007901b765489abead49d926f721d065a429c1";
	const char* testVec4 = "eefe3d61cd4da4e4e9945b3d6ba2158c2634e984";
	const char* testVec5 = "3d2eec4fe41c849b80c8d83662c0e44a8b291a964cf2f07038";
	const char* testVec6 = "56fa6aa75548099dcc37d7f03425e0c3";

	PBKDF2_HMAC_SHA1 test1( "salt", "password", 20, 1 );
	PBKDF2_HMAC_SHA1 test1_0( test1.toString());
	PBKDF2_HMAC_SHA1 test1_1( (const unsigned char*)"salt", 4, (const unsigned char*)"password", 8, 20, 1 );
	PBKDF2_HMAC_SHA1 test1_2( "salt", (const unsigned char*)"password", 8, 20, 1 );
	PBKDF2_HMAC_SHA1 test1_3( (const unsigned char*)"salt", 4, "password", 20, 1 );


	PBKDF2_HMAC_SHA1 test2( "salt", "password", 20, 2 );
	PBKDF2_HMAC_SHA1 test2_0( test2.toString());

	PBKDF2_HMAC_SHA1 test3( "salt", "password", 20, 4096 );
	PBKDF2_HMAC_SHA1 test3_0( test3.toString());

	PBKDF2_HMAC_SHA1 test4( "salt", "password", 20, 16777216 );
	PBKDF2_HMAC_SHA1 test4_0( test4.toString());

	PBKDF2_HMAC_SHA1 test5( "saltSALTsaltSALTsaltSALTsaltSALTsalt", "passwordPASSWORDpassword", 25, 4096 );
	PBKDF2_HMAC_SHA1 test5_0( test5.toString());

	PBKDF2_HMAC_SHA1 test6( (const unsigned char*)"sa\0lt", 5, (const unsigned char*)"pass\0word", 9, 16, 4096 );
	PBKDF2_HMAC_SHA1 test6_0( test6.toString());

	EXPECT_STRCASEEQ( testVec1, test1.toBCD().c_str() );
	EXPECT_TRUE( test1 == test1_0.toString() );
	EXPECT_TRUE( test1 == test1_0 );
	EXPECT_TRUE( test1 == test1_1 );
	EXPECT_TRUE( test1 == test1_2 );
	EXPECT_TRUE( test1 == test1_3 );

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
}

int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}


