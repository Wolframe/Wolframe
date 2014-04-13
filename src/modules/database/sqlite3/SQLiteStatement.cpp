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
///\file SQLiteStatement.cpp
#include "SQLiteStatement.hpp"
#include "types/variant.hpp"
#include "types/datetime.hpp"
#include "types/bignumber.hpp"
#include "types/customDataType.hpp"

#include <boost/lexical_cast.hpp>
#include "logger-v1.hpp"
#include <stdexcept>

using namespace _Wolframe;
using namespace _Wolframe::db;

SQLiteStatement::SQLiteStatement( )
	: BaseStatement( )
{
	m_data.reserve( MaxNofParams );
}

SQLiteStatement::SQLiteStatement( const SQLiteStatement &o )
	: BaseStatement( o )
{
}

void SQLiteStatement::setStatement( sqlite3_stmt *stm )
{
	m_stm = stm;
}

int SQLiteStatement::getLastStatus( )
{
	return m_rc;
}

static int wrap_sqlite3_bind_text( sqlite3_stmt* stm, int idx, const char* s, int n, void(*f)(void*))
{
	int rt = sqlite3_bind_text( stm, idx, s, n, f);
	LOG_DATA << "call sqlite3_bind_text " << idx << " '" << ((n<0)?std::string(s):std::string(s,n)) << "' returns " << rt;
	return rt;
}
static int wrap_sqlite3_bind_int( sqlite3_stmt* stm, int idx, int val)
{
	int rt = sqlite3_bind_int( stm, idx, val);
	LOG_DATA << "call sqlite3_bind_int " << idx << " " << val << " returns " << rt;
	return rt;
}
static int wrap_sqlite3_bind_int64( sqlite3_stmt* stm, int idx, types::Variant::Data::Int val)
{
	int rt = sqlite3_bind_int64( stm, idx, val);
	LOG_DATA << "call sqlite3_bind_int64 " << idx << " " << val << " returns " << rt;
	return rt;
}
static int wrap_sqlite3_bind_double( sqlite3_stmt* stm, int idx, double val)
{
	int rt = sqlite3_bind_double( stm, idx, val);
	LOG_DATA << "call sqlite3_bind_double " << idx << " " << val << " returns " << rt;
	return rt;
}
static int wrap_sqlite3_bind_null( sqlite3_stmt* stm, int idx)
{
	int rt = sqlite3_bind_null( stm, idx);
	LOG_DATA << "call sqlite3_bind_null " << idx << " returns " << rt;
	return rt;
}

void SQLiteStatement::clear( )
{
	BaseStatement::clear( );
	m_data.clear( );
}

void SQLiteStatement::bind( const unsigned int idx, const types::Variant &value )
{
	// does boundary checking
	BaseStatement::bind( idx, value );
	bindVariant( idx, value );
}

void SQLiteStatement::bindVariant( const unsigned int idx, const types::Variant &value )
{
	switch (value.type())
	{
		case types::Variant::Null:
			m_rc = wrap_sqlite3_bind_null( m_stm, (int)idx);
			break;
			
		case types::Variant::Bool:
			m_rc = wrap_sqlite3_bind_int( m_stm, (int)idx, value.tobool());
			break;
			
		case types::Variant::Int:
			if( value.data( ).value.Int <= std::numeric_limits<signed int>::max( ) && value.data( ).value.Int >= std::numeric_limits<signed int>::min( ) ) {
				m_rc = wrap_sqlite3_bind_int( m_stm, (int)idx, (signed int)value.toint());
			} else {
				m_rc = wrap_sqlite3_bind_int64( m_stm, (int)idx, value.toint());
			}
			break;
			
		case types::Variant::UInt:
			if( value.data( ).value.UInt <= (unsigned int)std::numeric_limits<signed int>::max( ) ) {
				m_rc = wrap_sqlite3_bind_int( m_stm, (int)idx, (signed int)value.toint());
			} else if ( value.data( ).value.UInt <= (_WOLFRAME_UINTEGER)std::numeric_limits<sqlite3_int64>::max( ) ) {
				m_rc = wrap_sqlite3_bind_int64( m_stm, (int)idx, value.toint());
			} else {
				m_data.push_back( value.tostring());
				m_rc = wrap_sqlite3_bind_text( m_stm, (int)idx, m_data.back().c_str(), m_data.back().size(), SQLITE_STATIC);
			}
			break;
			
		case types::Variant::Double:
			m_rc = wrap_sqlite3_bind_double( m_stm, (int)idx, value.todouble());
			break;
			
		case types::Variant::String:
			m_rc = wrap_sqlite3_bind_text( m_stm, (int)idx, value.charptr(), value.charsize(), SQLITE_STATIC);
			break;
			
		case types::Variant::Timestamp:
		{
			m_data.push_back( types::DateTime( value.totimestamp()).tostring( types::DateTime::StringFormat::ExtendedISOdateTime));
			m_rc = wrap_sqlite3_bind_text( m_stm, (int)idx, m_data.back().c_str(), m_data.back().size(), SQLITE_STATIC);
			break;
		}
		case types::Variant::BigNumber:
		{
			m_data.push_back( value.tostring());
			m_rc = wrap_sqlite3_bind_text( m_stm, (int)idx, m_data.back().c_str(), m_data.back().size(), SQLITE_STATIC);
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

const std::string SQLiteStatement::replace( const unsigned int idx ) const
{
	return "$" + boost::lexical_cast< std::string >( idx );
}
