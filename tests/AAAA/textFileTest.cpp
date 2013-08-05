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
// text file authenticator tests
//

#include "logger/logger-v1.hpp"
#include "gtest/gtest.h"

#include "TextFileAuth.hpp"
#include "types/sha2.h"
#include "types/byte2hex.h"
#include "system/globalRngGen.hpp"
#include <boost/algorithm/string.hpp>

using namespace _Wolframe::AAAA;
using namespace _Wolframe::log;
using namespace _Wolframe;
using namespace std;

// make the logger in the modules work
_Wolframe::log::LogBackend*	logBackendPtr;

// The fixture for testing class _Wolframe::module
class AuthenticationFixture : public ::testing::Test
{
protected:
	LogBackend& logBack;

	AuthenticationFixture( ) :
		logBack( LogBackend::instance( ) )
	{
		// Initialize the global random number generator
		_Wolframe::RandomGenerator::instance( "" );

		// set temporary logger pointer to the logger instantiated
		// in the test fixture
		logBackendPtr = &logBack;
//		logBack.setConsoleLevel( LogLevel::LOGLEVEL_DATA );
		logBack.setConsoleLevel( LogLevel::LOGLEVEL_INFO );
	}
};


//static User* CRAMauth( TextFileAuthenticator& auth, const std::string& /*user*/,
//		       const std::string& passwd, bool caseSensitive )
//{

//	CRAMchallenge	challenge;

//	unsigned char digest[ SHA224_DIGEST_SIZE ];
//	sha224((const unsigned char *)passwd.c_str(), passwd.length(), digest );

//	CRAMresponse	response( challenge, digest, SHA224_DIGEST_SIZE );

//	return auth.authenticate( challenge, response, caseSensitive );
//}


TEST_F( AuthenticationFixture, typeName )
{
	TextFileAuthenticator authenticator( "", "passwd" );
	EXPECT_STREQ( authenticator.className( ), "TextFileAuth" );
}

TEST_F( AuthenticationFixture, fileWithoutNewLine )
{
	User*	user;
	TextFileAuthenticator authenticator( "", "passwd-noNL" );

	user = authenticator.authenticatePlain( "admin", "Good Password", true );
	ASSERT_TRUE( user != NULL );
	EXPECT_STREQ( "admin", user->uname().c_str() );
	EXPECT_STREQ( "Wolframe Administrator", user->name().c_str() );
	delete user;

	user = authenticator.authenticatePlain( "goodusr", "User PassWord", true );
	ASSERT_TRUE( user != NULL );
	EXPECT_STREQ( "goodusr", user->uname().c_str() );
	EXPECT_STREQ( "Good User", user->name().c_str() );
	delete user;

	user = authenticator.authenticatePlain( "badusr", "User BadWord", true );
	ASSERT_TRUE( user != NULL );
	EXPECT_STREQ( "badusr", user->uname().c_str() );
	EXPECT_STREQ( "Bad User", user->name().c_str() );
	delete user;
}

TEST_F( AuthenticationFixture, validUsers )
{
	User*	user;
	TextFileAuthenticator authenticator( "", "passwd" );

	user = authenticator.authenticatePlain( "admin", "Good Password", true );
	ASSERT_TRUE( user != NULL );
	EXPECT_STREQ( "admin", user->uname().c_str() );
	EXPECT_STREQ( "Wolframe Administrator", user->name().c_str() );
	delete user;

	user = authenticator.authenticatePlain( "goodusr", "User PassWord", true );
	ASSERT_TRUE( user != NULL );
	EXPECT_STREQ( "goodusr", user->uname().c_str() );
	delete user;

	user = authenticator.authenticatePlain( "badusr", "User BadWord", true );
	ASSERT_TRUE( user != NULL );
	EXPECT_STREQ( "badusr", user->uname().c_str() );
	EXPECT_STREQ( "Bad User", user->name().c_str() );
	delete user;
}


TEST_F( AuthenticationFixture, caseInsensitive_Pass )
{
	User*	user;
	TextFileAuthenticator authenticator( "", "passwd" );

	user = authenticator.authenticatePlain( "AdMiN", "Good Password", false );
	ASSERT_TRUE( user != NULL );
	EXPECT_STREQ( "admin", user->uname().c_str() );
	EXPECT_STREQ( "Wolframe Administrator", user->name().c_str() );
	delete user;

	user = authenticator.authenticatePlain( "GoodUsr", "User PassWord", false );
	ASSERT_TRUE( user != NULL );
	EXPECT_STREQ( "goodusr", user->uname().c_str() );
	delete user;

	user = authenticator.authenticatePlain( "BadUsr", "User BadWord", false );
	ASSERT_TRUE( user != NULL );
	EXPECT_STREQ( "badusr", user->uname().c_str() );
	EXPECT_STREQ( "Bad User", user->name().c_str() );
	delete user;
}

