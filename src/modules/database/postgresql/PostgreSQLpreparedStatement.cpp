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
///\brief Implementation of processing prepared statements with postgres client using libpq
///\file modules/database/postgres/PostgreSQLpreparedStatement.cpp

#include "PostgreSQLpreparedStatement.hpp"
#include "PostgreSQL.hpp"
#include "logger-v1.hpp"
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>

using namespace _Wolframe;
using namespace _Wolframe::db;

PreparedStatementHandler_postgres::PreparedStatementHandler_postgres( PGconn* conn_, const types::keymap<std::string>* stmmap_, bool inTransactionContext)
	:m_state(inTransactionContext?Transaction:Init)
	,m_conn(conn_)
	,m_stmmap(stmmap_)
	,m_lastresult(0)
	,m_nof_rows(0)
	,m_idx_row(0){}

PreparedStatementHandler_postgres::~PreparedStatementHandler_postgres()
{
	clear();
}

void PreparedStatementHandler_postgres::clear()
{
	if (m_lastresult)
	{
		PQclear( m_lastresult);
		m_lastresult = 0;
	}
	m_lasterror.reset();
	m_statement.clear();
	m_state = Init;
	m_nof_rows = 0;
	m_idx_row = 0;
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

void PreparedStatementHandler_postgres::setDatabaseErrorMessage()
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
	m_lasterror.reset( new DatabaseError( severity, errorcode, PQdb(m_conn), m_statement.string().c_str(), errtype, errmsg, usermsg));
}

bool PreparedStatementHandler_postgres::status( PGresult* res, State newstate)
{
	bool rt;
	ExecStatusType es = PQresultStatus( res);

	if (es != PGRES_COMMAND_OK && es != PGRES_TUPLES_OK)
	{
		setDatabaseErrorMessage();
		m_state = Error;
		rt = false;
	}
	else
	{
		m_state = newstate;
		rt = true;
	}
	if (res != m_lastresult)
	{
		PQclear( res);
	}
	return rt;
}

bool PreparedStatementHandler_postgres::begin()
{
	LOG_TRACE << "[postgresql statement] CALL begin()";
	if (m_state != Init)
	{
		return errorStatus( std::string( "call of begin not allowed in state '") + stateName(m_state) + "'");
	}
	return status( PQexec( m_conn, "BEGIN;"), Transaction);
}

bool PreparedStatementHandler_postgres::commit()
{
	LOG_TRACE << "[postgresql statement] CALL commit()";
	if (m_state == Transaction)
	{
		LOG_WARNING << "executed transaction is empty";
	}
	else if (m_state != Executed && m_state != Prepared)
	{
		return errorStatus( std::string( "call of commit not allowed in state '") + stateName(m_state) + "'");
	}
	return status( PQexec( m_conn, "COMMIT;"), Init);
}

bool PreparedStatementHandler_postgres::rollback()
{
	LOG_TRACE << "[postgresql statement] CALL rollback()";
	return status( PQexec( m_conn, "ROLLBACK;"), Init);
}

bool PreparedStatementHandler_postgres::errorStatus( const std::string& message)
{
	if (m_state != Error)
	{
		m_lasterror.reset( new DatabaseError( log::LogLevel::LOGLEVEL_ERROR, 0, PQdb(m_conn), m_statement.string().c_str(), "INTERNAL", message.c_str(), "internal logic error"));
		m_state = Error;
	}
	return false;
}

bool PreparedStatementHandler_postgres::start( const std::string& stmname)
{
	LOG_TRACE << "[postgresql statement] CALL start (" << stmname << ")";
	if (m_state == Executed || m_state == Prepared)
	{
		m_state = Transaction;
	}
	if (m_state != Transaction)
	{
		return errorStatus( std::string( "call of start not allowed in state '") + stateName(m_state) + "'");
	}
	clear();
	types::keymap<std::string>::const_iterator si = m_stmmap->find( stmname);
	if (si == m_stmmap->end())
	{
		throw std::runtime_error( std::string( "statement '") + stmname + "' is not defined");
	}
	m_statement.init( si->second);
	m_state = Prepared;
	return true;
}

bool PreparedStatementHandler_postgres::bind( std::size_t idx, const types::VariantConst& value)
{
	if (value.defined())
	{
		LOG_TRACE << "[postgresql statement] CALL bind( " << idx << ", '" << value << "' )";
	}
	else
	{
		LOG_TRACE << "[postgresql statement] CALL bind( " << idx << ", NULL)";
	}
	if (m_state != Prepared && m_state != Executed)
	{
		return errorStatus( std::string( "call of bind not allowed in state '") + stateName(m_state) + "'");
	}
	if (idx == 0 || idx > m_statement.maxparam())
	{
		errorStatus( std::string( "index of parameter out of range (required to be in range 1..") + boost::lexical_cast<std::string>(m_statement.maxparam()) + " in statement '" + m_statement.string() + "'");
		return false;
	}
	if (idx == 0 || idx > m_statement.maxparam())
	{
		errorStatus( std::string( "index of bind parameter out of range (required to be in range 1..") + boost::lexical_cast<std::string>(m_statement.maxparam()) + " in statement '" + m_statement.string() + "'");
		return false;
	}
	if (value.defined())
	{
		std::string strval = value.tostring();
		char* encvalue = (char*)std::malloc( strval.size() * 2 + 3);
		encvalue[0] = '\'';
		boost::shared_ptr<void> encvaluer( encvalue, std::free);
		int error = 0;
		size_t encvaluesize = PQescapeStringConn( m_conn, encvalue+1, strval.c_str(), strval.size(), &error);
		encvalue[encvaluesize+1] = '\'';
		std::string bindval( encvalue, encvaluesize+2);
		m_statement.bind( idx, bindval);
	}
	else
	{
		m_statement.bind( idx, "NULL");
	}
	m_state = Prepared;
	return true;
}

bool PreparedStatementHandler_postgres::execute()
{
	if (m_state != Prepared)
	{
		return errorStatus( std::string( "call of execute not allowed in state '") + stateName(m_state) + "'");
	}
	if (m_lastresult)
	{
		PQclear( m_lastresult);
		m_lastresult = 0;
	}
	std::string stmstr = m_statement.expanded();
	LOG_TRACE << "[postgresql statement] CALL execute(" << stmstr << ")";
	m_lastresult = PQexec( m_conn, stmstr.c_str());

	bool rt = status( m_lastresult, Executed);
	if (rt)
	{
		m_nof_rows = (std::size_t)PQntuples( m_lastresult);
		m_idx_row = 0;
	}
	return rt;
}

bool PreparedStatementHandler_postgres::hasResult()
{
	return (m_nof_rows > 0);
}

std::size_t PreparedStatementHandler_postgres::nofColumns()
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

const char* PreparedStatementHandler_postgres::columnName( std::size_t idx)
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

const DatabaseError* PreparedStatementHandler_postgres::getLastError()
{
	return m_lasterror.get();
}

types::VariantConst PreparedStatementHandler_postgres::get( std::size_t idx)
{
	LOG_TRACE << "[postgresql statement] CALL get(" << idx << ")";
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
	char* rt = PQgetvalue( m_lastresult, (int)m_idx_row, (int)idx-1);
	if (!rt || rt[0] == '\0')
	{
		if (PQgetisnull( m_lastresult, (int)m_idx_row, (int)idx-1))
		{
			return types::VariantConst();
		}
	}
	return types::VariantConst( rt);
}

bool PreparedStatementHandler_postgres::next()
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


