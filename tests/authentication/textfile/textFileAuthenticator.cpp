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

#include "module/moduleDirectory.hpp"
#include "AAAA/AAAAprovider.hpp"


#include "TextFileAuth.hpp"
#include "types/base64.hpp"
#include "system/globalRngGen.hpp"
#include "crypto/HMAC.hpp"
#include "AAAA/passwordHash.hpp"
#include "AAAA/CRAM.hpp"
#include <boost/algorithm/string.hpp>
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

static std::string g_execdir;

using namespace _Wolframe::AAAA;
using namespace _Wolframe::log;
using namespace _Wolframe::module;

using namespace _Wolframe;
using namespace std;

// The fixture for testing class _Wolframe::module
class AuthenticatorFixture : public ::testing::Test
{
protected:
	LogBackend&		logBack;

	AuthenticatorFixture( ) :
		logBack( LogBackend::instance( ) )
	{
		// Initialize the global random number generator
		_Wolframe::GlobalRandomGenerator::instance( "" );
		// Set the log level
		logBack.setConsoleLevel( LogLevel::LOGLEVEL_INFO );

		// Build the modules directory
		ModulesDirectory modDir( g_execdir);
		static module::ConfiguredBuilderDescription< AAAA::TextFileAuthConstructor,
				AAAA::TextFileAuthConfig > builder( "Authentication file", "Authentication",
								    "TextFile", "TextFileAuth" );
		modDir.addBuilder( &builder );
		AAAAconfiguration config;
		AAAAprovider provider( &config, &modDir );
	}
};


//TEST_F( AuthenticatorFixture, AuthenticationSuccess )
//{
//	User* user = NULL;

//	AAAA::Authenticator* authenticator = authUnit.slice( "WOLFRAME-CRAM", net::RemoteTCPendpoint( "localhost", 2222 ));
//	slice->lastSlice();

//	ASSERT_TRUE( authenticator != NULL );

//	EXPECT_EQ( slice->status(), AAAA::AuthenticatorSlice::AWAITING_MESSAGE );

//	std::string userHash = usernameHash( "bzgg12" );
//	std::cout << "User hash: " << userHash << std::endl;
//	slice->messageIn( userHash );
//	EXPECT_EQ( slice->status(), AAAA::AuthenticatorSlice::MESSAGE_AVAILABLE );

//	std::string challenge = slice->messageOut();
//	std::cout << "Challenge: " << challenge << std::endl;
//	EXPECT_EQ( slice->status(), AAAA::AuthenticatorSlice::AWAITING_MESSAGE );

//	AAAA::CRAMresponse response( challenge, "Extremely Good Password " );
//	std::cout << "Response:  " << response.toString() << std::endl;
//	slice->messageIn( response.toString() );
//	EXPECT_EQ( slice->status(), AAAA::AuthenticatorSlice::INVALID_CREDENTIALS );

//	user = slice->user();
//	ASSERT_TRUE( user == NULL );
//}

//****************************************************************************

int main( int argc, char **argv )
{
	g_execdir = boost::filesystem::system_complete( argv[0]).parent_path().string();
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS( );
}
