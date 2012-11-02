/************************************************************************

 Copyright (C) 2011, 2012 Project Wolframe.
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
// Version class unit tests ( using gTest)
//

#include <climits>
#include <cstdlib>
#include <cstdio>
#include <ctime>

#include "version.hpp"
#include "gtest/gtest.h"

#define	MAX_STRING	128

// The fixture for testing class Wolframe::Version
class VersionFixture : public ::testing::Test	{
protected:
	// You can remove any or all of the following functions if its body is empty.

	// Set-up work for each test here.
	VersionFixture()	{
		srand((unsigned)time(0));
		major = (unsigned short)( rand() % 100 );
		minor = (unsigned short)( rand() % 100 );
		revision = (unsigned short)( rand() % 100 );
		build = (unsigned)( rand() % 100 );

		ver0 = _Wolframe::Version( major, minor, revision, build );
		ver1 = _Wolframe::Version( major, minor, 0 );
		ver2 = _Wolframe::Version( major, minor );
		ver3 = _Wolframe::Version( major * 1000000lu + minor * 10000lu + revision * 100 + build );
		sprintf( verStr0, "%d.%d.%d.%u", major, minor, revision, build );
		sprintf( verStr1, "%d.%d.%d", major, minor, revision );
		sprintf( verStr2, "%d.%d", major, minor );
		sprintf( verStr3, "This is version %d . %d . %d . %u %% /", major, minor, revision, build );
	}

	// Objects declared here can be used by all tests in the test case.
	unsigned short		major, minor, revision;
	unsigned		build;
	_Wolframe::Version	ver0, ver1, ver2, ver3;
	char			verStr0[MAX_STRING];
	char			verStr1[MAX_STRING];
	char			verStr2[MAX_STRING];
	char			verStr3[MAX_STRING];
};


// Tests the Version constructors and members
TEST_F( VersionFixture, Members )	{
	ASSERT_EQ( ver0.Major(), major );
	ASSERT_EQ( ver0.Minor(), minor );
	ASSERT_EQ( ver0.Revision(), revision );
	ASSERT_EQ( ver0.Build(), build );

	ASSERT_EQ( ver3.Major(), major );
	ASSERT_EQ( ver3.Minor(), minor );
	ASSERT_EQ( ver3.Revision(), revision );
	ASSERT_EQ( ver3.Build(), build );

	ASSERT_TRUE( ver0 == ver3 );
}

// Tests the Version operators
TEST_F( VersionFixture, Operators )	{
	ASSERT_TRUE(  ver0 != ver1 );
	ASSERT_TRUE(  ver0 >  ver1 );
	ASSERT_TRUE(  ver0 >= ver1 );
	ASSERT_FALSE( ver0 == ver1 );
	ASSERT_FALSE( ver0 <  ver1 );
	ASSERT_FALSE( ver0 <= ver1 );

	ASSERT_TRUE(  ver1 == ver2 );
	ASSERT_TRUE(  ver1 <= ver2 );
	ASSERT_TRUE(  ver1 >= ver2 );
	ASSERT_FALSE( ver1 != ver2 );
	ASSERT_FALSE( ver1 <  ver2 );
	ASSERT_FALSE( ver1 >  ver2 );
}

// Tests the Version pattern printing
TEST_F( VersionFixture, Printing )	{
	ASSERT_STREQ( ver0.toString().c_str(), verStr0 );
	ASSERT_STREQ( ver2.toString().c_str(), verStr2 );
	ASSERT_STREQ( ver0.toString( "%M.%m.%r.%b" ).c_str(), verStr0 );
	ASSERT_STREQ( ver0.toString( "%M.%m.%r" ).c_str(), verStr1 );
	ASSERT_STREQ( ver0.toString( "%M.%m" ).c_str(), verStr2 );
	ASSERT_STREQ( ver0.toString( "This is version %M . %m . %r . %b %% %/" ).c_str(), verStr3 );
}


int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
