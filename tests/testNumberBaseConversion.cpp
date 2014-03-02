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
///\file testNumberBaseConversion.cpp
///\brief Test for types/numberBaseConversion.hpp
#include "libwolframe/types/numberBaseConversion.hpp"
#include "gtest/gtest.h"
#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <boost/cstdint.hpp>
#include <boost/lexical_cast.hpp>

 
using namespace _Wolframe;

class NumberBaseConversions
	:public ::testing::Test
{
protected:
	NumberBaseConversions()
	{
		srand(435);
	}
	virtual ~NumberBaseConversions() {}
	virtual void SetUp() {}
	virtual void TearDown() {}
};

enum ByteOrder {BigEndian,LittleEndian};
static ByteOrder getByteOrder()
{
	unsigned char rt[2];
	static const unsigned short test = {0x0100};
	std::memcpy( &rt, &test, 2);
	if (rt[0])
	{
		return BigEndian;
	}
	else
	{
		return LittleEndian;
	}
}
static ByteOrder g_ByteOrder = getByteOrder();

template <typename NUMTYPE>
static NUMTYPE getRandomNumber()
{
	enum {NumSize=sizeof(NUMTYPE)};
	unsigned char num[NumSize];

	std::memset( num, 0, sizeof(num));
	unsigned int ii=0, nn=rand()%NumSize;


	switch (g_ByteOrder)
	{
		case LittleEndian:
			for (; ii<nn; ++ii)
			{
				num[ ii] = rand()%256;
			}
			break;
		case BigEndian:
			for (; ii<nn; ++ii)
			{
				num[ NumSize-ii-1] = rand()%256;
			}
			break;
	}
	NUMTYPE rt;
	std::memcpy( &rt, num, sizeof(rt));
	return rt;
}

template <typename UINTTYPE>
static void reorder( UINTTYPE& num)
{
	unsigned char* ref = (unsigned char*)&num;
	unsigned int ii=0, nn=sizeof(UINTTYPE)/2;
	for (; ii<nn; ++ii)
	{
		unsigned char tmp = ref[ii];
		ref[ii] = ref[ sizeof(UINTTYPE)-ii-1];
		ref[ sizeof(UINTTYPE)-ii-1] = tmp;
	}
}

TEST_F( NumberBaseConversions, tests)
{
	for (unsigned int testcnt=0; testcnt<40000; ++testcnt)
	{
		enum {DigitsBufSize=50};
		unsigned char digitsbuf[ DigitsBufSize];

		const boost::uint64_t num = getRandomNumber<boost::uint64_t>();
		boost::uint64_t num_be = num;

		if (g_ByteOrder == LittleEndian)
		{
			reorder( num_be);
		}
		/*[-]*/std::cout << "TEST " << num << std::endl;

		unsigned int nofDigits = types::convertBigEndianUintToBCD( num_be, digitsbuf, DigitsBufSize);
		std::string numstr;
		for (unsigned int ii=0; ii<nofDigits; ++ii)
		{
			numstr.push_back( '0' + digitsbuf[ii]);
		}
		EXPECT_EQ( boost::lexical_cast<std::string>( num), numstr);

		boost::uint64_t num_reverted_be;
		types::convertBCDtoBigEndianUint( digitsbuf, nofDigits, num_reverted_be);
		boost::uint64_t num_reverted = num_reverted_be;
		if (g_ByteOrder == LittleEndian)
		{
			reorder( num_reverted);
		}
		EXPECT_EQ( num_reverted, num);
	}
}

int main( int argc, char **argv)
{
	::testing::InitGoogleTest( &argc, argv);
	return RUN_ALL_TESTS();
}


