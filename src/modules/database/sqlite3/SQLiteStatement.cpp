#include "SQLiteStatement.hpp"

#include <boost/lexical_cast.hpp>

using namespace _Wolframe;
using namespace _Wolframe::db;

SQLiteStatement::SQLiteStatement( )
	: BaseStatement( )
{
}

SQLiteStatement::SQLiteStatement( const SQLiteStatement &o )
	: BaseStatement( o )
{
}

void SQLiteStatement::bind( const unsigned int idx, const types::Variant &arg )
{
	// does boundary checking
	BaseStatement::bind( idx, arg );
}

const std::string SQLiteStatement::replace( const unsigned int idx ) const
{
	return "$" + boost::lexical_cast< std::string >( idx );
}

sqlite3_stmt *SQLiteStatement::execute( ) const
{
}

