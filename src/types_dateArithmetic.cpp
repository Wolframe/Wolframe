/************************************************************************

 Copyright (C) 2011, 2012 Project Wolframe.
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
///\file types_dateArithmetic.cpp
///\brief Date arithmetic functions
///\remark based on http://alcor.concordia.ca/~gpkatch/gdate-algorithm.html: Date arithmetics algorithm's described and implemented by Gary Katch
#include "types/dateArithmetic.hpp"
#include <stdexcept>
#include <cstring>
#include <ctime>

using namespace _Wolframe::types;

namespace
{
struct sdate
{
	long y;
	long m;
	long d;
};
}

///\brief Convert date to day number
///\remark Original source from http://alcor.concordia.ca/~gpkatch/gdate-c.html
static long daynum( sdate d)
{
	long y, m;

	m = (d.m + 9)%12;	/* mar=0, feb=11 */
	y = d.y - m/10;		/* if Jan/Feb, year-- */
	return y*365 + y/4 - y/100 + y/400 + (m*306 + 5)/10 + (d.d - 1);
}

static sdate g_mindate = {1600, 1, 1};
static sdate g_sundaydate = {1600, 1, 2};
static long g_mindaynum = daynum( g_mindate);
static long g_sundaydaynum = daynum( g_sundaydate);

static short weekday( long d)
{
	return (d - g_sundaydaynum + 7) % 7;
}

///\brief Convert day number to y,m,d format
///\remark Original source from http://alcor.concordia.ca/~gpkatch/gdate-c.html
struct sdate dtf( long d)
{
	struct sdate pd;
	long y, ddd, mi;

	y = (10000*d + 14780)/3652425;
	ddd = d - (y*365 + y/4 - y/100 + y/400);
	if (ddd < 0)
	{
		y--;
		ddd = d - (y*365 + y/4 - y/100 + y/400);
	}
	mi = (52 + 100*ddd)/3060;
	pd.y = y + (mi + 2)/12;
	pd.m = (mi + 2)%12 + 1;
	pd.d = ddd - (mi*306 + 5)/10 + 1;
	return pd;
}


void Date::check()
{
	if (m_daynum < g_mindaynum)
	{
		throw std::runtime_error( "bad date in calculation");
	}
}

Date::Date( unsigned short y, unsigned short m, unsigned short d)
{
	sdate dt;
	dt.y = y;
	dt.m = m;
	dt.d = d;
	m_daynum = daynum( dt);
	check();
}

Date::Date( const std::string& dt, const char* format)
{
	struct tm tt;
	std::memset( &tt, 0, sizeof(tt));
	if (!strptime( dt.c_str(), format,  &tt))
	{
		throw std::runtime_error( "date conversion error");
	}
	sdate sdt;
	sdt.y = tt.tm_year;
	sdt.m = tt.tm_mon;
	sdt.d = tt.tm_mday;

	m_daynum = daynum( sdt);
}

std::string Date::tostring( const char* format) const
{
	char buf[ 1024];
	if (strlen( format) > 50) throw std::bad_alloc();
	struct tm tt;
	std::memset( &tt, 0, sizeof(tt));
	sdate dt = dtf( m_daynum);
	sdate dt_first_jan;
	dt_first_jan.y = dt.y;
	dt_first_jan.m = 1;
	dt_first_jan.d = 1;
	tt.tm_mday = dt.d;
	tt.tm_mon = dt.m;
	tt.tm_year = dt.y;
	tt.tm_wday = weekday( m_daynum);
	tt.tm_yday = m_daynum - daynum( dt_first_jan);

	strftime( buf, sizeof(buf), format, &tt);
	return std::string( buf);
}


