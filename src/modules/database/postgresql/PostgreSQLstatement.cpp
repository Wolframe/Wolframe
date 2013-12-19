#include "PostgreSQLstatement.hpp"
#include "types/variant.hpp"
#include <boost/cstdint.hpp>
#include <cstring>
#include <limits>
#include <stdint.h>
#include <libpq-fe.h>
#if defined( _WIN32 )
#include <winsock2.h>
#else
#include <sys/types.h>
#include <netinet/in.h>
#endif


using namespace _Wolframe;
using namespace _Wolframe::db;

PostgreSQLstatement::PostgreSQLstatement( const PostgreSQLstatement& o)
	:m_paramarsize(o.m_paramarsize)
	,m_buf(o.m_buf)
{
	std::memcpy( m_paramofs, o.m_paramofs, m_paramarsize * sizeof(*m_paramofs));
	std::memcpy( m_paramtype, o.m_paramtype, m_paramarsize * sizeof(*m_paramtype));
}


PostgreSQLstatement::PostgreSQLstatement()
	:m_paramarsize(0){}


void PostgreSQLstatement::clear()
{
	m_paramarsize = 0;
	m_buf.clear();
	m_stmstr.clear();
}

void PostgreSQLstatement::init( const std::string& stmstr)
{
	clear();
	m_stmstr = stmstr;
}

void PostgreSQLstatement::bind( unsigned int idx, const types::Variant& value)
{
	if (idx != ((unsigned int)m_paramarsize +1)) throw std::logic_error("iternal: wrong order of bind param in postgreSQL database module");

	switch (value.type())
	{
		case types::Variant::Null:
			bindNull();
			break;

		case types::Variant::Int:
			if (value.data().value.Int <= std::numeric_limits<boost::int16_t>::max())
			{
				bindInt16( (boost::int16_t)value.data().value.Int);
			}
			else if (value.data().value.Int <= std::numeric_limits<boost::int32_t>::max())
			{
				bindInt32( (boost::int32_t)value.data().value.Int);
			}
			else
			{
				bindInt64( value.data().value.Int);
			}
			break;

		case types::Variant::UInt:
			if (value.data().value.UInt <= std::numeric_limits<boost::uint16_t>::max())
			{
				bindUInt16( (boost::uint16_t)value.data().value.UInt);
			}
			else if (value.data().value.UInt <= std::numeric_limits<boost::uint32_t>::max())
			{
				bindUInt32( (boost::uint32_t)value.data().value.UInt);
			}
			else
			{
				bindUInt64( value.data().value.UInt);
			}
			break;

		case types::Variant::Double:
			bindDouble( value.data().value.Double);
			break;

		case types::Variant::Bool:
			bindBool( value.data().value.Bool);
			break;

		case types::Variant::String:
			bindString( value.charptr(), value.charsize());
			break;
	}
}

//\remark See implementation of pq_sendint64
void PostgreSQLstatement::bindUInt64( boost::uint64_t value, const char* type)
{
	boost::uint32_t pp[2];
	pp[0] = (boost::uint32_t) (value >> 32);
	pp[0] = htonl( pp[0]);
	pp[1] = (boost::uint32_t) (value);
	pp[1] = htonl( pp[1]);
	setNextParam( (const void*)&pp, sizeof(pp), type);
}

void PostgreSQLstatement::bindInt64( boost::int64_t value)
{
	bindUInt64( (boost::uint64_t)value, "int8");
}

void PostgreSQLstatement::bindUInt32( boost::uint32_t value, const char* type)
{
	boost::uint32_t pp;
	pp = htonl( value);
	setNextParam( &pp, sizeof(pp), type);
}

void PostgreSQLstatement::bindInt32( boost::int32_t value)
{
	bindUInt32( (boost::uint32_t)value, "int4");
}

void PostgreSQLstatement::bindUInt16( boost::uint16_t value, const char* type)
{
	boost::uint16_t pp = htons( value);
	setNextParam( &pp, sizeof(pp), type);
}

void PostgreSQLstatement::bindInt16( boost::int16_t value)
{
	bindUInt16( (boost::uint16_t)value, "int2");
}

void PostgreSQLstatement::bindByte( boost::uint8_t value, const char* type)
{
	setNextParam( &value, sizeof(value), type);
}

