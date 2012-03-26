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
		logBack.setConsoleLevel( LogLevel::LOGLEVEL_DATA );
	}
};

TEST_F( AuthenticationFixture, PlainTextAuthenticator )
{
	PlainTextAuthenticator authenticator( "plainPasswd" );
	ASSERT_STREQ( authenticator.typeName( ), "PlainTextAuth" );
}

int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS( );
}
