#include "OracleStatement.hpp"

#include <boost/lexical_cast.hpp>

using namespace _Wolframe;
using namespace _Wolframe::db;

OracleStatement::OracleStatement( )
	: m_env( 0 )
{
}

OracleStatement::OracleStatement( const OracleStatement &o )
	: m_env( 0 )
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
}

const std::string OracleStatement::replace( const unsigned int idx ) const
{
	return "$" + boost::lexical_cast< std::string >( idx );
}

OCIStmt *OracleStatement::execute( ) const
{
}

