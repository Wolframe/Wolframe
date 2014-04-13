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
///\file PostgreSQLstatement.cpp
#include "PostgreSQLstatement.hpp"
#include "types/variant.hpp"
#include "types/datetime.hpp"
#include "types/bignumber.hpp"
#include "types/customDataType.hpp"
#include <boost/cstdint.hpp>
#include <boost/lexical_cast.hpp>
#include <cstring>
#include <stdint.h>
#include <libpq-fe.h>

#if defined( _WIN32 )
#define WIN32_MEAN_AND_LEAN
#include <winsock2.h>
#else
#include <arpa/inet.h>
#endif


using namespace _Wolframe;
using namespace _Wolframe::db;

PostgreSQLstatement::PostgreSQLstatement( const PostgreSQLstatement& o)
	:BaseStatement(o)
	,m_paramarsize(o.m_paramarsize)
	,m_buf(o.m_buf)
	,m_conn(o.m_conn)
	,m_settings(o.m_settings)
{
	std::memcpy( m_paramofs, o.m_paramofs, m_paramarsize * sizeof(*m_paramofs));
	std::memcpy( m_paramtype, o.m_paramtype, m_paramarsize * sizeof(*m_paramtype));
}


PostgreSQLstatement::PostgreSQLstatement()
	:BaseStatement()
	,m_paramarsize(0)
	,m_conn(0)
	{}


void PostgreSQLstatement::clear()
{
	BaseStatement::clear();
	m_paramarsize = 0;
	m_buf.clear();
}

void PostgreSQLstatement::setConnection( PGconn *conn, const PostgreSQLserverSettings& settings_)
{
	m_conn = conn;
	m_settings = settings_;
}

void PostgreSQLstatement::bind( const unsigned int idx, const types::Variant& value)
{
	// does boundary checking
	BaseStatement::bind( idx, value );

	bindVariant( idx, value);
}

void PostgreSQLstatement::bindVariant( unsigned int idx, const types::Variant& value)
{
	if (idx != ((unsigned int)m_paramarsize +1)) throw std::logic_error("internal: wrong order of bind param in postgreSQL database module");

	switch (value.type())
	{
		case types::Variant::Null:
			bindNull();
			break;

		case types::Variant::Int:
			// numeric_limits::max does not work with windows includes
			if (value.data().value.Int <= 0x7FFF && value.data().value.Int >= -0x7FFF)
			{
				bindInt16( (boost::int16_t)value.data().value.Int);
			}
			else if (value.data().value.Int <= 0x7FFFffff && value.data().value.Int >= -(0x7FFFffff))
			{
				bindInt32( (boost::int32_t)value.data().value.Int);
			}
			else
			{
				bindInt64( value.data().value.Int);
			}
			break;

		case types::Variant::UInt:
			if (value.data().value.UInt <= 0xFFff)
			{
				bindUInt16( (boost::uint16_t)value.data().value.UInt);
			}
			else if (value.data().value.UInt <= 0xFFFFffff)
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

		case types::Variant::Timestamp:
		{
			std::string strval = types::DateTime( value.totimestamp()).tostring( types::DateTime::StringFormat::ExtendedISOdateTime);
			bindString( strval.c_str(), strval.size());
			break;
		}
		case types::Variant::BigNumber:
		{
			std::string strval = value.tostring();
			bindString( strval.c_str(), strval.size());
			break;
		}
		case types::Variant::Custom:
		{
			types::Variant baseval;
			try
			{
				if (value.customref()->getBaseTypeValue( baseval)
				&&  baseval.type() != types::Variant::Custom)
				{
					bindVariant( idx, baseval);
					break;
				}
			}
			catch (const std::runtime_error& e)
			{
				throw std::runtime_error( std::string("cannot convert value to base type for binding: ") + e.what());
			}
			bindVariant( idx, value.tostring());
			break;
		}
		default:
			throw std::logic_error( "Binding unknown type '" + std::string( value.typeName( ) ) + "'" );
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
	bindByte( value?1:0, "bool");
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
	m_paramofs[ m_paramarsize] = 0;
	m_paramtype[ m_paramarsize] = 0;
	m_paramlen[ m_paramarsize] = 0;
	m_parambinary[ m_paramarsize] = 1;
	++m_paramarsize;
}

void PostgreSQLstatement::setNextParam( const void* ptr, unsigned int size, const char* type)
{
	m_paramofs[ m_paramarsize] = m_buf.size();
	m_paramtype[ m_paramarsize] = type;
	m_paramlen[ m_paramarsize] = size;
	m_parambinary[ m_paramarsize] = (type && type[0])?1:0;

	m_buf.append( (const char*)ptr, size);
	++m_paramarsize;
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

PGresult* PostgreSQLstatement::execute( ) const
{
	std::string command = nativeSQL();
	Params params;
	getParams( params);

	// KLUDGE: format text for now as the parser in the state machine
	// expects it!
	return PQexecParams(
			m_conn, command.c_str(), params.paramarsize, 0/*no OIDs*/,
			params.paramar, m_paramlen, m_parambinary, 0);
}

const std::string PostgreSQLstatement::replace( const unsigned int idx ) const
{
	std::string rt;
	
	rt.append( "$" );
	rt.append( boost::lexical_cast< std::string >( idx ) );
	if( m_paramtype[idx-1] && m_paramtype[idx-1][0] ) {
		rt.append( "::" );
		rt.append( m_paramtype[idx-1] );
	}
	
	return rt;
}
			
