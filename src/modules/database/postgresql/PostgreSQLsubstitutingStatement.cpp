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
///\file PostgreSQLsubstitutingStatement.cpp
#include "PostgreSQLsubstitutingStatement.hpp"
#include "types/datetime.hpp"
#include "types/bignumber.hpp"
#include "types/customDataType.hpp"
#include <boost/shared_ptr.hpp>

using namespace _Wolframe;
using namespace _Wolframe::db;

PostgreSQLsubstitutingStatement::PostgreSQLsubstitutingStatement( )
	: SubstitutingStatement( ),
	m_conn( 0 )
{
}

PostgreSQLsubstitutingStatement::PostgreSQLsubstitutingStatement( const PostgreSQLsubstitutingStatement &o )
	: SubstitutingStatement( o ),
	m_conn( 0 )
{
}

void PostgreSQLsubstitutingStatement::setConnection( PGconn *conn )
{
	m_conn = conn;
}

const std::string PostgreSQLsubstitutingStatement::convert( const types::Variant &value ) const
{
	switch( value.type( ) ) {
		
		case types::Variant::Null:
			return "NULL";
			
		case types::Variant::Int:
		case types::Variant::UInt:
		case types::Variant::Double:
		case types::Variant::BigNumber:
			return value.tostring( );
			
		case types::Variant::Custom:
		{
			types::Variant baseval;
			try
			{
				if (value.customref()->getBaseTypeValue( baseval)
				&&  baseval.type() != types::Variant::Custom)
				{
					return convert( baseval);
				}
			}
			catch (const std::runtime_error& e)
			{
				throw std::runtime_error( std::string("cannot convert value to base type for binding: ") + e.what());
			}
			std::string strval = value.tostring();
			return convert( strval);
		}
		case types::Variant::Timestamp:
		{
			std::string strval = types::DateTime(value.totimestamp()).tostring( types::DateTime::StringFormat::ExtendedISOdateTime);
			return convert( strval);
		}
		case types::Variant::String:
		{
			std::string strval = value.tostring();
			char *encvalue = (char *)std::malloc( strval.size() * 2 + 3);
			encvalue[0] = '\'';
			boost::shared_ptr<void> encvaluer( encvalue, std::free);
			int error = 0;
			size_t encvaluesize = PQescapeStringConn( m_conn, encvalue+1, strval.c_str( ), strval.size( ), &error);
			if( error != 0 ) {
				throw new std::runtime_error( "Encoding error in PQescapeStringConn for string '" + strval + "'");
			}
			encvalue[encvaluesize+1] = '\'';
			return std::string( encvalue, encvaluesize+2);
		}

		case types::Variant::Bool:
			if( value.tobool( ) ) return "'t'";
			else return "'f'";
	}
	throw new std::logic_error( "Unknown variant type '" + std::string( value.typeName( ) ) + "'" );
}

PGresult *PostgreSQLsubstitutingStatement::execute( ) const
{
	return PQexec( m_conn, nativeSQL( ).c_str( ) );
}



