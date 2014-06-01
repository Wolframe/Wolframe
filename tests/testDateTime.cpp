/************************************************************************
Copyright (C) 2011 - 2014 Project Wolframe.
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
///\file testDateTime.cpp
///\brief Test for types/datetime.hpp
#include "types/datetime.hpp"
#include "gtest/gtest.h"
#include "wtest/testReport.hpp"
#include <iostream>
#include <string>
#include <limits>
#include <cstdlib>
#include <boost/cstdint.hpp>

using namespace _Wolframe;

class DateTimeDescriptionTest
	:public ::testing::Test
{
protected:
	DateTimeDescriptionTest()
	{
		srand(123);
	}
	virtual ~DateTimeDescriptionTest() {}
	virtual void SetUp() {}
	virtual void TearDown() {}
};

static types::DateTime getRandomDateTime()
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

static types::DateTime getRandomDateTimeIncrement( const types::DateTime& dt)
{
	unsigned short YY = dt.year();
	unsigned short MM = dt.month();
	unsigned short DD = dt.day();
	unsigned short hh = dt.hour();
	unsigned short mm = dt.minute();
	unsigned short ss = dt.second();
	unsigned short ll = dt.millisecond();
	unsigned short cc = dt.microsecond();

	for (;;)
	{
		unsigned int st = rand() % 8;
		switch (dt.subtype())
		{
			case types::DateTime::YYYYMMDD: if (st > 2) continue; break;
			case types::DateTime::YYYYMMDDhhmmss: if (st > 5) continue; break;
			case types::DateTime::YYYYMMDDhhmmss_lll: if (st > 6) continue; break;
			case types::DateTime::YYYYMMDDhhmmss_lllccc: if (st > 7) continue; break;
		}
		switch (st)
		{
			case 0: if (YY >= 2400) continue; YY+=1; break;
			case 1: if (MM >= 12) continue; MM+=1; break;
			case 2: if (DD >= 31) continue; DD+=1; break;
			case 3: if (hh >= 23) continue; hh+=1; break;
			case 4: if (mm >= 59) continue; mm+=1; break;
			case 5: if (ss >= 62) continue; ss+=1; break;
			case 6: if (ll >= 999) continue; ll+=1; break;
			case 7: if (cc >= 999) continue; cc+=1; break;
		}
		switch (dt.subtype())
		{
			case types::DateTime::YYYYMMDD: return types::DateTime( YY,MM,DD);
			case types::DateTime::YYYYMMDDhhmmss: return types::DateTime( YY,MM,DD,hh,mm,ss);
			case types::DateTime::YYYYMMDDhhmmss_lll: return types::DateTime( YY,MM,DD,hh,mm,ss,ll);
			case types::DateTime::YYYYMMDDhhmmss_lllccc: return types::DateTime( YY,MM,DD,hh,mm,ss,ll,cc);
		}
		throw std::logic_error("illegal state");
	}
}

TEST_F( DateTimeDescriptionTest, tests)
{
	for (unsigned int ii=0; ii<10000; ++ii)
	{
		types::DateTime dt1 = getRandomDateTime();
		types::DateTime dt2( dt1.tostring( types::DateTime::StringFormat::YYYYMMDDhhmmssxxxxxx));
		types::DateTime dt3( dt1.tostring( types::DateTime::StringFormat::ISOdateTime));
		types::DateTime dt4( dt1.tostring( types::DateTime::StringFormat::ExtendedISOdateTime));
		
		EXPECT_EQ( dt1.tostring(), dt2.tostring());
		EXPECT_EQ( dt1.tostring(), dt3.tostring());
		EXPECT_EQ( dt1.tostring(), dt4.tostring());

		types::DateTime dt1inc = getRandomDateTimeIncrement( dt1);
		EXPECT_TRUE( dt1 == dt2);
		EXPECT_TRUE( dt1 < dt1inc);
		EXPECT_TRUE( dt1 <= dt1inc);
		EXPECT_TRUE( dt1 != dt1inc);
		EXPECT_TRUE( dt1inc > dt1);
		EXPECT_TRUE( dt1inc >= dt1);
		EXPECT_TRUE( dt1inc != dt1);

		types::DateTime dt1tm( dt1.timestamp());
		EXPECT_TRUE( dt1 == dt1tm);
	}
}

int main( int argc, char **argv)
{
	WOLFRAME_GTEST_REPORT( argv[0], refpath.string());
	::testing::InitGoogleTest( &argc, argv);
	return RUN_ALL_TESTS();
}


