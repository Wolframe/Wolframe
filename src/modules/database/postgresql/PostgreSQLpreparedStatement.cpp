/************************************************************************

 Copyright (C) 2011, 2012 Project Wolframe.
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
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <stdexcept>

using namespace _Wolframe;
using namespace _Wolframe::db;

PreparedStatementHandler_postgres::PreparedStatementHandler_postgres( PGconn* conn)
	:m_state(Init)
	,m_conn(conn)
	,m_lastresult(0)
	,m_nof_rows(0)
	,m_idx_row(0)
{
	m_stmparamstr.push_back( '\0');
}

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
	m_stmname.clear();
	m_stmparamstr.clear();
	m_stmparamstr.push_back( '\0');
	m_bind.clear();
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
	if (PQresultStatus( res) != PGRES_COMMAND_OK)
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
	if (m_state != Init)
	{
		return errorStatus( std::string( "call of begin not allowed in state '") + stateName(m_state) + "'");
	}
	return status( PQexec( m_conn, "BEGIN;"), Transaction);
}

bool PreparedStatementHandler_postgres::commit()
{
	if (m_state != Executed)
	{
		return errorStatus( std::string( "call of commit not allowed in state '") + stateName(m_state) + "'");
	}
	return status( PQexec( m_conn, "COMMIT;"), Init);
}

bool PreparedStatementHandler_postgres::rollback()
{
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
	if (m_state == Executed || m_state == Prepared)
	{
		m_state = Transaction;
	}
	if (m_state != Transaction)
	{
		return errorStatus( std::string( "call of start not allowed in state '") + stateName(m_state) + "'");
	}
	clear();
	m_stmname = stmname;
	m_state = Prepared;
	return true;
}

bool PreparedStatementHandler_postgres::bind( std::size_t idx, const char* value)
{
	if (m_state != Prepared && m_state != Executed)
	{
		return errorStatus( std::string( "call of bind not allowed in state '") + stateName(m_state) + "'");
	}
	if (idx == 0 || idx > MaxBindParameters)
	{
		return errorStatus( std::string( "index of bind parameter out of range (required to be in range 1..64)"));
	}
	while (m_bind.size() <= idx) m_bind.push_back( 0);
	if (!value) return true;
	m_bind[ idx] = m_stmparamstr.size();
	m_stmparamstr.append( value, std::strlen(value)+1);
	m_state = Prepared;
	return true;
}

bool PreparedStatementHandler_postgres::execute()
{
	if (m_state != Prepared)
	{
		return errorStatus( std::string( "call of bind not allowed in state '") + stateName(m_state) + "'");
	}
	const char* paramValues[ MaxBindParameters];
	int paramLengths[ MaxBindParameters];
	int paramFormats[ MaxBindParameters];
	const int resultFormat = 0;

	std::size_t ii=0, nn=m_bind.size();
	for (; ii<nn; ++ii)
	{
		paramValues[ ii] = m_stmparamstr.c_str() + m_bind[ ii];
		paramLengths[ ii] = 0;
		paramFormats[ ii] = 0;
	}
	if (m_lastresult)
	{
		PQclear( m_lastresult);
		m_lastresult = 0;
	}
	m_lastresult = PQexecPrepared( m_conn, m_stmname.c_str(), m_bind.size(),
					paramValues, paramLengths, paramFormats,
					resultFormat);

	bool rt = status( m_lastresult, Executed);
	if (rt)
	{
		m_nof_rows = (std::size_t)PQntuples( m_lastresult);
		m_idx_row = 0;
	}
	return rt;
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
	if (idx == 0 || idx > m_bind.size())
	{
		errorStatus( std::string( "index of parameter out of range (required to be in range 1..64)"));
		return 0;
	}
	return PQfname( m_lastresult, (int)idx-1);
}

const char* PreparedStatementHandler_postgres::getLastError()
{
	return m_lasterror.size()?m_lasterror.c_str():0;
}

const char* PreparedStatementHandler_postgres::get( std::size_t idx)
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
	if (m_idx_row >= m_nof_rows) return 0;
	if (idx == 0 || idx > m_bind.size())
	{
		errorStatus( std::string( "index of parameter out of range (required to be in range 1..64)"));
		return 0;
	}
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


