/************************************************************************

 Copyright (C) 2011 - 2013 Project Wolframe.
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

#include "system/version.hpp"
#include "gtest/gtest.h"

using namespace _Wolframe;

#define	MAX_STRING	128

// The fixture for testing class Wolframe::Version
class VersionFixture : public ::testing::Test	{
protected:
	// You can remove any or all of the following functions if its body is empty.

	// Set-up work for each test here.
	VersionFixture()	{
		srand((unsigned)time(0));
		Major_ = (unsigned short)( rand() % 100 );
		Minor_ = (unsigned short)( rand() % 100 );
		Revision_ = (unsigned short)( rand() % 100 );
		Build_ = (unsigned)( rand() % 100 );

		ver0 = Version( Major_, Minor_, Revision_, Build_ );
		ver1 = Version( Major_, Minor_, 0 );
		ver2 = Version( Major_, Minor_ );
		ver3 = Version( Major_ * 10000000lu + Minor_ * 100000lu + Revision_ * 1000 + Build_ );
		sprintf( verStr0, "%d.%d.%d.%u", Major_, Minor_, Revision_, Build_ );
		sprintf( verStr1, "%d.%d.%d", Major_, Minor_, Revision_ );
		sprintf( verStr2, "%d.%d", Major_, Minor_ );
		sprintf( verStr3, "This is version %d . %d . %d . %u %% /", Major_, Minor_, Revision_, Build_ );

		char pverStr1[ 128];
		sprintf( pverStr1, "%d.%d.%d", Major_, Minor_, 0);

		pver0 = Version( verStr0);
		pver1 = Version( pverStr1);
		pver2 = Version( verStr2);
		pver3 = Version( verStr3, "This is version %M . %m . %r . %b %% /");
	}

	// Objects declared here can be used by all tests in the test case.
	unsigned short		Major_, Minor_, Revision_;
	unsigned		Build_;
	Version	ver0, ver1, ver2, ver3;
	Version	pver0, pver1, pver2, pver3;
	char			verStr0[MAX_STRING];
	char			verStr1[MAX_STRING];
	char			verStr2[MAX_STRING];
	char			verStr3[MAX_STRING];
};


// Tests the Version constructors and members
TEST_F( VersionFixture, Members )	{
	EXPECT_EQ( ver0.Major(), Major_ );
	EXPECT_EQ( ver0.Minor(), Minor_ );
	EXPECT_EQ( ver0.Revision(), Revision_ );
	EXPECT_EQ( ver0.Build(), Build_ );

	EXPECT_EQ( ver3.Major(), Major_ );
	EXPECT_EQ( ver3.Minor(), Minor_ );
	EXPECT_EQ( ver3.Revision(), Revision_ );
	EXPECT_EQ( ver3.Build(), Build_ );

	EXPECT_TRUE( ver0 == ver3 );
}

// Tests the Version operators
TEST_F( VersionFixture, Operators )	{
	EXPECT_TRUE(  ver0 != ver1 );
	EXPECT_TRUE(  ver0 >  ver1 );
	EXPECT_TRUE(  ver0 >= ver1 );
	EXPECT_FALSE( ver0 == ver1 );
	EXPECT_FALSE( ver0 <  ver1 );
	EXPECT_FALSE( ver0 <= ver1 );

	EXPECT_TRUE(  ver1 == ver2 );
	EXPECT_TRUE(  ver1 <= ver2 );
	EXPECT_TRUE(  ver1 >= ver2 );
	EXPECT_FALSE( ver1 != ver2 );
	EXPECT_FALSE( ver1 <  ver2 );
	EXPECT_FALSE( ver1 >  ver2 );
}

// Tests the Version pattern printing
TEST_F( VersionFixture, Printing )	{
	EXPECT_STREQ( ver0.toString().c_str(), verStr0 );
	EXPECT_STREQ( ver2.toString().c_str(), verStr2 );
	EXPECT_STREQ( ver0.toString( "%M.%m.%r.%b" ).c_str(), verStr0 );
	EXPECT_STREQ( ver0.toString( "%M.%m.%r" ).c_str(), verStr1 );
	EXPECT_STREQ( ver0.toString( "%M.%m" ).c_str(), verStr2 );
	EXPECT_STREQ( ver0.toString( "This is version %M . %m . %r . %b %% %/" ).c_str(), verStr3 );
}

// Tests the Version parsing
TEST_F( VersionFixture, Parsing )	{
	EXPECT_TRUE( pver0 == ver0 );
	EXPECT_TRUE( pver1 == ver1 );
	EXPECT_TRUE( pver2 == ver2 );
	EXPECT_TRUE( pver3 == ver3 );
}

// Tests the Version compatibility
TEST_F( VersionFixture, Compatibility )	{
	EXPECT_TRUE( Version( 10000000 ).isCompatible( Version( 13443233 )) );
	EXPECT_FALSE( Version( 13443233 ).isCompatible( Version( 10000000 )) );
	EXPECT_TRUE( Version( 13443233 ).isCompatible( Version( 13443233 )) );
	EXPECT_FALSE( Version( 13443233 ).isCompatible( Version( 23443233 )) );
}

int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
