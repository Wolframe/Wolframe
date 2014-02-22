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
#include <cstring>
#include <cstdlib>
#include <boost/cstdint.hpp>

using namespace _Wolframe;

class BigNumberTest
	:public ::testing::Test
{
protected:
	BigNumberTest()
	{
		srand(123);
	}
	virtual ~BigNumberTest() {}
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
	unsigned short precision = (unsigned short)shortRand()+1;
	bool sign = (rand()%2 == 1);
	signed short scale = (signed short)shortRand();
	std::string buf;

	for (unsigned short ii=0; ii<precision; ++ii)
	{
		buf.push_back( (char)(rand()%10));
	}
	unsigned short lz=0; //...leading zeros to cut away
	for (; lz<precision && buf[lz] == 0; ++lz){}
	precision -= lz;
	const unsigned char* digits = (const unsigned char*)buf.c_str() + lz;
	while (precision > 0 && scale > 0 && digits[ precision-1] == 0)
	{
		//... cut away superfluous ending zeros in the fractional part of the number
		--precision;
		--scale;
	}
	if (precision == 0)
	{
		//... map -0 to +0 (no sign for zero) 
		sign = false;
		scale = 0;
	}
	return types::BigNumber( sign, precision, scale, digits);
}

static bool isequal( const types::BigNumber& aa, const types::BigNumber& bb)
{
	if (aa.precision() != bb.precision()) return false;
	if (aa.scale() != bb.scale()) return false;
	if (aa.sign() != bb.sign()) return false;
	if (0!=std::memcmp( aa.digits(), bb.digits(), aa.size())) return false;
	return true;
}

TEST_F( BigNumberTest, tests)
{
	for (unsigned int ii=0; ii<40000; ++ii)
	{
		types::BigNumber num = getRandomBigNumber();
		types::BigNumber oth( num.tostring());
		EXPECT_TRUE( isequal( num, oth));
		types::BigNumber normnum( num.tostringNormalized());
		EXPECT_TRUE( isequal( num, normnum));
	}
}

int main( int argc, char **argv)
{
	::testing::InitGoogleTest( &argc, argv);
	return RUN_ALL_TESTS();
}


