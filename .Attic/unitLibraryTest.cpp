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
// function library template unit tests
//

#include "unitLibrary.hpp"
#include "gtest/gtest.h"
#include <boost/lexical_cast.hpp>


// The fixture for testing Wolframe::ObjectPool template
class FunctionLibraryFixture : public ::testing::Test	{
protected:
	// Set-up work for each test here.
	FunctionLibraryFixture()	{
		libSize = 47;
	}

	// Clean-up work that doesn't throw exceptions here.
	virtual ~FunctionLibraryFixture()	{
	}

	// Code here will be called immediately after the constructor
	// (right before each test).
	virtual void SetUp() {
	}

	// Code here will be called immediately after each test (right
	// before the destructor).
	virtual void TearDown() {
	}

	// Objects declared here can be used by all tests in the test case.
	_Wolframe::UnitLibrary< int >	library;
	size_t				libSize;
public:
};


// Tests the ObjectPool get & release
TEST_F( FunctionLibraryFixture, Add_Retrieve )	{
	for ( size_t i = 0; i < libSize; i++ )	{
		bool ret = library.addFunction( i, boost::lexical_cast<std::string>( i ) );
		ASSERT_TRUE( ret );
	}
	for ( size_t i = 0; i < libSize; i++ )	{
		const int* ret = library.function( boost::lexical_cast<std::string>( i ) );
		ASSERT_NE( (const int*)0, ret );
		ASSERT_EQ( *ret, i );
	}
}


int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
