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
///\brief Implementation of processing prepared statements with sqlite3
///\file modules/database/sqlite3/SQLite3PreparedStatement.cpp

#include "SQLite3PreparedStatement.hpp"
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <limits>
#include <stdexcept>
#if SQLITE_VERSION_NUMBER < 3005000
#error This SQLite version is not supported by this module. It relies on the 'V2' interface
#endif

using namespace _Wolframe;
using namespace _Wolframe::db;

PreparedStatementHandler_sqlite3::PreparedStatementHandler_sqlite3( sqlite3* conn, const std::map<std::string,std::string>* stmmap)
	:m_state(Init)
	,m_conn(conn)
	,m_stmmap(stmmap)
	,m_stm(0){}

PreparedStatementHandler_sqlite3::~PreparedStatementHandler_sqlite3()
{
	clear();
}

void PreparedStatementHandler_sqlite3::clear()
{
	m_lasterror.clear();
	if (m_stm)
	{
		sqlite3_finalize( m_stm);
		m_stm = 0;
	}
	m_state = Init;
}

void PreparedStatementHandler_sqlite3::setDatabaseErrorMessage()
{
	const char* str = sqlite3_errmsg( m_conn);
	int errcode = sqlite3_errcode( m_conn);
#if SQLITE_VERSION_NUMBER >= 3006005
	int extcode = sqlite3_extended_errcode( m_conn);
#else
	int extcode = 0;
#endif

	std::ostringstream msg;
	msg << "SQLite error " << errcode;
	if (extcode != 0) msg << " (extended error code " << extcode << ")";
	msg << "; message: '" << str << "'";
	m_lasterror = msg.str();
}

bool PreparedStatementHandler_sqlite3::executeInstruction( const char* stmstr, State newstate)
{
	sqlite3_stmt* inst = 0;
	const char *stmtail;
	int rc = sqlite3_prepare_v2( m_conn, stmstr, -1, &inst, &stmtail);

	if (rc != SQLITE_OK) return status( rc, newstate);

	if (stmtail != 0)
	{
		std::size_t ii=0;
		while (stmtail[ii] && stmtail[ii]<32) ++ii;
		if (stmtail[ii]) throw std::logic_error( "executing list of SQL commands as a single instruction");
	}
	bool rt = status( rc, newstate) && status( sqlite3_step( inst), newstate);
	sqlite3_finalize( inst);
	return rt;
}

bool PreparedStatementHandler_sqlite3::begin()
{
	if (m_state != Init)
	{
		return errorStatus( std::string( "call of begin not allowed in state '") + stateName(m_state) + "'");
	}
	return executeInstruction( "BEGIN TRANSACTION;", Transaction);
}

bool PreparedStatementHandler_sqlite3::commit()
{
	if (m_state != Executed)
	{
		return errorStatus( std::string( "call of commit not allowed in state '") + stateName(m_state) + "'");
	}
	bool rt = executeInstruction( "COMMIT TRANSACTION;", Init);
	clear();
	return rt;
}

bool PreparedStatementHandler_sqlite3::rollback()
{
	bool rt = executeInstruction( "ROLLBACK TRANSACTION;", Init);
	clear();
	return rt;
}

bool PreparedStatementHandler_sqlite3::status( int rc, State newstate)
{
	if (rc != SQLITE_OK && rc != SQLITE_DONE)
	{
		if (m_state != Error)
		{
			setDatabaseErrorMessage();
			m_state = Error;
		}
		return false;
	}
	m_state = newstate;
	return true;
}

bool PreparedStatementHandler_sqlite3::errorStatus( const std::string& message)
{
	if (m_state != Error)
	{
		m_lasterror = message;
		m_state = Error;
	}
	return false;
}

