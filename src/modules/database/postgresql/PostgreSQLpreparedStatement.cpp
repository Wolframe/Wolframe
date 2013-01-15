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
#include "logger-v1.hpp"
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>
#undef LOWLEVEL_DEBUG

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
	m_lasterror.clear();
	m_statement.clear();
	m_state = Init;
	m_nof_rows = 0;
	m_idx_row = 0;
}

void PreparedStatementHandler_postgres::setDatabaseErrorMessage()
{
	const char* statusType = m_lastresult?PQresStatus( PQresultStatus( m_lastresult)):"unknown";
	const char* errmsg = m_lastresult?PQresultErrorMessage( m_lastresult):"";

	std::ostringstream msg;
	msg << "PostgreSQL status " << statusType;
	msg << "; message: '" << errmsg << "'";
	m_lasterror = msg.str();
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
#ifdef LOWLEVEL_DEBUG
	std::cerr << "CALL begin()" << std::endl;
#endif
	if (m_state != Init)
	{
		return errorStatus( std::string( "call of begin not allowed in state '") + stateName(m_state) + "'");
	}
	return status( PQexec( m_conn, "BEGIN;"), Transaction);
}

bool PreparedStatementHandler_postgres::commit()
{
#ifdef LOWLEVEL_DEBUG
	std::cerr << "CALL commit()" << std::endl;
#endif
	if (m_state == Transaction)
	{
		LOG_WARNING << "executed transaction is empty";
	}
	else if (m_state != Executed)
	{
		return errorStatus( std::string( "call of commit not allowed in state '") + stateName(m_state) + "'");
	}
	return status( PQexec( m_conn, "COMMIT;"), Init);
}

bool PreparedStatementHandler_postgres::rollback()
{
#ifdef LOWLEVEL_DEBUG
	std::cerr << "CALL rollback()" << std::endl;
#endif
	return status( PQexec( m_conn, "ROLLBACK;"), Init);
}

bool PreparedStatementHandler_postgres::errorStatus( const std::string& message)
{
	if (m_state != Error)
	{
		m_lasterror = message;
		m_state = Error;
	}
	return false;
}

bool PreparedStatementHandler_postgres::start( const std::string& stmname)
{
#ifdef LOWLEVEL_DEBUG
	std::cerr << "CALL start (" << stmname << ")" << std::endl;
#endif
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

bool PreparedStatementHandler_postgres::bind( std::size_t idx, const char* value)
{
#ifdef LOWLEVEL_DEBUG
	if (value)
	{
		std::cerr << "CALL bind( " << idx << ", '" << value << "' )" << std::endl;
	}
	else
	{
		std::cerr << "CALL bind( " << idx << ", NULL)" << std::endl;
	}
#endif
	if (m_state != Prepared && m_state != Executed)
	{
		return errorStatus( std::string( "call of bind not allowed in state '") + stateName(m_state) + "'");
	}
	if (idx == 0 || idx > m_statement.maxparam())
	{
		errorStatus( std::string( "index of parameter out of range (required to be in range 1..") + boost::lexical_cast<std::string>(m_statement.maxparam()) + " in statement '" + m_statement.string() + "'");
		return 0;
	}
	if (idx == 0 || idx > m_statement.maxparam())
	{
		errorStatus( std::string( "index of bind parameter out of range (required to be in range 1..") + boost::lexical_cast<std::string>(m_statement.maxparam()) + " in statement '" + m_statement.string() + "'");
		return 0;
	}
	if (value)
	{
		size_t valuesize = std::strlen( value);
		char* encvalue = (char*)std::malloc( valuesize * 2 + 3);
		encvalue[0] = '\'';
		boost::shared_ptr<void> encvaluer( encvalue, std::free);
		int error = 0;
		size_t encvaluesize = PQescapeStringConn( m_conn, encvalue+1, value, valuesize, &error);
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
#ifdef LOWLEVEL_DEBUG
	std::cerr << "CALL execute(" << stmstr << ")" << std::endl;
#endif
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

const char* PreparedStatementHandler_postgres::getLastError()
{
	return m_lasterror.size()?m_lasterror.c_str():0;
}

const char* PreparedStatementHandler_postgres::get( std::size_t idx)
{
#ifdef LOWLEVEL_DEBUG
	std::cerr << "CALL get(" << idx << ")" << std::endl;
#endif
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
	if (m_idx_row >= m_nof_rows) return 0;
	char* rt = PQgetvalue( m_lastresult, (int)m_idx_row, (int)idx-1);
	if (!rt || rt[0] == '\0')
	{
		if (PQgetisnull( m_lastresult, (int)m_idx_row, (int)idx-1))
		{
			return 0;
		}
	}
	return rt;
}

bool PreparedStatementHandler_postgres::next()
{
#ifdef LOWLEVEL_DEBUG
	std::cerr << "CALL next()" << std::endl;
#endif
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


