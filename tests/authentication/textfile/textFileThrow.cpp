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
// text file authenticator throw tests
//

#include "logger-v1.hpp"
#include "gtest/gtest.h"

#include "TextFileAuth.hpp"
#include "types/base64.hpp"
#include "crypto/HMAC.hpp"
#include "system/globalRngGen.hpp"
#include "AAAA/passwordHash.hpp"
#include "AAAA/authSlice.hpp"
#include "AAAA/CRAM.hpp"

#include <boost/algorithm/string.hpp>

using namespace _Wolframe::AAAA;
using namespace _Wolframe::log;
using namespace _Wolframe;
using namespace std;

// The fixture for testing class _Wolframe::module
class AuthenticationFixture : public ::testing::Test
{
protected:
	LogBackend& logBack;

	AuthenticationFixture( ) :
		logBack( LogBackend::instance( ) )
	{
		// Initialize the global random number generator
		_Wolframe::GlobalRandomGenerator::instance( "" );

		logBack.setConsoleLevel( LogLevel::LOGLEVEL_INFO );
	}
};


TEST_F( AuthenticationFixture, typeName )
{
	TextFileAuthUnit authenticator( "", "passwd" );
	EXPECT_STREQ( authenticator.className( ), "TextFileAuth" );
}


static std::string usernameHash( const std::string& username )
{
	_Wolframe::GlobalRandomGenerator& rnd = _Wolframe::GlobalRandomGenerator::instance();
	unsigned char salt[ PASSWORD_SALT_SIZE ];

	rnd.generate( salt, PASSWORD_SALT_SIZE );

	crypto::HMAC_SHA256 hmac0( salt, PASSWORD_SALT_SIZE, username );

	char saltStr[ 2 * PASSWORD_SALT_SIZE ];
	base64::encode( salt, PASSWORD_SALT_SIZE, saltStr, 2 * PASSWORD_SALT_SIZE, 0 );

	return "$" + std::string( saltStr ) + "$" + hmac0.toString();
}

TEST_F( AuthenticationFixture, UnexpectedMessageOut )
{
	TextFileAuthUnit authUnit( "test", "passwd" );
	AAAA::AuthenticatorSlice* slice = authUnit.slice( "WOLFRAME-CRAM", net::RemoteTCPendpoint( "localhost", 2222 ));

	ASSERT_TRUE( slice != NULL );

	EXPECT_EQ( slice->status(), AAAA::AuthenticatorSlice::AWAITING_MESSAGE );

	EXPECT_THROW( std::string msg = slice->messageOut(),
		      std::logic_error );

	delete slice;
}

TEST_F( AuthenticationFixture, UnexpectedMessageIn )
{
	TextFileAuthUnit authUnit( "test", "passwd" );
	AAAA::AuthenticatorSlice* slice = authUnit.slice( "WOLFRAME-CRAM", net::RemoteTCPendpoint( "localhost", 2222 ));

	ASSERT_TRUE( slice != NULL );

	EXPECT_EQ( slice->status(), AAAA::AuthenticatorSlice::AWAITING_MESSAGE );

	std::string userHash = usernameHash( "Admin" );
	slice->messageIn( userHash );
	EXPECT_EQ( slice->status(), AAAA::AuthenticatorSlice::MESSAGE_AVAILABLE );

	EXPECT_THROW( slice->messageIn( "The wrong message at the wrong time" ),
		      std::logic_error );

	delete slice;
}

//****************************************************************************

int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS( );
}