bool PreparedStatementHandler_sqlite3::start( const std::string& stmname)
{
	if (m_state == Executed || m_state == Prepared)
	{
		sqlite3_finalize( m_stm);
		m_stm = 0;
		m_state = Transaction;
	}
	if (m_state != Transaction)
	{
		return errorStatus( std::string( "call of start not allowed in state '") + stateName(m_state) + "'");
	}
	std::map<std::string,std::string>::const_iterator si = m_stmmap->find( stmname);
	if (si == m_stmmap->end())
	{
		return errorStatus( std::string( "statement not found '") + stmname + "'");
	}
	const char *stmtail;
	int rc = sqlite3_prepare_v2( m_conn, si->second.c_str(), -1, &m_stm, &stmtail);
	if (rc == SQLITE_OK)
	{
		if (stmtail != 0)
		{
			std::size_t ii=0;
			while (stmtail[ii] && stmtail[ii]<32) ++ii;
			if (stmtail[ii])
			{
				return errorStatus( std::string( "prepared statement '") + stmname + "' consists of more than one SQL statement");
			}
		}
	}
	return status( rc, Prepared);
}

bool PreparedStatementHandler_sqlite3::bind( std::size_t idx, const char* value)
{
	if (m_state != Prepared || m_state != Executed)
	{
		return errorStatus( std::string( "call of bind not allowed in state '") + stateName(m_state) + "'");
	}
	if (idx == 0 || idx >= (std::size_t)std::numeric_limits<int>::max())
	{
		return errorStatus( "bind index out of range");
	}
	if (idx >= (std::size_t)sqlite3_bind_parameter_count( m_stm))
	{
		return errorStatus( "bind parameter index bigger than number of parameters in prepared statement");
	}
	if (value)
	{
		return status( sqlite3_bind_text( m_stm, (int)idx-1, value, 0,  SQLITE_STATIC), Prepared);
	}
	else
	{
		return status( sqlite3_bind_null( m_stm, (int)idx-1), Prepared);
	}
}

bool PreparedStatementHandler_sqlite3::execute()
{
	if (m_state != Prepared)
	{
		return errorStatus( std::string( "call of execute not allowed in state '") + stateName(m_state) + "'");
	}
	return status( sqlite3_step( m_stm), Executed);
}

std::size_t PreparedStatementHandler_sqlite3::nofColumns()
{
	if (m_state != Executed)
	{
		errorStatus( std::string( "number of columns not available in state '") + stateName(m_state) + "'");
		return 0;
	}
	return (std::size_t)sqlite3_column_count( m_stm);
}

const char* PreparedStatementHandler_sqlite3::columnName( std::size_t idx)
{
	if (m_state != Executed)
	{
		errorStatus( std::string( "number of columns not available in state '") + stateName(m_state) + "'");
		return 0;
	}
	if (idx == 0 || idx >= (std::size_t)std::numeric_limits<int>::max())
	{
		errorStatus( "column index out of range");
		return 0;
	}
	const char* rt = sqlite3_column_name( m_stm, (int)idx-1);
	if (!rt && idx <= (std::size_t)sqlite3_column_count( m_stm))
	{
		throw std::bad_alloc();
	}
	return rt;
}

const char* PreparedStatementHandler_sqlite3::getLastError()
{
	return m_lasterror.size()?m_lasterror.c_str():0;
}

const char* PreparedStatementHandler_sqlite3::get( std::size_t idx)
{
	if (m_state != Executed)
	{
		errorStatus( std::string( "number of columns not available in state '") + stateName(m_state) + "'");
		return 0;
	}
	if (idx == 0 || idx >= (std::size_t)std::numeric_limits<int>::max())
	{
		errorStatus( "column index out of range");
		return 0;
	}
	if (sqlite3_column_type( m_stm, (int)idx-1) == SQLITE_NULL)
	{
		return 0;
	}
	const char* rt = (const char*)sqlite3_column_text( m_stm, (int)idx-1);
	if (!rt && idx <= (std::size_t)sqlite3_column_count( m_stm))
	{
		errorStatus( "cannot convert result value to string for column");
		return 0;
	}
	return rt;
}

bool PreparedStatementHandler_sqlite3::next()
{
	if (m_state != Executed)
	{
		return errorStatus( std::string( "command not executed, next result of not available in state '") + stateName(m_state) + "'");
	}
	int rc = sqlite3_step( m_stm);
	if (rc == SQLITE_ROW) return true;
	return status( rc, Executed);
}


