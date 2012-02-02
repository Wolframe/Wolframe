//
// testing the various authenticators
//

#include "logger-v1.hpp"
#include <gtest/gtest.h>

#ifdef WITH_PAM
#include "PAMAuth.hpp"
#endif // WITH_PATH

using namespace _Wolframe::AAAA;
using namespace _Wolframe::log;
using namespace _Wolframe;
using namespace std;

// Aba: tempoarary hack to make the logger in the PAMAuthenticator work
// (some mixup here with logging in a module of a class which would actualy
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

// Aba: avoiding module loader here on purpose, so we include the things to
// test with conditional compilation
#ifdef WITH_PAM
TEST_F( AuthenticationFixture, PamAuthenticatorIdentity )
{
	PAMAuthenticator authenticator( "wolframe" );
	ASSERT_STREQ( authenticator.typeName( ), "PAMAuth" );
}
#endif // WITH_PATH

int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS( );
}
