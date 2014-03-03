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
#define USECS_PER_DAY		86400000000ULL	/* 60*60*24*1000000 */

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

void db::writeDateParam( PostgresDate* out, const types::DateTime& dt)
{
	*out = date2j( dt.year(), dt.month(), dt.day()) - POSTGRES_EPOCH_JDATE;
}

void db::readDateParam( const PostgresDate* in, types::DateTime& dt)
{
	int year;
	int month;
	int day;

	j2date( *in + POSTGRES_EPOCH_JDATE, &year, &month, &day);
	dt = types::DateTime( year, month, day);
}


PGint8 time = *(PGint8 *) tbuf;
 316     int date = date2j(year, mon, pgts->date.mday) - POSTGRES_EPOCH_JDATE;
 317     PGint8 val = (PGint8) (date * USECS_PER_DAY + time);
