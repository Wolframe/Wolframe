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

static types::BigNumber getRandomBigNumber()
{
	unsigned short YY = (unsigned short)(1000 + rand()%1399);
	unsigned short MM = (unsigned short)(1+rand()%12);
	unsigned short DD = (unsigned short)(1+rand()%31);
	unsigned short hh = (unsigned short)(rand()%24);
	unsigned short mm = (unsigned short)(rand()%60);
	unsigned short ss = (unsigned short)(rand()%64);
	unsigned short ll = (unsigned short)(rand()%1000);
	unsigned short cc = (unsigned short)(rand()%1000);

	switch (rand()%4)
	{
		case 0: return types::DateTime( YY,MM,DD,hh,mm,ss,ll,cc);
		case 1: return types::DateTime( YY,MM,DD,hh,mm,ss,ll);
		case 2: return types::DateTime( YY,MM,DD,hh,mm,ss);
		case 3: return types::DateTime( YY,MM,DD);
	}
	throw std::logic_error("illegal state");
}


TEST_F( BigNumberDescriptionTest, tests)
{
}

int main( int argc, char **argv)
{
	::testing::InitGoogleTest( &argc, argv);
	return RUN_ALL_TESTS();
}


