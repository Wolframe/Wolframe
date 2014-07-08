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
// text file authenticator tests
//

#include "logger-v1.hpp"
#include "gtest/gtest.h"

#include "TextFileAuth.hpp"
#include "passwdFile.hpp"		// for USERNAME_DEFAULT_CASE_SENSIVE
#include "types/base64.hpp"
#include "crypto/HMAC.hpp"
#include "system/globalRngGen.hpp"
#include "AAAA/passwordHash.hpp"
#include "AAAA/CRAM.hpp"

#include <boost/algorithm/string.hpp>

using namespace _Wolframe::AAAA;
using namespace _Wolframe::log;
using namespace _Wolframe;
using namespace std;


// The fixture for testing
class AuthenticationFixture : public ::testing::Test
{
protected:
	LogBackend& logBack;

	AuthenticationFixture( ) :
		logBack( LogBackend::instance( ) )
	{
		// Initialize the global random number generator
		_Wolframe::GlobalRandomGenerator::instance( "" );

//		logBack.setConsoleLevel( LogLevel::LOGLEVEL_DATA );
		logBack.setConsoleLevel( LogLevel::LOGLEVEL_INFO );
	}
};


TEST_F( AuthenticationFixture, typeName )
{
	TextFileAuthUnit authenticator( "", "passwd" );
	EXPECT_STREQ( authenticator.className( ), "TextFileAuth" );
}

TEST_F( AuthenticationFixture, fileWithoutNewLine )
{
	User*	user;
	TextFileAuthUnit authenticator( "", "passwd-noNL" );

	user = authenticator.authenticatePlain( "Admin", "Good Password" );
	ASSERT_TRUE( user != NULL );
	EXPECT_STREQ( "Admin", user->uname().c_str() );
	EXPECT_STREQ( "Just a test user", user->name().c_str() );
	delete user;

	user = authenticator.authenticatePlain( "goodusr", "User PassWord" );
	ASSERT_TRUE( user != NULL );
	EXPECT_STREQ( "goodusr", user->uname().c_str() );
	EXPECT_STREQ( "Another test user", user->name().c_str() );
	delete user;

	user = authenticator.authenticatePlain( "testusr", "UserPassWord" );
	ASSERT_TRUE( user != NULL );
	EXPECT_STREQ( "testusr", user->uname().c_str() );
	EXPECT_STREQ( "Good test user", user->name().c_str() );
	delete user;
}

TEST_F( AuthenticationFixture, validUsers )
{
	User*	user;
	TextFileAuthUnit authenticator( "", "passwd" );

	user = authenticator.authenticatePlain( "Admin", "Good Password" );
	ASSERT_TRUE( user != NULL );
	EXPECT_STREQ( "Admin", user->uname().c_str() );
	EXPECT_STREQ( "Just a test user", user->name().c_str() );
	delete user;

	user = authenticator.authenticatePlain( "goodusr", "User PassWord" );
	ASSERT_TRUE( user != NULL );
	EXPECT_STREQ( "goodusr", user->uname().c_str() );
	EXPECT_STREQ( "Another test user", user->name().c_str() );
	delete user;

	user = authenticator.authenticatePlain( "testusr", "UserPassWord" );
	ASSERT_TRUE( user != NULL );
	EXPECT_STREQ( "testusr", user->uname().c_str() );
	EXPECT_STREQ( "Good test user", user->name().c_str() );
	delete user;
}


TEST_F( AuthenticationFixture, DISABLED_caseInsensitive_Pass )
{
	User*	user;
	TextFileAuthUnit authenticator( "", "passwd" );

	user = authenticator.authenticatePlain( "AdMiN", "Good Password" );
	ASSERT_TRUE( user != NULL );
	EXPECT_STREQ( "Admin", user->uname().c_str() );
	EXPECT_STREQ( "Just a test user", user->name().c_str() );
	delete user;

	user = authenticator.authenticatePlain( "GoodUsr", "User PassWord" );
	ASSERT_TRUE( user != NULL );
	EXPECT_STREQ( "goodusr", user->uname().c_str() );
	EXPECT_STREQ( "Another test user", user->name().c_str() );
	delete user;

	user = authenticator.authenticatePlain( "TestUsr", "UserPassWord" );
	ASSERT_TRUE( user != NULL );
	EXPECT_STREQ( "testusr", user->uname().c_str() );
	EXPECT_STREQ( "Good test user", user->name().c_str() );
	delete user;
}

