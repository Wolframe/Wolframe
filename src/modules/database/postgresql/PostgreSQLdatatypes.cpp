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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
//\brief Implementation for higher data type transformations for libpq
//\remark The implementation of the data types are taken from libpqtype but with another interface
//\file PostgreSQLdatatypes.cpp
#include "PostgreSQLdatatypes.hpp"

using namespace _Wolframe;
using namespace _Wolframe::db;

#define POSTGRES_EPOCH_JDATE	2451545		/* == date2j(2000, 1, 1) */
#define HRS_PER_DAY		24L
#define MINS_PER_DAY		(60L*HRS_PER_DAY)
#define SECS_PER_DAY		(60L*MINS_PER_DAY)
#define MSECS_PER_DAY		(1000ULL*SECS_PER_DAY)
#define USECS_PER_DAY		(1000ULL*MSECS_PER_DAY)

//\remark Code from liubrary libpqtypes (datetime.c)
static PostgresDate date2j(int y, int m, int d)
{
	PostgresDate julian;
	PostgresDate century;

	if (m > 2)
	{
		m += 1;
		y += 4800;
	}
	else
	{
		m += 13;
		y += 4799;
	}
	century = y / 100;
	julian = y * 365 - 32167;
	julian += y / 4 - century + century / 4;
	julian += 7834 * m / 256 + d;

	return julian;
}

//\remark Code from liubrary libpqtypes (datetime.c)
static void j2date(int jd, int *year, int *month, int *day)
{
	unsigned int julian;
	unsigned int quad;
	unsigned int extra;
	int y;

	julian = jd;
	julian += 32044;
	quad = julian / 146097;
	extra = (julian - quad * 146097) * 4 + 3;
	julian += 60 + quad * 3 + extra / 146097;
	quad = julian / 1461;
	julian -= quad * 1461;
	y = julian * 4 / 1461;
	julian = ((y != 0) ? ((julian + 305) % 365) : ((julian + 306) % 366)) + 123;
	y += quad * 4;
	*year = y - 4800;
	quad = julian * 2141 / 65536;
	*day = julian - 7834 * quad / 256;
	*month = (quad + 10) % 12 + 1;
}

PostgresDate db::getDateParam( const types::DateTime& dt)
{
	return date2j( dt.year(), dt.month(), dt.day()) - POSTGRES_EPOCH_JDATE;
}

types::DateTime getDateTime( const PostgresDate& in)
{
	int year;
	int month;
	int day;

	j2date( in + POSTGRES_EPOCH_JDATE, &year, &month, &day);
	return types::DateTime( year, month, day);
}

PostgresTimestamp db::getTimestampParam( const types::DateTime& dt)
{
	PostgresTimestamp rt = date2j( dt.year(), dt.month(), dt.day()) - POSTGRES_EPOCH_JDATE;
	rt *= USECS_PER_DAY;
	return rt
		+ dt.hour()		* (USECS_PER_DAY/HRS_PER_DAY)
		+ dt.minute()		* (USECS_PER_DAY/MINS_PER_DAY)
		+ dt.second()		* (USECS_PER_DAY/SECS_PER_DAY)
		+ dt.millisecond()	* (USECS_PER_DAY/MSECS_PER_DAY)
		+ dt.microsecond()	* (USECS_PER_DAY/USECS_PER_DAY);
}

types::DateTime db::getDateTime( const PostgresTimestamp& in)
{
	int year;
	int month;
	int day;
	int hour;
	int minute;
	int second;
	int usecond;

	j2date( (int)(in/USECS_PER_DAY) + POSTGRES_EPOCH_JDATE, &year, &month, &day);
	PostgresTimestamp rest = in % USECS_PER_DAY;
	usecond = rest % 1000000;
	rest /= 1000000;
	second = rest % 60;
	rest /= 60;
	minute = rest % 60;
	rest /= 60;
	hour = rest % 24;

	return types::DateTime( year, month, day, hour, minute, second, usecond);
}


