#include "OracleStatement.hpp"
#include "types/variant.hpp"
#include "types/datetime.hpp"
#include "types/bignumber.hpp"
#include "types/customDataType.hpp"

#include <boost/lexical_cast.hpp>

using namespace _Wolframe;
using namespace _Wolframe::db;

OracleStatement::OracleStatement( )
	: BaseStatement( ),
	m_env( 0 ),
	m_conn( 0 ),
	m_stmt( 0 ),
	m_status( 0 )
{
	m_data.reserve( MaxNofParams );
}

OracleStatement::OracleStatement( const OracleStatement &o )
	: BaseStatement( o ),
	m_env( o.m_env ),
	m_conn( o.m_conn ),
	m_stmt( o.m_stmt ),
	m_status( o.m_status )
{
}

OracleStatement::OracleStatement( OracleEnvirenment *env )
	: m_env( env ),
	m_conn( 0 ),
	m_stmt( 0 ),
	m_status( 0 )
{
	m_data.reserve( MaxNofParams );
}

void OracleStatement::setConnection( OracleConnection *conn )
{
	m_conn = conn;
}

void OracleStatement::setStatement( OCIStmt *stmt )
{
	m_stmt = stmt;
}

sword OracleStatement::getLastStatus( )
{
	return m_status;
}

void OracleStatement::bindUInt64( const unsigned int idx, oraub8 &value )
{
	OCIBind *bindhp = (OCIBind *)0;

	m_status = OCIBindByPos( m_stmt, &bindhp, m_conn->errhp,
		(ub4)idx, (dvoid *)&value, (sb4)sizeof( oraub8 ),
		SQLT_UIN, (dvoid *)0, (ub2 *)0, (ub2 *)0,
		(ub4)0, (ub4 *)0, OCI_DEFAULT );
}

void OracleStatement::bindInt64( const unsigned int idx, orasb8 &value )
{
	OCIBind *bindhp = (OCIBind *)0;

	m_status = OCIBindByPos( m_stmt, &bindhp, m_conn->errhp,
		(ub4)idx, (dvoid *)&value, (sb4)sizeof( orasb8 ),
		SQLT_INT, (dvoid *)0, (ub2 *)0, (ub2 *)0,
		(ub4)0, (ub4 *)0, OCI_DEFAULT );
}

void OracleStatement::bindUInt( const unsigned int idx, unsigned int &value )
{
	OCIBind *bindhp = (OCIBind *)0;

	m_status = OCIBindByPos( m_stmt, &bindhp, m_conn->errhp,
		(ub4)idx, (dvoid *)&value, (sb4)sizeof( unsigned int ),
		SQLT_UIN, (dvoid *)0, (ub2 *)0, (ub2 *)0,
		(ub4)0, (ub4 *)0, OCI_DEFAULT );
}

void OracleStatement::bindInt( const unsigned int idx, signed int &value )
{
	OCIBind *bindhp = (OCIBind *)0;

	m_status = OCIBindByPos( m_stmt, &bindhp, m_conn->errhp,
		(ub4)idx, (dvoid *)&value, (sb4)sizeof( signed int ),
		SQLT_INT, (dvoid *)0, (ub2 *)0, (ub2 *)0,
		(ub4)0, (ub4 *)0, OCI_DEFAULT );
}

// TODO: hard-wired to a NUMBER(1) with 0 and 1 for now	
void OracleStatement::bindBool( const unsigned int idx, signed int &value )
{
	OCIBind *bindhp = (OCIBind *)0;

	m_status = OCIBindByPos( m_stmt, &bindhp, m_conn->errhp,
		(ub4)idx, (dvoid *)&value, (sb4)sizeof( signed int ),
		SQLT_INT, (dvoid *)0, (ub2 *)0, (ub2 *)0,
		(ub4)0, (ub4 *)0, OCI_DEFAULT );
}

void OracleStatement::bindDouble( const unsigned int idx, double &value )
{
	OCIBind *bindhp = (OCIBind *)0;
	
	m_status = OCIBindByPos( m_stmt, &bindhp, m_conn->errhp,
		(ub4)idx, (dvoid *)&value, (sb4)sizeof( double ),
		SQLT_BDOUBLE, (dvoid *)0, (ub2 *)0, (ub2 *)0,
		(ub4)0, (ub4 *)0, OCI_DEFAULT );
}

void OracleStatement::bindNumber( const unsigned int idx, const _WOLFRAME_INTEGER &value )
{
	throw new std::logic_error( "bindNumber with _WOLFRAME_INTEGER not implemented for value '"
		+ boost::lexical_cast<std::string>( value ) + "' for index "
		+ boost::lexical_cast<std::string>( idx ) );
}

void OracleStatement::bindNumber( const unsigned int idx, const _WOLFRAME_UINTEGER &value )
{
	throw new std::logic_error( "bindNumber with _WOLFRAME_UINTEGER not implemented for value '"
		+ boost::lexical_cast<std::string>( value ) + "' for index "
		+ boost::lexical_cast<std::string>( idx ) );
}

void OracleStatement::bindString( const unsigned int idx, char* value, const std::size_t size )
{
	OCIBind *bindhp = (OCIBind *)0;

	m_status = OCIBindByPos( m_stmt, &bindhp, m_conn->errhp,
		(ub4)idx, (dvoid *)value, (sb4)size,
		SQLT_CHR, (dvoid *)0, (ub2 *)0, (ub2 *)0,
		(ub4)0, (ub4 *)0, OCI_DEFAULT );
}

