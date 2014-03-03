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
			/*[PF:TODO] Implementation*/
			m_data.push_back( value.tostring());
			m_rc = wrap_sqlite3_bind_text( m_stm, (int)idx, m_data.back().c_str(), m_data.back().size(), SQLITE_STATIC);
		}
		case types::Variant::BigNumber:
		{
			/*[PF:TODO] Implementation*/
			m_data.push_back( value.tostring());
			m_rc = wrap_sqlite3_bind_text( m_stm, (int)idx, m_data.back().c_str(), m_data.back().size(), SQLITE_STATIC);
		}
		case types::Variant::Custom:
		{
			types::Variant baseval;
			try
			{
				if (value.customref()->getBaseTypeValue( baseval)
				&&  baseval.type() != types::Variant::Custom)
				{
					bind( idx, baseval);
					break;
				}
			}
			catch (const std::runtime_error& e)
			{
				throw std::runtime_error( std::string("cannot convert value to base type for binding: ") + e.what());
			}
			bind( idx, value.tostring());
		}
		default:
			throw std::logic_error( "Binding unknown type '" + std::string( value.typeName( ) ) + "'" );
	}
}

const std::string SQLiteStatement::replace( const unsigned int idx ) const
{
	return "$" + boost::lexical_cast< std::string >( idx );
}
