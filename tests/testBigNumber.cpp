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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
///\file testBigNumber.cpp
///\brief Test for types/bignumber.hpp
#include "types/bignumber.hpp"
#include "gtest/gtest.h"
#include <iostream>
#include <string>
#include <limits>
#include <cstdlib>
#include <boost/cstdint.hpp>

using namespace _Wolframe;

class BigNumberDescriptionTest
	:public ::testing::Test
{
protected:
	BigNumberDescriptionTest()
	{
		srand(123);
	}
	virtual ~BigNumberDescriptionTest() {}
	virtual void SetUp() {}
	virtual void TearDown() {}
};

static unsigned short shortRand()
{
	int fib[16] = {1,2,3,5,8,13,21,34,55,89,144,233,377,610,987,1597};
	int dd = rand() % fib[15];
	int ii=0;
	for (; fib[ii] < dd; ++ii);
	unsigned short limit = (1 << (15-ii));
	return (rand()%limit);
}

static types::BigNumber getRandomBigNumber()
{
	bool sign = (rand()%2 == 1);
	unsigned short precision = (unsigned short)shortRand();
	signed short scale = (signed short)shortRand();
	std::string digits;

	for (unsigned short ii=0; ii<precision; ++ii)
	{
		digits.push_back( '0'+(char)(rand()%10));
	}
	return types::BigNumber( sign, precision, scale, (const unsigned char*)digits.c_str());
}


TEST_F( BigNumberDescriptionTest, tests)
{
	for (unsigned int ii=0; ii<1; ++ii)
	{
		types::BigNumber num = getRandomBigNumber();
	}
}

int main( int argc, char **argv)
{
	::testing::InitGoogleTest( &argc, argv);
	return RUN_ALL_TESTS();
}