void PostgreSQLstatement::bindByte( boost::int8_t value)
{
	bindByte( (boost::uint8_t)value, "int1");
}

void PostgreSQLstatement::bindBool( bool value)
{
	bindByte( value?1:0, "uint1");
}

//\remark See implementation of pq_sendfloat8
void PostgreSQLstatement::bindDouble( double value)
{
	union
	{
		double f;
		boost::uint64_t i;
	} swap;
	swap.f = value;
	bindUInt64( swap.i, "float8");
}

void PostgreSQLstatement::bindString( const char* value, std::size_t size)
{
	setNextParam( value, size, "");
	m_buf.push_back( '\0');
}

void PostgreSQLstatement::bindNull()
{
	if (m_paramarsize > (int)MaxNofParam)
	{
		throw std::runtime_error( "Too many parameters in statement");
	}
	m_paramofs[ m_paramarsize] = 0;
	m_paramtype[ m_paramarsize] = 0;
	m_paramlen[ m_paramarsize] = 0;
	m_parambinary[ m_paramarsize] = 1;
	++m_paramarsize;
}

void PostgreSQLstatement::setNextParam( const void* ptr, unsigned int size, const char* type)
{
	if (m_paramarsize > (int)MaxNofParam)
	{
		throw std::runtime_error( "Too many parameters in statement");
	}
	m_paramofs[ m_paramarsize] = m_buf.size();
	m_paramtype[ m_paramarsize] = type;
	m_paramlen[ m_paramarsize] = size;
	m_parambinary[ m_paramarsize] = (type && type[0])?1:0;

	m_buf.append( (const char*)ptr, size);
	++m_paramarsize;
}

std::string PostgreSQLstatement::statementString() const
{
	std::string rt;
	std::string::const_iterator si = m_stmstr.begin(), se = m_stmstr.end();
	std::string::const_iterator chunkstart = si;

	for (; si != se; ++si)
	{
		if (*si == '\'' || *si == '\"')
		{
			// ignore contents in string:
			char eb = *si;
			for (++si; si != se && *si != eb; ++si)
			{
				if (*si == '\\')
				{
					++si;
					if (si == se) break;
				}
			}
			if (si == se) throw std::runtime_error( "string not terminated in statement");
		}
		if (*si == '$')
		{
			if (si > chunkstart)
			{
				rt.append( chunkstart, si);
				chunkstart = si;
			}
			int idx = 0;
			for (++si; si != se && *si >= '0' && *si <= '9'; ++si)
			{
				idx *= 10;
				idx += (*si - '0');
				if (idx > MaxNofParam) throw std::runtime_error( "parameter index out of range");
			}
			if (si != se)
			{
				if ((*si|32) >= 'a' && (*si|32) <= 'z') throw std::runtime_error( "illegal parameter index (immediately followed by identifier)");
				if (*si == '_') throw std::runtime_error( "illegal parameter index (immediately followed by underscore)");
			}
			if (idx == 0 || idx > m_paramarsize) throw std::runtime_error( "parameter index out of range");
			if (m_paramtype[ idx-1])
			{
				rt.append( "$");
				rt.append( chunkstart, si);
				if (m_paramtype[ idx-1][0])
				{
					rt.append( "::");
					rt.append( m_paramtype[ idx-1]);
				}
			}
			else
			{
				rt.append( "NULL");
			}
			chunkstart = si;
			if (si == se) break;
		}
	}
	if (si > chunkstart)
	{
		rt.append( chunkstart, si);
	}
	return rt;
}

void PostgreSQLstatement::getParams( Params& params) const
{
	params.paramarsize = m_paramarsize;
	for (int ii=0; ii<m_paramarsize; ++ii)
	{
		if (m_paramtype[ ii])
		{
			params.paramar[ ii] = (m_buf.c_str() + m_paramofs[ ii]);
		}
		else
		{
			params.paramar[ ii] = 0;
		}
	}
}

PGresult* PostgreSQLstatement::execute( PGconn *conn) const
{
	std::string command = statementString();
	Params params;
	getParams( params);

	return PQexecParams(
			conn, command.c_str(), params.paramarsize, 0/*no OIDs*/, 
			params.paramar, m_paramlen, m_parambinary, 1/*result binary*/);
}