void OracleStatement::bindNull( const unsigned int idx )
{
	OCIBind *bindhp = (OCIBind *)0;

	m_status = OCIBindByPos( m_stmt, &bindhp, m_conn->errhp,
		(ub4)idx, (dvoid *)0, (sb4)0,
		SQLT_CHR, (dvoid *)0, (ub2 *)0, (ub2 *)0,
		(ub4)0, (ub4 *)0, OCI_DEFAULT );
}

void OracleStatement::clear( )
{
	BaseStatement::clear( );
	m_data.clear( );
}

void OracleStatement::bind( const unsigned int idx, const types::Variant &value )
{
	// does boundary checking
	BaseStatement::bind( idx, value );

	// remember value
	OracleData data;
	data.v = value;
	m_data.push_back( data );
	
	// bind it
	switch( value.type( ) ) {
		case types::Variant::Null:
			bindNull( idx );
			break;
		
		case types::Variant::Int:
			if( value.data( ).value.Int <= std::numeric_limits<signed int>::max( ) && value.data( ).value.Int >= std::numeric_limits<signed int>::min( ) ) {
				m_data.back( ).i = (signed int)m_data.back( ).v.data( ).value.Int;
				bindInt( idx, m_data.back( ).i );
#if OCI_MAJOR_VERSION >= 12 || ( OCI_MAJOR_VERSION == 11 && OCI_MAJOR_VERSION >= 2 )
			} else if( value.data( ).value.Int <= std::numeric_limits<orasb8>::max( ) ) {
				m_data.back( ).i64 = m_data.back( ).v.data( ).value.Int;
				bindInt64( idx, m_data.back( ).i64 );
#else				
			} else {
				bindNumber( idx, m_data.back( ).v.data( ).value.Int );
#endif
			}
			break;

		case types::Variant::UInt:
			if( value.data( ).value.Int <= std::numeric_limits<signed int>::max( ) ) {
				m_data.back( ).ui = (unsigned int)m_data.back( ).v.data( ).value.UInt;
				bindUInt( idx, m_data.back( ).ui );
#if OCI_MAJOR_VERSION >= 12 || ( OCI_MAJOR_VERSION == 11 && OCI_MAJOR_VERSION >= 2 )
			} else if( value.data( ).value.Int <= std::numeric_limits<orasb8>::max( ) ) {
				m_data.back( ).ui64 = m_data.back( ).v.data( ).value.UInt;
				bindUInt64( idx, m_data.back( ).ui64 );
#else				
			} else {
				bindNumber( idx, m_data.back( ).v.data( ).value.UInt );
#endif
			}
			break;

		case types::Variant::Double:
			m_data.back( ).d = m_data.back( ).v.data().value.Double;
			bindDouble( idx, m_data.back( ).d );
			break;

		case types::Variant::Bool:
			m_data.back( ).i = m_data.back( ).v.toint( );
			bindBool( idx, m_data.back( ).i );
			break;

		case types::Variant::String:
			m_data.back( ).s = (char *)malloc( m_data.back( ).v.charsize( ) + 1 );
			memcpy( m_data.back( ).s, m_data.back( ).v.charptr( ), m_data.back( ).v.charsize( ) );
			bindString( idx, m_data.back( ).s, m_data.back( ).v.charsize( ) );
			break;

		case types::Variant::Timestamp:
		{
			/*[PF:TODO] Implementation*/
			m_data.back().v.convert( types::Variant::String);
			m_data.back( ).s = (char *)malloc( m_data.back( ).v.charsize( ) + 1 );
			memcpy( m_data.back( ).s, m_data.back( ).v.charptr( ), m_data.back( ).v.charsize( ) );
			bindString( idx, m_data.back( ).s, m_data.back( ).v.charsize( ) );
			break;
		}
		case types::Variant::BigNumber:
		{
			/*[PF:TODO] Implementation*/
			m_data.back().v.convert( types::Variant::String);
			m_data.back( ).s = (char *)malloc( m_data.back( ).v.charsize( ) + 1 );
			memcpy( m_data.back( ).s, m_data.back( ).v.charptr( ), m_data.back( ).v.charsize( ) );
			bindString( idx, m_data.back( ).s, m_data.back( ).v.charsize( ) );
			break;
		}
		case types::Variant::Custom:
		{
			types::Variant baseval;
			try
			{
				value.customref()->getBaseTypeValue( baseval);
				if (baseval.type() != types::Variant::Custom)
				{
					bind( idx, baseval);
					break;
				}
			}
			catch (const std::runtime_error& e)
			{
				throw std::runtime_error( std::string("cannot convert value to base type for binding: ") + e.what());
			}
			throw std::runtime_error( "cannot convert value to base type for binding");
		}
		default:
			throw std::logic_error( "Binding unknown type '" + std::string( value.typeName( ) ) + "'" );
	}
}

OracleStatement::~OracleStatement( )
{
	std::vector<OracleData>::iterator it, end = m_data.end( );
	for( it = m_data.begin( ); it != end; it++ ) {
		if( (*it).s ) free( (*it).s );
	}
}

const std::string OracleStatement::replace( const unsigned int idx ) const
{
	return ":" + boost::lexical_cast< std::string >( idx );
}

