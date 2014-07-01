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
// text file password changer tests
//

#include "logger-v1.hpp"
#include "gtest/gtest.h"

#include "TextFileAuth.hpp"
#include "types/base64.hpp"
#include "types/byte2hex.h"
#include "crypto/AES256.h"
#include "AAAA/passwordHash.hpp"
#include "AAAA/passwordChanger.hpp"
#include "AAAA/pwdChangeMessage.hpp"
#include "AAAA/CRAM.hpp"
#include "system/globalRngGen.hpp"

#include <boost/algorithm/string.hpp>

using namespace _Wolframe::AAAA;
using namespace _Wolframe::log;
using namespace _Wolframe;
using namespace std;

// The fixture for testing
class PasswordChangerFixture : public ::testing::Test
{
protected:
	LogBackend& logBack;

	PasswordChangerFixture( ) :
		logBack( LogBackend::instance( ) )
	{
		// Initialize the global random number generator
		_Wolframe::GlobalRandomGenerator::instance( "" );

//		logBack.setConsoleLevel( LogLevel::LOGLEVEL_DATA );
		logBack.setConsoleLevel( LogLevel::LOGLEVEL_INFO );
	}
};


TEST_F( PasswordChangerFixture, PasswordMessage )
{
	PasswordChangeMessage	msg0( "Good Password" );

	unsigned char buf[ 64 ];
	hex2byte( "0D476F6F642050617373776F72640000000000000000000000000000000000000000000000000000000000000000000003A5B5940AF7517EE9EBF096671C7C4E", buf, 64 );
	PasswordChangeMessage	msg1( buf );

	EXPECT_STREQ( "Good Password", msg0.password().c_str() );
	EXPECT_STREQ( "Good Password", msg1.password().c_str() );
}

TEST_F( PasswordChangerFixture, PasswordMessageThrow )
{
	unsigned char buf[ 64 ];
	hex2byte( "0D476F6F642050617373776F72640000000000000000000000000000000000000000000010000000000000000000000003A5B5940AF7517EE9EBF096671C7C4E", buf, 64 );
	EXPECT_THROW( PasswordChangeMessage msg( buf ), std::runtime_error );

	std::string goodPwd = "Goooooooooooooooood looooooooooooooong password";
	std::string badPwd  = "Baaaaaaaaaaaaaaaaaad looooooooooooooong password";
	EXPECT_NO_THROW( PasswordChangeMessage msg( goodPwd ));
	EXPECT_EQ( goodPwd.length(), 47u );
	EXPECT_THROW( PasswordChangeMessage msg( badPwd ), std::runtime_error );
	EXPECT_GT( badPwd.length(), 47u );
}

TEST_F( PasswordChangerFixture, PasswordMessageEncryption )
{
	PasswordChangeMessage	msg0( "Good Password" );
	ASSERT_EQ( msg0.size(), 64u );

	unsigned char IV[ 16 ] =  { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
				    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };
	unsigned char key[ 32 ] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
				    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
				    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
				    0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f };

	unsigned char cryptMsg [ 64 ];
	memcpy( cryptMsg, msg0.data(), 64 );
	AES256_context	ctx;
	AES256_init( &ctx, key );
	AES256_encrypt_CBC( &ctx, IV, cryptMsg, 64 );
	AES256_done( &ctx );

	char buf[ 129 ];
	byte2hex( msg0.data(), 64, buf, 129 );
	std::cout << "Plain text message: " << buf << std::endl;
	byte2hex( cryptMsg, 64, buf, 129 );
	std::cout << "Encrypted message:  " << buf << std::endl;

	AES256_init( &ctx, key );
	AES256_decrypt_CBC( &ctx, IV, cryptMsg, 64 );
	AES256_done( &ctx );
	PasswordChangeMessage	msg1( cryptMsg );
	EXPECT_STREQ( "Good Password", msg1.password().c_str() );
	EXPECT_TRUE( 0 == std::memcmp( msg1.data(), msg0.data(), msg0.size() ));

	memcpy( cryptMsg, msg0.data(), 64 );
	AES256_init( &ctx, key );
	AES256_encrypt_CBC( &ctx, IV, cryptMsg, 64 );
	AES256_done( &ctx );
	cryptMsg [ 37 ] ^= 0x01;	// Just toggle a bit
	AES256_init( &ctx, key );
	AES256_decrypt_CBC( &ctx, IV, cryptMsg, 64 );
	AES256_done( &ctx );
	EXPECT_THROW( PasswordChangeMessage msg2( cryptMsg ), std::runtime_error );
}

