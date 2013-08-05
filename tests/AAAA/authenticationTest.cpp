//
// testing the various authenticators
//

#include "logger/logger-v1.hpp"
#include "gtest/gtest.h"

// Aba: avoiding module loader here on purpose, so we include the things to
// test with conditional compilation

#include "TextFileAuth.hpp"
#include "DBauth.hpp"
#ifdef WITH_PAM
#include "PAMAuth.hpp"
#endif // WITH_PATH
#ifdef WITH_SASL
#include "SaslAuth.hpp"
#endif // WITH_SASL

using namespace _Wolframe::AAAA;
using namespace _Wolframe::log;
using namespace _Wolframe;
using namespace std;

// Aba: temporary hack to make the logger in the PAMAuthenticator work
// (some mixup here with logging in a module of a class which would actually
// be standalone)
_Wolframe::log::LogBackend*	logBackendPtr;

// The fixture for testing class _Wolframe::module
class AuthenticationFixture : public ::testing::Test
{
	protected:
		LogBackend& logBack;

	protected:
		AuthenticationFixture( ) :
			logBack( LogBackend::instance( ) )
		{
			// set temporary logger pointer to the logger instantiated
			// in the test fixture
			logBackendPtr = &logBack;

			logBack.setConsoleLevel( LogLevel::LOGLEVEL_DATA );
		}
};

TEST_F( AuthenticationFixture, TextFileAuthenticator )
{
	TextFileAuthenticator authenticator( "", "passwd" );
	ASSERT_STREQ( authenticator.className( ), "TextFileAuth" );
}

TEST_F( AuthenticationFixture, DatabaseAuthenticator )
{
	DBauthenticator authenticator( "", "sqlitedb" );
	ASSERT_STREQ( authenticator.className( ), "DBAuth" );
}

#ifdef WITH_PAM
TEST_F( AuthenticationFixture, PamAuthenticator )
{
	PAMAuthenticator authenticator( "", "wolframe" );
	ASSERT_STREQ( authenticator.className( ), "PAMAuth" );
}
#endif // WITH_PATH

#ifdef WITH_SASL
TEST_F( AuthenticationFixture, SaslAuthenticator )
{
	SaslAuthenticator authenticator( "", "wolframe", "sasl.conf" );
	ASSERT_STREQ( authenticator.className( ), "SASLAuth" );
}
#endif // WITH_SASL

int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS( );
}