TEST_F( AuthenticationFixture, caseInsensitive_Fail )
{
	User*	user;
	TextFileAuthenticator authenticator( "", "passwd" );

	user = authenticator.authenticatePlain( "AdMiN", "Good Password", true );
	EXPECT_EQ( NULL, user );
	user = authenticator.authenticatePlain( "GoodUsr", "User PassWord", true );
	EXPECT_EQ( NULL, user );
	user = authenticator.authenticatePlain( "BadUsr", "User BadWord", true );
	EXPECT_EQ( NULL, user );
}

TEST_F( AuthenticationFixture, wrongPasswords )
{
	User*	user;
	TextFileAuthenticator authenticator( "", "passwd" );

	user = authenticator.authenticatePlain( "admin", "Goood Password", true );
	EXPECT_EQ( NULL, user );
	user = authenticator.authenticatePlain( "goodusr", "User Password", true );
	EXPECT_EQ( NULL, user );
	user = authenticator.authenticatePlain( "badusr", "user BadWord", true );
	EXPECT_EQ( NULL, user );
}

TEST_F( AuthenticationFixture, nonExistentUsers )
{
	User*	user;
	TextFileAuthenticator authenticator( "", "passwd" );

	user = authenticator.authenticatePlain( "adminn", "xx", true );
	EXPECT_EQ( NULL, user );
	user = authenticator.authenticatePlain( "gooduser", "xx", true );
	EXPECT_EQ( NULL, user );
	user = authenticator.authenticatePlain( "baduser", "xx", true );
	EXPECT_EQ( NULL, user );
}

TEST_F( AuthenticationFixture, invalidPasswordHashes )
{
	User*	user;
	TextFileAuthenticator authenticator( "", "passwd" );


	user = authenticator.authenticatePlain( "badmin", "Good Password", true );
	EXPECT_EQ( NULL, user );
	user = authenticator.authenticatePlain( "wrongusr", "User PassWord", true );
	EXPECT_EQ( NULL, user );
	user = authenticator.authenticatePlain( "shortusr", "User BadWord", true );
	EXPECT_EQ( NULL, user );
}

TEST_F( AuthenticationFixture, nonexistentFile )
{
	User*	user;
	TextFileAuthenticator authenticator( "", "passwds" );

	user = authenticator.authenticatePlain( "admin", "xx", true );
	EXPECT_EQ( NULL, user );
	user = authenticator.authenticatePlain( "goodusr", "xx", true );
	EXPECT_EQ( NULL, user );
	user = authenticator.authenticatePlain( "badusr", "xx", true );
	EXPECT_EQ( NULL, user );
}

//******  CRAM  **************************************************************

//TEST_F( AuthenticationFixture, CRAM_fileWithoutNewLine )
//{
//	User*	user;
//	TextFileAuthenticator authenticator( "", "passwd-noNL" );

//	user = CRAMauth( authenticator, "admin", "Good Password", true );
//	ASSERT_TRUE( user != NULL );
//	EXPECT_STREQ( "admin", user->uname().c_str() );
//	EXPECT_STREQ( "Wolframe Administrator", user->name().c_str() );
//	delete user;

//	user = CRAMauth( authenticator, "goodusr", "User PassWord", true );
//	ASSERT_TRUE( user != NULL );
//	EXPECT_STREQ( "goodusr", user->uname().c_str() );
//	EXPECT_STREQ( "Good User", user->name().c_str() );
//	delete user;

//	user = CRAMauth( authenticator, "badusr", "User BadWord", true );
//	ASSERT_TRUE( user != NULL );
//	EXPECT_STREQ( "badusr", user->uname().c_str() );
//	EXPECT_STREQ( "Bad User", user->name().c_str() );
//	delete user;
//}

//TEST_F( AuthenticationFixture, CRAM_validUsers )
//{
//	User*	user;
//	TextFileAuthenticator authenticator( "", "passwd" );

//	user = CRAMauth( authenticator, "admin", "Good Password", true );
//	ASSERT_TRUE( user != NULL );
//	EXPECT_STREQ( "admin", user->uname().c_str() );
//	EXPECT_STREQ( "Wolframe Administrator", user->name().c_str() );
//	delete user;

