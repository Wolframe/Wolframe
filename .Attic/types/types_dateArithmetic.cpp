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
///\file types_dateArithmetic.cpp
///\brief Date arithmetic functions
///\remark based on http://alcor.concordia.ca/~gpkatch/gdate-algorithm.html: Date arithmetics algorithm's described and implemented by Gary Katch
#include "types/dateArithmetic.hpp"
#include <stdexcept>
#include <cstring>
#include "boost/date_time/gregorian/gregorian.hpp"

using namespace _Wolframe;
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
static long g_mindaynum = daynum( g_mindate);

///\brief Convert day number to y,m,d format
///\remark Original source from http://alcor.concordia.ca/~gpkatch/gdate-c.html
struct sdate get_sdate( long d)
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
	std::istringstream dti(dt);
	boost::gregorian::date_facet dtf( format);
	dti.imbue( std::locale( std::locale::classic(), &dtf));
	boost::gregorian::date gdate;
	dti >> gdate;

	sdate dd;
	dd.y = gdate.year();
	dd.m = gdate.month();
	dd.d = gdate.day();

	m_daynum = daynum( dd);
}

std::string Date::tostring( const char* format) const
{
	std::ostringstream dto;
	boost::gregorian::date_facet dtf( format);
	dto.imbue( std::locale( std::locale::classic(), &dtf));
	sdate thisdate = get_sdate( m_daynum);
	boost::gregorian::date gdate( thisdate.y, thisdate.m, thisdate.d);
	dto << gdate;
	return dto.str();
}

