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
// paswword file manipulation tests
//

#include "logger-v1.hpp"
#include "gtest/gtest.h"
#include "passwdFile.hpp"
#include "system/globalRngGen.hpp"
#include "AAAA/password.hpp"
#include "crypto/HMAC.hpp"

using namespace _Wolframe::AAAA;
using namespace _Wolframe::crypto;
using namespace std;

// The fixture for testing class _Wolframe::module
class PasswdFileFixture : public ::testing::Test
{
protected:
	PasswdFileFixture( )
	{
		_Wolframe::GlobalRandomGenerator::instance( "" );
	}
};


TEST_F( PasswdFileFixture, PasswordSaltTest )
{
	unsigned char		salt[ PASSWORD_SALT_SIZE ];
	PasswordHash::Salt	salt0;

	EXPECT_EQ( salt0.size(), 0 );
	EXPECT_STREQ( "", salt0.toBCD().c_str() );
	EXPECT_STREQ( "", salt0.toString().c_str() );

	for ( size_t i = 0; i < PASSWORD_SALT_SIZE; i++ )
		salt[ i ] = 0;
	PasswordHash::Salt	salt1( salt, PASSWORD_SALT_SIZE );
	PasswordHash::Salt	salt1_0( "AAAAAAAAAAAAAAAAAAAAAA" );
	EXPECT_EQ( salt1.size(), 16 );
	EXPECT_STRCASEEQ( "00000000000000000000000000000000", salt1.toBCD().c_str() );
	EXPECT_STRCASEEQ( "AAAAAAAAAAAAAAAAAAAAAA", salt1.toString().c_str() );
	EXPECT_TRUE( salt1 == salt1_0 );

	for ( size_t i = 0; i < PASSWORD_SALT_SIZE; i++ )
		salt[ i ] = i;
	PasswordHash::Salt	salt2( salt, PASSWORD_SALT_SIZE );
	EXPECT_EQ( salt2.size(), PASSWORD_SALT_SIZE );
	EXPECT_STRCASEEQ( "000102030405060708090a0b0c0d0e0f", salt2.toBCD().c_str() );
	EXPECT_STRCASEEQ( "AAECAwQFBgcICQoLDA0ODw", salt2.toString().c_str() );

	for ( size_t i = 0; i < PASSWORD_SALT_SIZE; i++ )
		salt[ i ] = 0xff;
	PasswordHash::Salt	salt3( salt, PASSWORD_SALT_SIZE );
	EXPECT_EQ( salt3.size(), PASSWORD_SALT_SIZE );
	EXPECT_STRCASEEQ( "ffffffffffffffffffffffffffffffff", salt3.toBCD().c_str() );
	EXPECT_STRCASEEQ( "/////////////////////w", salt3.toString().c_str() );
}

TEST_F( PasswdFileFixture, PasswordHashTest )
{
	PasswordHash	hash0( "$AAAAAAAAAAAAAAAAAAAAAA$AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");

	EXPECT_STREQ( "00000000000000000000000000000000", hash0.salt().toBCD().c_str() );
	EXPECT_EQ( 0, hash0.hash().hash()[0] );
	std::cout << hash0.toString();
}


TEST_F( PasswdFileFixture, getUser )
{
	PasswordFile	pwdFile( "passwd", false );
	PwdFileUser	user;
	bool result;

	result = pwdFile.getUser( "Admin", user );
	EXPECT_FALSE( result );

	result = pwdFile.getUser( "admin", user );
	ASSERT_TRUE( result );
	EXPECT_STREQ( "admin", user.user.c_str() );
	EXPECT_STREQ( "1841bac2def7cf53a978f0414aa8d5c3e7c4618899709c84fedcdcd6", user.hash.c_str() );
	EXPECT_STREQ( "Wolframe Administrator", user.info.c_str() );
}

TEST_F( PasswdFileFixture, getHMACuser )
{
	PasswordFile		pwdFile( "passwd", false );
	PwdFileUser		user;
	bool			result;

	_Wolframe::GlobalRandomGenerator& rnd = _Wolframe::GlobalRandomGenerator::instance();
	unsigned char saltData[ PASSWORD_SALT_SIZE ];
	rnd.generate( saltData, PASSWORD_SALT_SIZE );
	PasswordHash::Salt	salt( saltData, PASSWORD_SALT_SIZE );

	HMAC_SHA256	hmac0( salt.salt(), salt.size(), "Admin" );
	result = pwdFile.getHMACuser( hmac0.toString(), salt.toString(), user, true );
	EXPECT_FALSE( result );

	HMAC_SHA256	hmac1( salt.salt(), salt.size(), "admin" );
	result = pwdFile.getHMACuser( hmac1.toString(), salt.toString(), user, false );
	ASSERT_TRUE( result );
	EXPECT_STREQ( "admin", user.user.c_str() );
	EXPECT_STREQ( "1841bac2def7cf53a978f0414aa8d5c3e7c4618899709c84fedcdcd6", user.hash.c_str() );
	EXPECT_STREQ( "Wolframe Administrator", user.info.c_str() );

	result = pwdFile.getHMACuser( hmac1.toString(), salt.toString(), user, true );
	ASSERT_TRUE( result );
	EXPECT_STREQ( "admin", user.user.c_str() );
	EXPECT_STREQ( "1841bac2def7cf53a978f0414aa8d5c3e7c4618899709c84fedcdcd6", user.hash.c_str() );
	EXPECT_STREQ( "Wolframe Administrator", user.info.c_str() );

	result = pwdFile.getHMACuser( hmac1.toString(), salt.toString(), user, false );
	ASSERT_TRUE( result );
	EXPECT_STREQ( "admin", user.user.c_str() );
	EXPECT_STREQ( "1841bac2def7cf53a978f0414aa8d5c3e7c4618899709c84fedcdcd6", user.hash.c_str() );
	EXPECT_STREQ( "Wolframe Administrator", user.info.c_str() );
}
//****************************************************************************

int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS( );
}
