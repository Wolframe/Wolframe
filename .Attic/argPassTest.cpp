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
// tests for an attempt to optimize the logger
//

#include "gtest/gtest.h"
#include <iostream>
#include <string>

class TestBackend
{
public:
	TestBackend( int val )	{ m_val = val; std::cout << "backend created with value: " << m_val << "\n"; }
	~TestBackend()		{ std::cout << "backend with value: " << m_val << "destroyed\n"; }
	inline int value() const	{ return m_val; }
private:
	int	m_val;
};


class TestPrinter
{
public:
	TestPrinter( int val, TestBackend& back )
		: m_backend( back )
				{ m_val = val; std::cout << "printer created with value: " << m_val << "\n"; }
	~TestPrinter()		{ std::cout << "printer with value: " << m_val << "destroyed\n"; }

	template<typename T> friend TestPrinter& operator << ( TestPrinter& printer, T object );

protected:
	std::ostringstream	os;
private:
	int			m_val;
	const TestBackend&	m_backend;
};

template<typename T>
TestPrinter& operator << ( TestPrinter& printer, T obj )
{
	printer.os << obj;
	return printer;
}


class TestObject
{
public:
	TestObject()		{ m_val = 0; std::cout << "default constructor called\n"; }
	TestObject( int val )	{ m_val = val; std::cout << "constructor called with value: " << m_val << "\n"; }
	~TestObject()		{ std::cout << "destructor called for test value: " << m_val << "\n"; }

private:
	int	m_val;
};

// The fixture for testing the whatever....
class ArgPassingFixture : public ::testing::Test	{
protected:
	// Set-up work for each test here.
	ArgPassingFixture()	{
	}

	// Clean-up work that doesn't throw exceptions here.
	virtual ~ArgPassingFixture()	{
	}

	// Code here will be called immediately after the constructor
	// (right before each test).
	virtual void SetUp() {
	}

	// Code here will be called immediately after each test
	// (right before the destructor).
	virtual void TearDown() {
	}

	// Objects declared here can be used by all tests in the test case.
	// ----
public:
};


// Tests the objects creation and deletion
TEST_F( ArgPassingFixture, Creation_and_deletion )	{
	ASSERT_TRUE( true );
}


int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}