TEST_F( AuthenticationFixture, caseInsensitive_Fail )
{
	User*	user;
	TextFileAuthUnit authenticator( "", "passwd" );

	user = authenticator.authenticatePlain( "AdMiN", "Good Password" );
	EXPECT_EQ( NULL, user );
	user = authenticator.authenticatePlain( "GoodUsr", "User PassWord" );
	EXPECT_EQ( NULL, user );
	user = authenticator.authenticatePlain( "BadUsr", "UserPassWord" );
	EXPECT_EQ( NULL, user );
}

TEST_F( AuthenticationFixture, wrongPasswords )
{
	User*	user;
	TextFileAuthUnit authenticator( "", "passwd" );

	user = authenticator.authenticatePlain( "Admin", "Goood Password" );
	EXPECT_EQ( NULL, user );
	user = authenticator.authenticatePlain( "goodusr", "User Password" );
	EXPECT_EQ( NULL, user );
	user = authenticator.authenticatePlain( "badusr", "userPassWord" );
	EXPECT_EQ( NULL, user );
}

TEST_F( AuthenticationFixture, nonExistentUsers )
{
	User*	user;
	TextFileAuthUnit authenticator( "", "passwd" );

	user = authenticator.authenticatePlain( "adminn", "xx" );
	EXPECT_EQ( NULL, user );
	user = authenticator.authenticatePlain( "gooduser", "xx" );
	EXPECT_EQ( NULL, user );
	user = authenticator.authenticatePlain( "baduser", "xx" );
	EXPECT_EQ( NULL, user );
}

TEST_F( AuthenticationFixture, invalidPasswordHashes )
{
	TextFileAuthUnit authenticator( "", "passwd" );

	EXPECT_THROW( authenticator.authenticatePlain( "shortusr", "User Password" ),
		      std::runtime_error );

	EXPECT_THROW( authenticator.authenticatePlain( "longusr", "User Password" ),
		      std::runtime_error );

}

TEST_F( AuthenticationFixture, nonexistentFile )
{
	TextFileAuthUnit authenticator( "", "passwds" );

	EXPECT_THROW( authenticator.authenticatePlain( "admin", "xx" ),
		      std::runtime_error );
	EXPECT_THROW( authenticator.authenticatePlain( "goodusr", "xx" ),
		      std::runtime_error );
	EXPECT_THROW( authenticator.authenticatePlain( "testusr", "xx" ),
		      std::runtime_error );
}


static std::string usernameHash( const std::string& username,
				 bool caseSensitive = USERNAME_DEFAULT_CASE_SENSIVE )
{
	_Wolframe::GlobalRandomGenerator& rnd = _Wolframe::GlobalRandomGenerator::instance();
	unsigned char salt[ PASSWORD_SALT_SIZE ];

	rnd.generate( salt, PASSWORD_SALT_SIZE );

	std::string uname;
	if ( caseSensitive )
		uname = username;
	else
		uname = boost::to_lower_copy( username );

	crypto::HMAC_SHA256 hmac0( salt, PASSWORD_SALT_SIZE, uname );

	char saltStr[ 2 * PASSWORD_SALT_SIZE ];
	base64::encode( salt, PASSWORD_SALT_SIZE, saltStr, 2 * PASSWORD_SALT_SIZE, 0 );

	return "$" + std::string( saltStr ) + "$" + hmac0.toString();
}

TEST_F( AuthenticationFixture, AuthenticationSuccess )
{
	User* user = NULL;
	TextFileAuthUnit authUnit( "test", "passwd" );
	AAAA::AuthenticatorSlice* slice = authUnit.slice( "WOLFRAME-CRAM", net::RemoteTCPendpoint( "localhost", 2222 ));

	ASSERT_TRUE( slice != NULL );

	EXPECT_EQ( slice->status(), AAAA::AuthenticatorSlice::AWAITING_MESSAGE );

	std::string userHash = usernameHash( "Admin" );
	std::cout << "User hash: " << userHash << std::endl;
	slice->messageIn( userHash );
	EXPECT_EQ( slice->status(), AAAA::AuthenticatorSlice::MESSAGE_AVAILABLE );

	std::string challenge = slice->messageOut();
	std::cout << "Challenge: " << challenge << std::endl;
	EXPECT_EQ( slice->status(), AAAA::AuthenticatorSlice::AWAITING_MESSAGE );

	AAAA::CRAMresponse response( challenge, "Good Password" );
	std::cout << "Response:  " << response.toString() << std::endl;
	slice->messageIn( response.toString() );
	EXPECT_EQ( slice->status(), AAAA::AuthenticatorSlice::AUTHENTICATED );

	user = slice->user();
	ASSERT_TRUE( user != NULL );

	EXPECT_EQ( user->uname(), "Admin" );
	EXPECT_EQ( user->name(), "Just a test user" );

	if ( user )
		delete user;

	user = slice->user();
	ASSERT_TRUE( user == NULL );

	if ( slice )
		delete slice;
}

