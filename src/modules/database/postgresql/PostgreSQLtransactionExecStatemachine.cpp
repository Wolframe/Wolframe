/************************************************************************

 Copyright (C) 2011 - 2013 Project Wolframe.
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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
///\brief Implementation of the standard database transaction execution statemechine with postgres client using libpq
///\file PostgreSQLtransactionExecStatemachine.cpp

#include "PostgreSQLtransactionExecStatemachine.hpp"
#include "PostgreSQL.hpp"
#undef SUBSTITUTE_STATEMENT
#ifdef SUBSTITUTE_STATEMENT
#include "PostgreSQLsubstitutingStatement.hpp"
#define STATEMENT_CLASS PostgreSQLsubstitutingStatement
#else
#include "PostgreSQLstatement.hpp"
#define STATEMENT_CLASS PostgreSQLstatement
#endif
#include "logger-v1.hpp"
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <boost/lexical_cast.hpp>

using namespace _Wolframe;
using namespace _Wolframe::db;

TransactionExecStatemachine_postgres::TransactionExecStatemachine_postgres( const std::string& name_, PostgreSQLdbUnit* dbunit_)
	:TransactionExecStatemachine(name_)
	,m_state(Init)
	,m_lastresult(0)
	,m_statement( new STATEMENT_CLASS( ) )
	,m_nof_rows(0)
	,m_idx_row(0)
	,m_hasResult(false)
	,m_dbunit(dbunit_)
	,m_conn(0)
	{}

TransactionExecStatemachine_postgres::~TransactionExecStatemachine_postgres()
{
	if (m_conn) delete m_conn;
	clear();
	delete m_statement;
}

void TransactionExecStatemachine_postgres::clear()
{
	if (m_lastresult)
	{
		PQclear( m_lastresult);
		m_lastresult = 0;
	}
	m_lasterror.reset();
	m_statement->clear();
	m_state = Init;
	m_nof_rows = 0;
	m_idx_row = 0;
	m_hasResult = false;
}

static const char* getErrorType( const char* tp)
{
	if (!tp) return 0;
	if (strcmp (tp,"01007") == 0 || strcmp (tp,"01008") == 0)
	{
		return "PRIVILEGE";
	}
	if (memcmp( tp, "0L", 2) == 0 || memcmp( tp, "0P", 2) == 0 || memcmp( tp, "28", 2) == 0)
	{
		return "PRIVILEGE";
	}
	if (memcmp( tp, "03", 2) == 0)
	{
		return "STATEMENT";
	}
	if (memcmp( tp, "08", 2) == 0)
	{
		return "CONNECTION";
	}
	if (memcmp( tp, "09", 2) == 0 || memcmp( tp, "2F", 2) == 0 || memcmp( tp, "38", 2) == 0 || memcmp( tp, "39", 2) == 0 || memcmp( tp, "3B", 2) == 0)
	{
		return "EXCEPTION";
	}
	if (memcmp( tp, "21", 2) == 0)
	{
		return "PARAMETER";
	}
	if (memcmp( tp, "22", 2) == 0)
	{
		return "PARAMETER";
	}
	if (memcmp( tp, "23", 2) == 0)
	{
		return "CONSTRAINT";
	}
	if (memcmp( tp, "42", 2) == 0)
	{
		return "SYNTAX";
	}
	if (memcmp( tp, "53", 2) == 0)
	{
		return "RESOURCE";
	}
	if (memcmp( tp, "5", 1) == 0)
	{
		return "SYSTEM";
	}
	if (memcmp( tp, "F0", 2) == 0)
	{
		return "CONFIGURATION";
	}
	if (memcmp( tp, "P0", 2) == 0)
	{
		return "PLSQL";
	}
	if (memcmp( tp, "XX", 2) == 0)
	{
		return "INTERNAL";
	}
	return 0;
}

void TransactionExecStatemachine_postgres::setDatabaseErrorMessage()
{
	const char* errmsg = m_lastresult?PQresultErrorMessage( m_lastresult):"";
	const char* errtype = m_lastresult?getErrorType( PQresultErrorField( m_lastresult, PG_DIAG_SQLSTATE)):"INTERNAL";
	const char* severitystr = m_lastresult?PQresultErrorField( m_lastresult, PG_DIAG_SEVERITY):"ERROR";
	log::LogLevel::Level severity = PostgreSQLdbUnit::getLogLevel( severitystr);

	const char* usermsg = 0;
	if (errmsg)
	{
		usermsg = strstr( errmsg, "DETAIL:");
		if (usermsg) usermsg = usermsg + 7;
		if (!usermsg)
		{
			usermsg = strstr( errmsg, "ERROR:");
			if (usermsg) usermsg = usermsg + 7;
		}
		if (!usermsg)
		{
			usermsg = errmsg;
		}
	}
	int errorcode = 0;
	m_lasterror.reset( new DatabaseError( severity, errorcode, PQdb(**m_conn), m_statement->nativeSQL().c_str(), errtype, errmsg, usermsg));
}

bool TransactionExecStatemachine_postgres::status( PGresult* res, State newstate)
{
	bool rt;
	ExecStatusType es = PQresultStatus( res);

	if (es == PGRES_COMMAND_OK)
	{
		m_hasResult = false;
		m_state = newstate;
		rt = true;
	}
	else if (es == PGRES_TUPLES_OK)
	{
		m_hasResult = true;
		m_state = newstate;
		rt = true;
	}
	else
	{
		setDatabaseErrorMessage();
		m_state = Error;
		rt = false;
	}
	if (res != m_lastresult)
	{
		PQclear( res);
	}
	return rt;
}

bool TransactionExecStatemachine_postgres::begin()
{
	LOG_TRACE << "[postgresql statement] CALL begin()";
	if (m_state != Init)
	{
		return errorStatus( std::string( "call of begin not allowed in state '") + stateName(m_state) + "'");
	}
	if (m_conn) delete m_conn;
	m_conn = m_dbunit->newConnection();
	static_cast<STATEMENT_CLASS *>( m_statement )->setConnection( **m_conn );
	return status( PQexec( **m_conn, "BEGIN;"), Transaction);
}

bool TransactionExecStatemachine_postgres::commit()
{
	LOG_TRACE << "[postgresql statement] CALL commit()";
	if (m_state == Transaction)
	{
		LOG_WARNING << "executed transaction is empty";
	}
	else if (m_state != Executed && m_state != CommandReady)
	{
		return errorStatus( std::string( "call of commit not allowed in state '") + stateName(m_state) + "'");
	}
	bool rt = status( PQexec( **m_conn, "COMMIT;"), Init);
	if (rt)
	{
		delete m_conn;
		m_conn = 0;
	}
	return rt;
}

bool TransactionExecStatemachine_postgres::rollback()
{
	LOG_TRACE << "[postgresql statement] CALL rollback()";
	if (m_conn)
	{
		bool rt = status( PQexec( **m_conn, "ROLLBACK;"), Init);
		if (rt)
		{
			delete m_conn;
			m_conn = 0;
		}
		return rt;
	}
	return true;
}

bool TransactionExecStatemachine_postgres::errorStatus( const std::string& message)
{
	if (m_state != Error)
	{
		const char* dbname = m_conn?PQdb(**m_conn):"POSTGRESQL";
		m_lasterror.reset( new DatabaseError( log::LogLevel::LOGLEVEL_ERROR, 0, dbname, m_statement->nativeSQL().c_str(), "INTERNAL", message.c_str(), "internal logic error"));
		m_state = Error;
	}
	return false;
}

bool TransactionExecStatemachine_postgres::start( const std::string& statement)
{
	LOG_TRACE << "[postgresql statement] CALL start (" << statement << ")";
	if (m_state == Executed || m_state == CommandReady)
	{
		m_state = Transaction;
	}
	if (m_state != Transaction)
	{
		return errorStatus( std::string( "call of start not allowed in state '") + stateName(m_state) + "'");
	}
	clear();
	m_statement->init( statement);
	m_state = CommandReady;
	return true;
}

bool TransactionExecStatemachine_postgres::bind( std::size_t idx, const types::VariantConst& value)
{
	if (value.defined())
	{
		LOG_TRACE << "[postgresql statement] CALL bind( " << idx << ", '" << value << "', " << value.typeName( ) << " )";
	}
	else
	{
		LOG_TRACE << "[postgresql statement] CALL bind( " << idx << ", NULL)";
	}
	if (m_state != CommandReady && m_state != Executed)
	{
		return errorStatus( std::string( "call of bind not allowed in state '") + stateName(m_state) + "'");
	}

	try {
		m_statement->bind( idx, value );
	} catch( const std::runtime_error &e ) {
		return errorStatus( e.what( ) );
	}
	
	m_state = CommandReady;
	return true;
}

bool TransactionExecStatemachine_postgres::execute()
{
	if (m_state != CommandReady)
	{
		return errorStatus( std::string( "call of execute not allowed in state '") + stateName(m_state) + "'");
	}
	if (m_lastresult)
	{
		PQclear( m_lastresult);
		m_lastresult = 0;
	}
	// TODO: or another string which also works when PQexec is used?
	m_statement->substitute( );
	std::string stmstr = m_statement->nativeSQL();
	LOG_TRACE << "[postgresql statement] CALL execute(" << stmstr << ")";
	m_lastresult = static_cast<STATEMENT_CLASS *>( m_statement )->execute( );

	bool rt = status( m_lastresult, Executed);
	if (rt)
	{
		if (m_hasResult)
		{
			m_nof_rows = (std::size_t)PQntuples( m_lastresult);
			m_idx_row = 0;
		}
		else
		{
			m_nof_rows = 0;
			m_idx_row = 0;
		}
	}
	return rt;
}

bool TransactionExecStatemachine_postgres::hasResult()
{
	return m_hasResult && m_nof_rows > 0;
}

std::size_t TransactionExecStatemachine_postgres::nofColumns()
{
	if (m_state != Executed)
	{
		return errorStatus( std::string( "inspect command result not possible in state '") + stateName(m_state) + "'");
	}
	if (!m_lastresult)
	{
		return errorStatus( "command result is empty");
	}
	return PQnfields( m_lastresult);
}

const char* TransactionExecStatemachine_postgres::columnName( std::size_t idx)
{
	if (m_state != Executed)
	{
		errorStatus( std::string( "inspect command result not possible in state '") + stateName(m_state) + "'");
		return 0;
	}
	if (!m_lastresult)
	{
		errorStatus( "command result is empty");
		return 0;
	}
	const char* rt = PQfname( m_lastresult, (int)idx-1);
	if (!rt) errorStatus( std::string( "index of column out of range (") + boost::lexical_cast<std::string>(idx) + ")");
	return rt;
}

const DatabaseError* TransactionExecStatemachine_postgres::getLastError()
{
	return m_lasterror.get();
}

// OIDs from the PostgreSQL system catalog pg_type. For system types
// those constants are fix, so we hard-code them here. For user-defined
// types and special conversions (even variant plugins) we need a differnt
// mechanism..
enum PostgreSQLfieldTypes
{
	PGSQL_FIELD_TYPE_BOOLEAN	= 16,	// boolean
	PGSQL_FIELD_TYPE_INT8		= 20,	// int8
	PGSQL_FIELD_TYPE_INT4		= 23,	// int4
	PGSQL_FIELD_TYPE_TEXT		= 25,	// text
	PGSQL_FIELD_TYPE_REAL		= 700	// real
};

types::VariantConst TransactionExecStatemachine_postgres::get( std::size_t idx)
{
	if (m_state != Executed)
	{
		errorStatus( std::string( "inspect command result not possible in state '") + stateName(m_state) + "'");
		return types::VariantConst();
	}
	if (!m_lastresult)
	{
		errorStatus( "command result is empty");
		return types::VariantConst();
	}
	if (m_idx_row >= m_nof_rows) return types::VariantConst();
	
	// no matter the type, NULL is indicated with PQgetisnull, in this case
	// resval[0] is 0 (empty string).
	// TODO: handle binary data for blobs and user-defined types, we
	// cannot simply force it to a string! In this case there should be
	// an extension mechanism for the variant type..?
	char* resval = PQgetvalue( m_lastresult, (int)m_idx_row, (int)idx-1);
	if( PQgetisnull( m_lastresult, (int)m_idx_row, (int)idx-1 ) )
	{
		LOG_DATA << "[postgresql statement] CALL get(" << idx << ") => NULL";
		return types::VariantConst();
	}

	// depending on the system type we try to keep as much type information
	// as possible in the next upper layer
	Oid type = PQftype( m_lastresult, (int)idx-1);
	types::VariantConst rt;
	switch( type )
	{
		case PGSQL_FIELD_TYPE_BOOLEAN:
			if( strcmp( resval, "t" ) == 0 ) {
				rt = types::VariantConst( true );
			} else if( strcmp( resval, "f" ) == 0 ) {
				rt = types::VariantConst( false );
			} else {
				// tertium non datur
				errorStatus( std::string( "unexpected value '" ) + resval + "' for boolean type" );
				rt = types::VariantConst();
			}
			break;
				
		case PGSQL_FIELD_TYPE_INT4:
		case PGSQL_FIELD_TYPE_INT8:
			rt = types::VariantConst( resval );
			rt.convert( types::Variant::Int );
			//~ rt = types::VariantConst( rt.toint( ) );
			break;
		
		case PGSQL_FIELD_TYPE_REAL:
			rt = types::VariantConst( resval );
			rt = types::VariantConst( rt.todouble( ) );
			break;
			
		case PGSQL_FIELD_TYPE_TEXT:
			rt = types::VariantConst( resval );
			break;
			
		default:
			LOG_DATA << "[postgresql statement] unknown Postgresql type '" << type << "' in column " << idx << ", assuming string";
			rt = types::VariantConst( resval );
	}
		
	LOG_DATA << "[postgresql statement] CALL get(" << idx << ") => " << rt.typeName() << " '" << rt.tostring() << "'";
	return rt;
}

bool TransactionExecStatemachine_postgres::next()
{
	LOG_TRACE << "[postgresql statement] CALL next()";
	if (m_state != Executed)
	{
		errorStatus( std::string( "get next command result not possible in state '") + stateName(m_state) + "'");
		return false;
	}
	if (m_idx_row < m_nof_rows)
	{
		++m_idx_row;
		return (m_idx_row < m_nof_rows);
	}
	return false;
}

const std::string& TransactionExecStatemachine_postgres::databaseID() const
{
	return m_dbunit->ID();
}



