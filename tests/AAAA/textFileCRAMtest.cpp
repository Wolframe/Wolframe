/************************************************************************

 Copyright (C) 2011 Project Wolframe.
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
		// set temporary logger pointer to the logger instantiated
		// in the test fixture
		logBackendPtr = &logBack;
//		logBack.setConsoleLevel( LogLevel::LOGLEVEL_DATA );
		logBack.setConsoleLevel( LogLevel::LOGLEVEL_INFO );
	}
};

TEST_F( AuthenticationFixture, typeName )
{
	TextFileAuthenticator authenticator( "", "passwd" );
	ASSERT_STREQ( authenticator.typeName( ), "TextFileAuth" );
}

TEST_F( AuthenticationFixture, validUsers )
{
	User*	user;
	TextFileAuthenticator authenticator( "", "passwd" );

	user = authenticator.authenticate( "admin", "Good Password", true );
	ASSERT_TRUE( user != NULL );
	ASSERT_STREQ( "admin", user->uname().c_str() );
	ASSERT_STREQ( "Wolframe Administrator", user->name().c_str() );
	delete user;

	user = authenticator.authenticate( "goodusr", "User PassWord", true );
	ASSERT_TRUE( user != NULL );
	ASSERT_STREQ( "goodusr", user->uname().c_str() );
	ASSERT_STREQ( "Good User", user->name().c_str() );
	delete user;

	user = authenticator.authenticate( "badusr", "User BadWord", true );
	ASSERT_TRUE( user != NULL );
	ASSERT_STREQ( "badusr", user->uname().c_str() );
	ASSERT_STREQ( "Bad User", user->name().c_str() );
	delete user;
}

TEST_F( AuthenticationFixture, caseInsensitive_Pass )
{
	User*	user;
	TextFileAuthenticator authenticator( "", "passwd" );

	user = authenticator.authenticate( "AdMiN", "Good Password", false );
	ASSERT_TRUE( user != NULL );
	ASSERT_STREQ( "admin", user->uname().c_str() );
	ASSERT_STREQ( "Wolframe Administrator", user->name().c_str() );
	delete user;

	user = authenticator.authenticate( "GoodUsr", "User PassWord", false );
	ASSERT_TRUE( user != NULL );
	ASSERT_STREQ( "goodusr", user->uname().c_str() );
	ASSERT_STREQ( "Good User", user->name().c_str() );
	delete user;

	user = authenticator.authenticate( "BadUsr", "User BadWord", false );
	ASSERT_TRUE( user != NULL );
	ASSERT_STREQ( "badusr", user->uname().c_str() );
	ASSERT_STREQ( "Bad User", user->name().c_str() );
	delete user;
}

TEST_F( AuthenticationFixture, caseInsensitive_Fail )
{
	User*	user;
	TextFileAuthenticator authenticator( "", "passwd" );

	user = authenticator.authenticate( "AdMiN", "Good Password", true );
	ASSERT_EQ( NULL, user );
	user = authenticator.authenticate( "GoodUsr", "User PassWord", true );
	ASSERT_EQ( NULL, user );
	user = authenticator.authenticate( "BadUsr", "User BadWord", true );
	ASSERT_EQ( NULL, user );
}

TEST_F( AuthenticationFixture, invalidPasswords )
{
	User*	user;
	TextFileAuthenticator authenticator( "", "passwd" );

	user = authenticator.authenticate( "admin", "Goood Password", true );
	ASSERT_EQ( NULL, user );
	user = authenticator.authenticate( "goodusr", "User Password", true );
	ASSERT_EQ( NULL, user );
	user = authenticator.authenticate( "badusr", "user BadWord", true );
	ASSERT_EQ( NULL, user );
}

TEST_F( AuthenticationFixture, invalidUsers )
{
	User*	user;
	TextFileAuthenticator authenticator( "", "passwd" );

	user = authenticator.authenticate( "adminn", "xx", true );
	ASSERT_EQ( NULL, user );
	user = authenticator.authenticate( "gooduser", "xx", true );
	ASSERT_EQ( NULL, user );
	user = authenticator.authenticate( "baduser", "xx", true );
	ASSERT_EQ( NULL, user );
}

TEST_F( AuthenticationFixture, invalidFile )
{
	User*	user;
	TextFileAuthenticator authenticator( "", "passwds" );

	user = authenticator.authenticate( "admin", "xx", true );
	ASSERT_EQ( NULL, user );
	user = authenticator.authenticate( "goodusr", "xx", true );
	ASSERT_EQ( NULL, user );
	user = authenticator.authenticate( "badusr", "xx", true );
	ASSERT_EQ( NULL, user );
}


int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS( );
}
