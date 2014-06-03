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
#include "AAAA/passwordHash.hpp"
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


TEST_F( PasswdFileFixture, getUser )
{
	PasswordFile	pwdFile( "passwd", false );
	PwdFileUser	user;
	bool result;

	result = pwdFile.getUser( "admin", user );
	EXPECT_FALSE( result );

	result = pwdFile.getUser( "admin", user, false );
	ASSERT_TRUE( result );
	EXPECT_STREQ( "Admin", user.user.c_str() );
	EXPECT_STREQ( "Just a test user", user.info.c_str() );

	result = pwdFile.getUser( "Admin", user );
	ASSERT_TRUE( result );
	EXPECT_STREQ( "Admin", user.user.c_str() );
	EXPECT_STREQ( "Just a test user", user.info.c_str() );
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

	HMAC_SHA256	hmac0( salt.salt(), salt.size(), "admin" );
	result = pwdFile.getHMACuser( hmac0.toString(), salt.toString(), user, false );
	ASSERT_TRUE( result );
	EXPECT_STREQ( "Admin", user.user.c_str() );
	EXPECT_STREQ( "Just a test user", user.info.c_str() );

	result = pwdFile.getHMACuser( hmac0.toString(), salt.toString(), user, true );
	EXPECT_FALSE( result );

	HMAC_SHA256	hmac1( salt.salt(), salt.size(), "Admin" );
	result = pwdFile.getHMACuser( hmac1.toString(), salt.toString(), user, true );
	ASSERT_TRUE( result );
	EXPECT_STREQ( "Admin", user.user.c_str() );
	EXPECT_STREQ( "Just a test user", user.info.c_str() );
}

//****************************************************************************

int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS( );
}
