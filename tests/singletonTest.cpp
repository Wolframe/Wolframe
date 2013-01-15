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
// tests singleton, especially cleanup and live time issues when destructing
//

#include "types/singleton.hpp"

#include "gtest/gtest.h"

#include <string>

class TestObject : public Singleton< TestObject > {
private:
	std::string *somedata;

public:
	TestObject( ) {
		somedata = new std::string( "bla" );
	}

	virtual ~TestObject( ) {
		delete somedata;
	}
};

// The fixture for testing _Wolframe::Singleton
class SingletonFixture : public ::testing::Test	{
protected:
	SingletonFixture( ) {
	}

	virtual ~SingletonFixture( ) {
	}
};

// Tests to check basic functionality of the singleton
// (implicitly tests for memory leaks in the destructor of
// the test object)
TEST_F( SingletonFixture, basic ) {
	TestObject *o1 = &TestObject::instance( );
	TestObject *o2 = &TestObject::instance( );

	ASSERT_EQ( o1, o2 );
}

int main( int argc, char *argv[] )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS( );
}
