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
//
//

#include "logger-v1.hpp"
#include <gtest/gtest.h>

#include "PlainTextAuth.hpp"

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
	PlainTextAuthenticator authenticator( "plainPasswd" );
	ASSERT_STREQ( authenticator.typeName( ), "PlainTextAuth" );
}

TEST_F( AuthenticationFixture, validUsers )
{
	User*	user;
	PlainTextAuthenticator authenticator( "plainPasswd" );

	user = authenticator.authenticate( "admin", "Good Password" );
	ASSERT_TRUE( user != NULL );
	ASSERT_STREQ( "admin", user->uname().c_str() );
	ASSERT_EQ( 0, user->uid() );
	ASSERT_EQ( 0, user->gid() );
	ASSERT_STREQ( "Wolframe Administrator", user->name().c_str() );
	delete user;

	user = authenticator.authenticate( "goodusr", "User PassWord" );
	ASSERT_TRUE( user != NULL );
	ASSERT_STREQ( "goodusr", user->uname().c_str() );
	ASSERT_EQ( 1000, user->uid() );
	ASSERT_EQ( 100, user->gid() );
	ASSERT_STREQ( "Good User", user->name().c_str() );
	delete user;

	user = authenticator.authenticate( "badusr", "User BadWord" );
	ASSERT_TRUE( user != NULL );
	ASSERT_STREQ( "badusr", user->uname().c_str() );
	ASSERT_EQ( 1001, user->uid() );
	ASSERT_EQ( 100, user->gid() );
	ASSERT_STREQ( "Bad User", user->name().c_str() );
	delete user;
}

TEST_F( AuthenticationFixture, invalidPasswords )
{
	User*	user;
	PlainTextAuthenticator authenticator( "plainPasswd" );

	user = authenticator.authenticate( "admin", "Goood Password" );
	ASSERT_EQ( NULL, user );
	user = authenticator.authenticate( "goodusr", "User Password" );
	ASSERT_EQ( NULL, user );
	user = authenticator.authenticate( "badusr", "user BadWord" );
	ASSERT_EQ( NULL, user );
}

TEST_F( AuthenticationFixture, invalidUsers )
{
	User*	user;
	PlainTextAuthenticator authenticator( "plainPasswd" );

	user = authenticator.authenticate( "adminn", "xx" );
	ASSERT_EQ( NULL, user );
	user = authenticator.authenticate( "gooduser", "xx" );
	ASSERT_EQ( NULL, user );
	user = authenticator.authenticate( "baduser", "xx" );
	ASSERT_EQ( NULL, user );
}


int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS( );
}
