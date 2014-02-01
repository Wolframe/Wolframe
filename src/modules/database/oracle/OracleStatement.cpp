#include "OracleStatement.hpp"

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

void OracleStatement::bindInt( const unsigned int idx, _WOLFRAME_INTEGER value )
{
}

void OracleStatement::bindUInt( const unsigned int idx, _WOLFRAME_UINTEGER value )
{
	OCIBind *bindh = 0;

	//~ m_status = OCIDefineByPos( m_stmt, &bindh, m_conn->errhp,
	//~ 32bit, otherwise use number!
		//~ (ub4)idx, (dvoid)&value, sizeof( _WOLFRAME_UINTEGER ),


//~ status_ = OCIDefineByPos( m_lastresult, &descrRef->defhp,
	//~ (*m_conn)->errhp, counter, (dvoid *)descrRef->buf,
	//~ (sb4)descrRef->bufsize, descrRef->fetchType,
	//~ &descrRef->ind, &descrRef->len, &descrRef->errcode, OCI_DEFAULT );

 //~ if (status = OCIBindByPos(stmthp1, &bnd6p, errhp, 1,
           //~ (dvoid *) &deptno, (sword) sizeof(deptno),SQLT_INT,
           //~ (dvoid *) 0, (ub2 *) 0, (ub2 *) 0, (ub4) 0, (ub4 *) 0,OCI_DEFAULT))

//~ OCIBindByPos(stmt->stmt, (OCIBind **) &bnd->buf.handle,
	     //~ stmt->con->err, (ub4) index, (void *) bnd->buf.data,
	     //~ bnd->size, bnd->code, bnd->buf.inds, (ub2 *) bnd->buf.lens,
	     //~ bnd->plrcds, (ub4) (is_pltbl == TRUE ? nbelem : 0),
	     //~ pnbelem, exec_mode)
}

void OracleStatement::bindBool( const unsigned int idx, bool value )
{
}

void OracleStatement::bindDouble( const unsigned int idx, double value )
{
}

void OracleStatement::bindString( const unsigned int idx, const char* value, std::size_t size )
{
}

void OracleStatement::bindNull( const unsigned int idx )
{
}

void OracleStatement::bind( const unsigned int idx, const types::Variant &value )
{
	// does boundary checking
	BaseStatement::bind( idx, value );

	switch( value.type( ) ) {
		case types::Variant::Null:
			bindNull( idx );
			break;
		
		case types::Variant::Int:
			bindInt( idx, value.data( ).value.Int );
			break;

		case types::Variant::UInt:
			bindUInt( idx, value.data( ).value.UInt );
			break;
		
		case types::Variant::Double:
			bindDouble( idx, value.data().value.Double );
			break;

		case types::Variant::Bool:
			bindBool( idx, value.data().value.Bool );
			break;

		case types::Variant::String:
			bindString( idx, value.charptr( ), value.charsize( ) );
			break;

		case types::Variant::Custom:
			throw std::logic_error( "Custom type in PostgreSQL database module not supported yet!");
			break;
		
		default:
			throw std::logic_error( "Binding unknown type '" + std::string( value.typeName( ) ) + "'" );
	}

	//~ if (value.defined())
	//~ {
		//~ // TODO: replace this with OracleStatement.. use type binding..
		//~ if( value.type( ) == types::Variant::Bool ) {
			//~ // TODO: hard-wired to a NUMBER(1) with 0 and 1 for now
			//~ if( value.tobool( ) ) {
				//~ m_statement.bind( idx, "1" );
			//~ } else {
				//~ m_statement.bind( idx, "0" );
			//~ }
		//~ } else {
			//~ m_statement.bind( idx, "'" + boost::replace_all_copy( value.tostring( ), "'", "''" ) + "'" );
		//~ }
	//~ }
	//~ else
	//~ {
		//~ m_statement.bind( idx, "NULL");
	//~ }
}

const std::string OracleStatement::replace( const unsigned int idx ) const
{
	return ":" + boost::lexical_cast< std::string >( idx );
}