//	user = CRAMauth( authenticator, "goodusr", "User PassWord", true );
//	ASSERT_TRUE( user != NULL );
//	EXPECT_STREQ( "goodusr", user->uname().c_str() );
//	delete user;

//	user = CRAMauth( authenticator, "badusr", "User BadWord", true );
//	ASSERT_TRUE( user != NULL );
//	EXPECT_STREQ( "badusr", user->uname().c_str() );
//	EXPECT_STREQ( "Bad User", user->name().c_str() );
//	delete user;
//}


//TEST_F( AuthenticationFixture, CRAM_caseInsensitive_Pass )
//{
//	User*	user;
//	TextFileAuthenticator authenticator( "", "passwd" );

//	user = CRAMauth( authenticator, "AdMiN", "Good Password", false );
//	ASSERT_TRUE( user != NULL );
//	EXPECT_STREQ( "admin", user->uname().c_str() );
//	EXPECT_STREQ( "Wolframe Administrator", user->name().c_str() );
//	delete user;

//	user = CRAMauth( authenticator, "GoodUsr", "User PassWord", false );
//	ASSERT_TRUE( user != NULL );
//	EXPECT_STREQ( "goodusr", user->uname().c_str() );
//	delete user;

//	user = CRAMauth( authenticator, "BadUsr", "User BadWord", false );
//	ASSERT_TRUE( user != NULL );
//	EXPECT_STREQ( "badusr", user->uname().c_str() );
//	EXPECT_STREQ( "Bad User", user->name().c_str() );
//	delete user;
//}

////TEST_F( AuthenticationFixture, CRAM_caseInsensitive_Fail )
////{
////	User*	user;
////	TextFileAuthenticator authenticator( "", "passwd" );

////	user = CRAMauth( authenticator, "AdMiN", "Good Password", true );
////	EXPECT_EQ( NULL, user );
////	user = CRAMauth( authenticator, "GoodUsr", "User PassWord", true );
////	EXPECT_EQ( NULL, user );
////	user = CRAMauth( authenticator, "BadUsr", "User BadWord", true );
////	EXPECT_EQ( NULL, user );
////}

//TEST_F( AuthenticationFixture, CRAM_wrongPasswords )
//{
//	User*	user;
//	TextFileAuthenticator authenticator( "", "passwd" );

//	user = CRAMauth( authenticator, "admin", "Goood Password", true );
//	EXPECT_EQ( NULL, user );
//	user = CRAMauth( authenticator, "goodusr", "User Password", true );
//	EXPECT_EQ( NULL, user );
//	user = CRAMauth( authenticator, "badusr", "user BadWord", true );
//	EXPECT_EQ( NULL, user );
//}

//TEST_F( AuthenticationFixture, CRAM_nonExistentUsers )
//{
//	User*	user;
//	TextFileAuthenticator authenticator( "", "passwd" );

//	user = CRAMauth( authenticator, "adminn", "xx", true );
//	EXPECT_EQ( NULL, user );
//	user = CRAMauth( authenticator, "gooduser", "xx", true );
//	EXPECT_EQ( NULL, user );
//	user = CRAMauth( authenticator, "baduser", "xx", true );
//	EXPECT_EQ( NULL, user );
//}

////TEST_F( AuthenticationFixture, CRAM_invalidPasswordHashes )
////{
////	User*	user;
////	TextFileAuthenticator authenticator( "", "passwd" );


////	user = CRAMauth( authenticator, "badmin", "Good Password", true );
////	EXPECT_EQ( NULL, user );
////	user = CRAMauth( authenticator, "wrongusr", "User PassWord", true );
////	EXPECT_EQ( NULL, user );
////	user = CRAMauth( authenticator, "shortusr", "User BadWord", true );
////	EXPECT_EQ( NULL, user );
////}

//TEST_F( AuthenticationFixture, CRAM_nonexistentFile )
//{
//	User*	user;
//	TextFileAuthenticator authenticator( "", "passwds" );

//	user = CRAMauth( authenticator, "admin", "xx", true );
//	EXPECT_EQ( NULL, user );
//	user = CRAMauth( authenticator, "goodusr", "xx", true );
//	EXPECT_EQ( NULL, user );
//	user = CRAMauth( authenticator, "badusr", "xx", true );
//	EXPECT_EQ( NULL, user );
//}

//****************************************************************************

int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS( );
}