TEST_F( AuthenticationFixture, AuthenticationWrongPassword )
{
	User* user = NULL;
	TextFileAuthUnit authUnit( "test", "passwd" );
	AAAA::AuthenticatorSlice* slice = authUnit.slice( "WOLFRAME-CRAM", net::RemoteTCPendpoint( "localhost", 2222 ));

	ASSERT_TRUE( slice != NULL );

	EXPECT_EQ( slice->status(), AAAA::AuthenticatorSlice::AWAITING_MESSAGE );

	std::string userHash = usernameHash( "Admin" );
	slice->messageIn( userHash );
	EXPECT_EQ( slice->status(), AAAA::AuthenticatorSlice::MESSAGE_AVAILABLE );

	std::string challenge = slice->messageOut();
	EXPECT_EQ( slice->status(), AAAA::AuthenticatorSlice::AWAITING_MESSAGE );

	AAAA::CRAMresponse response( challenge, "Good Password " );

	slice->messageIn( response.toString() );
	EXPECT_EQ( slice->status(), AAAA::AuthenticatorSlice::INVALID_CREDENTIALS );

	user = slice->user();
	ASSERT_TRUE( user == NULL );

	if ( slice )
		delete slice;
}

TEST_F( AuthenticationFixture, AuthenticationWrongUser )
{
	User* user = NULL;
	TextFileAuthUnit authUnit( "test", "passwd" );
	AAAA::AuthenticatorSlice* slice = authUnit.slice( "WOLFRAME-CRAM", net::RemoteTCPendpoint( "localhost", 2222 ));

	ASSERT_TRUE( slice != NULL );

	EXPECT_EQ( slice->status(), AAAA::AuthenticatorSlice::AWAITING_MESSAGE );

	std::string userHash = usernameHash( "admin" );

	slice->messageIn( userHash );
	EXPECT_EQ( slice->status(), AAAA::AuthenticatorSlice::USER_NOT_FOUND );

	user = slice->user();
	ASSERT_TRUE( user == NULL );

	if ( slice )
		delete slice;
}

TEST_F( AuthenticationFixture, AuthenticationLastSlice )
{
	User* user = NULL;
	TextFileAuthUnit authUnit( "test", "passwd" );
	AAAA::AuthenticatorSlice* slice = authUnit.slice( "WOLFRAME-CRAM", net::RemoteTCPendpoint( "localhost", 2222 ));
	slice->lastSlice();

	ASSERT_TRUE( slice != NULL );

	EXPECT_EQ( slice->status(), AAAA::AuthenticatorSlice::AWAITING_MESSAGE );

	std::string userHash = usernameHash( "bzgg12" );
	std::cout << "User hash: " << userHash << std::endl;
	slice->messageIn( userHash );
	EXPECT_EQ( slice->status(), AAAA::AuthenticatorSlice::MESSAGE_AVAILABLE );

	std::string challenge = slice->messageOut();
	std::cout << "Challenge: " << challenge << std::endl;
	EXPECT_EQ( slice->status(), AAAA::AuthenticatorSlice::AWAITING_MESSAGE );

	AAAA::CRAMresponse response( challenge, "Extremely Good Password " );
	std::cout << "Response:  " << response.toString() << std::endl;
	slice->messageIn( response.toString() );
	EXPECT_EQ( slice->status(), AAAA::AuthenticatorSlice::INVALID_CREDENTIALS );

	user = slice->user();
	ASSERT_TRUE( user == NULL );

	if ( slice )
		delete slice;
}

//****************************************************************************

int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS( );
}
