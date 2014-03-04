#include "PostgreSQLserverSettings.hpp"
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


bool PostgreSQLserverSettings::load( PGconn* conn)
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


