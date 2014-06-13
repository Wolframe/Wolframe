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
// low-level SASL authenticator tests
//

#include "logger-v1.hpp"
#include "gtest/gtest.h"

#include "SASLAuth.hpp"

using namespace _Wolframe::AAAA;
using namespace _Wolframe::log;
using namespace _Wolframe;
using namespace std;

class AuthenticationFixture : public ::testing::Test
{
protected:
	LogBackend& logBack;

	AuthenticationFixture( ) :
		logBack( LogBackend::instance( ) )
	{
		logBack.setConsoleLevel( LogLevel::LOGLEVEL_DATA );
//		logBack.setConsoleLevel( LogLevel::LOGLEVEL_INFO );
	}
};


TEST_F( AuthenticationFixture, typeName )
{
	SASLAuthUnit authenticator( "", "wolframe" );
	EXPECT_STREQ( authenticator.className( ), "SASLAuth" );
}

TEST_F( AuthenticationFixture, validUserAndPassword )
{
	User*	user;
	SASLAuthUnit authenticator( "test", "wolframe" );

	user = authenticator.authenticatePlain( "wolfusr", "wolfpwd" );
	ASSERT_TRUE( user != NULL );
	EXPECT_STREQ( "wolfusr", user->uname().c_str() );
	delete user;
}

TEST_F( AuthenticationFixture, validUserAndWrongPassword )
{
	User*	user;
	SASLAuthUnit authenticator( "test", "wolframe" );

	user = authenticator.authenticatePlain( "wolfusr", "xx" );
	ASSERT_TRUE( user == NULL );
}

TEST_F( AuthenticationFixture, unknownUser )
{
	User*	user;
	SASLAuthUnit authenticator( "test", "wolframe" );

	user = authenticator.authenticatePlain( "wolfusr2", "xx" );
	ASSERT_TRUE( user == NULL );
}

TEST_F( AuthenticationFixture, AuthenticationSliceUserPassword )
{
	User* user = NULL;
	SASLAuthUnit authUnit( "test", "wolframe" );
	AAAA::AuthenticatorSlice* slice = authUnit.slice( "WOLFRAME-SASL", net::RemoteTCPendpoint( "localhost", 2222 ));

	ASSERT_TRUE( slice != NULL );

	EXPECT_EQ( slice->status(), AAAA::AuthenticatorSlice::AWAITING_MESSAGE );

	slice->messageIn( "wolfusr" );

	EXPECT_EQ( slice->status(), AAAA::AuthenticatorSlice::MESSAGE_AVAILABLE );
	std::string passwordRequired = slice->messageOut();
	EXPECT_EQ( passwordRequired, "password?" );
	
	EXPECT_EQ( slice->status(), AAAA::AuthenticatorSlice::AWAITING_MESSAGE );
	slice->messageIn( "wolfpwd" );

	EXPECT_EQ( slice->status(), AAAA::AuthenticatorSlice::AUTHENTICATED );
	
	user = slice->user();
	ASSERT_TRUE( user != NULL );

	EXPECT_EQ( user->uname(), "wolfusr" );

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
	SASLAuthUnit authUnit( "test", "wolframe" );
	AAAA::AuthenticatorSlice* slice = authUnit.slice( "WOLFRAME-SASL", net::RemoteTCPendpoint( "localhost", 2222 ));

	ASSERT_TRUE( slice != NULL );
	ASSERT_TRUE( slice != NULL );

	EXPECT_EQ( slice->status(), AAAA::AuthenticatorSlice::AWAITING_MESSAGE );

	slice->messageIn( "wolfusr" );

	EXPECT_EQ( slice->status(), AAAA::AuthenticatorSlice::MESSAGE_AVAILABLE );
	std::string passwordRequired = slice->messageOut();
	EXPECT_EQ( passwordRequired, "password?" );
	
	EXPECT_EQ( slice->status(), AAAA::AuthenticatorSlice::AWAITING_MESSAGE );
	slice->messageIn( "xx" );

	EXPECT_EQ( slice->status(), AAAA::AuthenticatorSlice::INVALID_CREDENTIALS );
	
	user = slice->user();
	ASSERT_TRUE( user == NULL );

	if ( slice )
		delete slice;
}

TEST_F( AuthenticationFixture, AuthenticationWrongUser )
{
	User* user = NULL;
	SASLAuthUnit authUnit( "test", "wolframe" );
	AAAA::AuthenticatorSlice* slice = authUnit.slice( "WOLFRAME-SASL", net::RemoteTCPendpoint( "localhost", 2222 ));

	ASSERT_TRUE( slice != NULL );
	ASSERT_TRUE( slice != NULL );

	EXPECT_EQ( slice->status(), AAAA::AuthenticatorSlice::AWAITING_MESSAGE );

	slice->messageIn( "wolfusr2" );

	EXPECT_EQ( slice->status(), AAAA::AuthenticatorSlice::MESSAGE_AVAILABLE );
	std::string passwordRequired = slice->messageOut();
	EXPECT_EQ( passwordRequired, "password?" );
	
	EXPECT_EQ( slice->status(), AAAA::AuthenticatorSlice::AWAITING_MESSAGE );
	slice->messageIn( "xx" );

	EXPECT_EQ( slice->status(), AAAA::AuthenticatorSlice::USER_NOT_FOUND );
	
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
