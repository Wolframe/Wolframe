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
///\file PostgreSQLServerSettings.cpp
#include "PostgreSQLServerSettings.hpp"
#include "logger-v1.hpp"
#include <boost/algorithm/string.hpp>
#include <stdarg.h> 

using namespace _Wolframe;
using namespace _Wolframe::db;

//\brief Parse a flag and return its index in a list of candidates
static int status( const char* res, unsigned int nn, ...)
{
	if (!res) return -1;

	int rt = -1;
	va_list vl;
	va_start( vl, nn);
	for (unsigned int ii=0; ii<nn; ii++)
	{
		const char* candidate = va_arg( vl, const char*);
		if (boost::iequals( candidate, res))
		{
			rt = (int)ii;
			break;
			
		}
	}
	va_end( vl);
	return rt;
}


bool PostgreSQLServerSettings::load( PGconn* conn)
{
	LOG_DEBUG << "Reading binary protocol settings of server:";

	const char* integer_datetimes = PQparameterStatus( conn, "integer_datetimes");
	switch (status( integer_datetimes, 2, "on", "off"))
	{
		case 0: m_binaryTimestampFormat = TimestampAsInt; break;
		case 1: m_binaryTimestampFormat = TimestampAsDouble; break;
		default: integer_datetimes = "undefined"; m_binaryTimestampFormat = TimestampFormatUndefined; break;
	}
	LOG_DEBUG << "set protocol flag integer_datetimes = " << integer_datetimes;
	return true;
}


