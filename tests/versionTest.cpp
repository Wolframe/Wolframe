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
// version class unit tests using google test framework (gTest)
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
		major = (unsigned short)( rand() % USHRT_MAX );
		minor = (unsigned short)( rand() % USHRT_MAX );
		revision = (unsigned short)( rand() % USHRT_MAX );
		build = (unsigned)( rand() % UINT_MAX );
		if ( !build )
			build++;

		ver = new _Wolframe::Version( major, minor, revision, build );
		ver0 = new _Wolframe::Version( major, minor, revision );
		sprintf( verStr, "%d.%d.%d.%u", major, minor, revision, build );
	}

	// Clean-up work that doesn't throw exceptions here.
	virtual ~VersionFixture()	{
		delete ver0;
		delete ver;
	}


	// If the constructor and destructor are not enough for setting up
	// and cleaning up each test, you can define the following methods:
	//   Code here will be called immediately after the constructor (right
	//   before each test).
	virtual void SetUp() {
	}

	//   Code here will be called immediately after each test (right
	//   before the destructor).
	virtual void TearDown() {
	}

	// Objects declared here can be used by all tests in the test case.
	unsigned short	major, minor, revision;
	unsigned	build;
	_Wolframe::Version	*ver, *ver0;
	char		verStr[MAX_STRING];
};


// Tests the Version constructors and members
TEST_F( VersionFixture, Members )	{
	ASSERT_EQ( ver->Major(), major );
	ASSERT_EQ( ver->Minor(), minor );
	ASSERT_EQ( ver->Revision(), revision );
	ASSERT_EQ( ver->Build(), build );
	ASSERT_STREQ( ver->toString().c_str(), verStr );
}

// Tests the Version operators
TEST_F( VersionFixture, Operators )	{
	ASSERT_TRUE( *ver != *ver0 );
	ASSERT_TRUE( *ver > *ver0 );
	ASSERT_TRUE( *ver >= *ver0 );
	ASSERT_FALSE( *ver == *ver0 );
	ASSERT_FALSE( *ver < *ver0 );
	ASSERT_FALSE( *ver <= *ver0 );
}

// Tests the Version pattern printing
TEST_F( VersionFixture, Printing )	{
	ASSERT_STREQ( ver->toString( "%M.%m.%r.%b" ).c_str(), verStr );
	ASSERT_STRNE( ver->toString( "%M.%m.%r" ).c_str(), verStr );
}


int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
