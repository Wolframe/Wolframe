#include "OracleStatement.hpp"

#include <boost/lexical_cast.hpp>

using namespace _Wolframe;
using namespace _Wolframe::db;

OracleStatement::OracleStatement( )
	: BaseStatement( ),
	m_env( 0 )
{
}

OracleStatement::OracleStatement( const OracleStatement &o )
	: BaseStatement( o ),
	m_env( o.m_env )
{
}

OracleStatement::OracleStatement( OracleEnvirenment *env )
	: m_env( env )
{
}

void OracleStatement::bind( const unsigned int idx, const types::Variant &arg )
{
	// does boundary checking
	BaseStatement::bind( idx, arg );

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
	return "$" + boost::lexical_cast< std::string >( idx );
}

OCIStmt *OracleStatement::execute( ) const
{
}