TEST_F( PasswordChangerFixture, PasswordMessageBase64 )
{
	PasswordChangeMessage msg0( "Good Password" );
	ASSERT_EQ( msg0.size(), 64u );

	unsigned char IV[ 16 ] =  { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
				    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };
	unsigned char key[ 32 ] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
				    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
				    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
				    0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f };

	std::string msgStr = msg0.toBase64( IV, key );
	std::cout << "Encrypted message: " << msgStr << std::endl;

	PasswordChangeMessage msg1;
	EXPECT_TRUE( msg1.fromBase64( msgStr, IV, key ) );
	EXPECT_STREQ( "Good Password", msg1.password().c_str() );
	EXPECT_TRUE( 0 == std::memcmp( msg1.data(), msg0.data(), msg0.size() ));

	msgStr[ 37 ] ^= 0x01;	// Mess with the message
	EXPECT_FALSE( msg1.fromBase64( msgStr, IV, key ) );
}

TEST_F( PasswordChangerFixture, GetPasswordChanger )
{
	TextFileAuthUnit authUnit( "test", "passwd" );
	User user0( "test", "WOLFRAME-CRAM", "blabla", "Bla bla test user ");
	PasswordChanger* changer = authUnit.passwordChanger( user0, net::RemoteTCPendpoint( "localhost", 2222 ));
	ASSERT_TRUE( changer != NULL );
	if ( changer )
		delete changer;

	// Bad identifier
	User user1( "BadTest", "WOLFRAME-CRAM", "blabla", "Bla bla test user");
	changer = authUnit.passwordChanger( user1, net::RemoteTCPendpoint( "localhost", 2222 ));
	ASSERT_TRUE( changer == NULL );

	// Bad mech
	User user2( "test", "WOLFRAME-SOMETING", "blabla", "Bla bla test user");
	changer = authUnit.passwordChanger( user2, net::RemoteTCPendpoint( "localhost", 2222 ));
	ASSERT_TRUE( changer == NULL );
}


TEST_F( PasswordChangerFixture, PasswordChangeSuccess )
{
	TextFileAuthUnit authUnit( "test", "passwd" );
	User user( "test", "WOLFRAME-CRAM", "Admin", "");
	PasswordChanger* changer = authUnit.passwordChanger( user, net::RemoteTCPendpoint( "localhost", 2222 ));
	ASSERT_TRUE( changer != NULL );

	EXPECT_EQ( changer->status(), AAAA::PasswordChanger::MESSAGE_AVAILABLE );

	std::string challenge = changer->messageOut();
	std::cout << "Challenge: " << challenge << std::endl;
	EXPECT_EQ( changer->status(), AAAA::PasswordChanger::AWAITING_MESSAGE );

	AAAA::CRAMsalt salt( challenge );
	std::cout << "Salt (IV): " << salt.toString() << std::endl;
	AAAA::CRAMresponse response( challenge, "Good Password" );
	std::cout << "Response:  " << response.toString() << std::endl;

	PasswordChangeMessage pwd( "New Password" );
	std::string pwdMsg = pwd.toBase64( salt.salt(), response.response() );
	changer->messageIn( pwdMsg );
	EXPECT_EQ( changer->status(), AAAA::PasswordChanger::PASSWORD_EXCHANGED );

	EXPECT_EQ( changer->password(), "New Password" );

	if ( changer )
		delete changer;
}

TEST_F( PasswordChangerFixture, PasswordChangeFailure )
{
	TextFileAuthUnit authUnit( "test", "passwd" );
	User user( "test", "WOLFRAME-CRAM", "Admin", "");
	PasswordChanger* changer = authUnit.passwordChanger( user, net::RemoteTCPendpoint( "localhost", 2222 ));
	ASSERT_TRUE( changer != NULL );

	EXPECT_EQ( changer->status(), AAAA::PasswordChanger::MESSAGE_AVAILABLE );

	std::string challenge = changer->messageOut();
	std::cout << "Challenge: " << challenge << std::endl;
	EXPECT_EQ( changer->status(), AAAA::PasswordChanger::AWAITING_MESSAGE );

	AAAA::CRAMsalt salt( challenge );
	std::cout << "Salt (IV): " << salt.toString() << std::endl;
	AAAA::CRAMresponse response( challenge, "Good Password" );
	std::cout << "Response:  " << response.toString() << std::endl;

	PasswordChangeMessage pwd( "New Password" );
	std::string pwdMsg = pwd.toBase64( salt.salt(), response.response() );
	pwdMsg[ 59 ] ^= 0x01;
	changer->messageIn( pwdMsg );
	EXPECT_EQ( changer->status(), AAAA::PasswordChanger::INVALID_MESSAGE );

	EXPECT_THROW( changer->password(), std::logic_error );

	if ( changer )
		delete changer;
}

//****************************************************************************

int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS( );
}
