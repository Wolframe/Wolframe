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
///\brief Implementation of processing prepared statements with sqlite3
///\file modules/database/sqlite3/SQLitePreparedStatement.cpp
#include "SQLitePreparedStatement.hpp"
#include "logger-v1.hpp"
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <limits>
#include <stdexcept>
#include <boost/lexical_cast.hpp>
#if SQLITE_VERSION_NUMBER < 3005000
#error This SQLite version is not supported by this module. It relies on the 'V2' interface
#endif

using namespace _Wolframe;
using namespace _Wolframe::db;

PreparedStatementHandler_sqlite3::PreparedStatementHandler_sqlite3( sqlite3* conn, const std::string& dbname_, const types::keymap<std::string>* stmmap, bool inTransactionContext)
	:m_state(inTransactionContext?Transaction:Init)
	,m_conn(conn)
	,m_dbname(dbname_)
	,m_stmmap(stmmap)
	,m_hasResult(false)
	,m_stm(0)
{
	m_curstm = m_stmmap->end();
}

PreparedStatementHandler_sqlite3::~PreparedStatementHandler_sqlite3()
{
	clear();
}

void PreparedStatementHandler_sqlite3::clear()
{
	m_lasterror.reset();
	if (m_stm)
	{
		sqlite3_finalize( m_stm);
		m_stm = 0;
	}
	m_hasResult = false;
	m_curstm = m_stmmap->end();
	m_state = Init;
}

void PreparedStatementHandler_sqlite3::setDatabaseErrorMessage()
{
	int errcode = sqlite3_errcode( m_conn);
#if SQLITE_VERSION_NUMBER >= 3006005
	int extcode = sqlite3_extended_errcode( m_conn);
#else
	int extcode = 0;
#endif
	const char* errmsg = sqlite3_errmsg( m_conn);
	const char* errtype = 0;
	switch (errcode)
	{
		case SQLITE_ERROR: errtype = "DATABASE"; break;
		case SQLITE_INTERNAL: errtype = "INTERNAL"; break;
		case SQLITE_PERM: errtype = "PRIVILEGE"; break;
		case SQLITE_ABORT: errtype = "EXCEPTION"; break;
		case SQLITE_LOCKED: errtype = "RESOURCE"; break;
		case SQLITE_BUSY: errtype = "RESOURCE"; break;
		case SQLITE_NOMEM: errtype = "RESOURCE"; break;
		case SQLITE_READONLY: errtype = "RESOURCE"; break;
		case SQLITE_INTERRUPT: errtype = "EXCEPTION"; break;
		case SQLITE_IOERR: errtype = "SYSTEM"; break;
		case SQLITE_CORRUPT: errtype = "INTERNAL"; break;
		case SQLITE_NOTFOUND: errtype = "INTERNAL"; break;
		case SQLITE_FULL: errtype = "RESOURCE"; break;
		case SQLITE_CANTOPEN: errtype = "RESOURCE"; break;
		case SQLITE_PROTOCOL: errtype = "INTERNAL"; break;
		case SQLITE_EMPTY: errtype = "DATABASE"; break;
		case SQLITE_SCHEMA: errtype = "DATABASE"; break;
		case SQLITE_TOOBIG: errtype = "RESOURCE"; break;
		case SQLITE_CONSTRAINT: errtype = "CONSTRAINT"; break;
		case SQLITE_MISMATCH: errtype = "PARAMETER"; break;
		case SQLITE_MISUSE: errtype = "INTERNAL"; break;
		case SQLITE_NOLFS: errtype = "SYSTEM"; break;
		case SQLITE_AUTH: errtype = "PRIVILEGE"; break;
		case SQLITE_FORMAT: errtype = "SYSTEM"; break;
		case SQLITE_RANGE: errtype = "PARAMETER"; break;
		case SQLITE_NOTADB: errtype = "SYSTEM"; break;
	}
	log::LogLevel::Level severity = log::LogLevel::LOGLEVEL_ERROR;
	m_lasterror.reset( new DatabaseError( severity, extcode?extcode:errcode, m_dbname.c_str(), m_curstm->second.c_str(), errtype, errmsg, errmsg));
}

bool PreparedStatementHandler_sqlite3::executeInstruction( const char* stmstr, State newstate)
{
	m_hasResult = false;
	m_curstm = m_stmmap->end();
	sqlite3_stmt* inst = 0;
	const char *stmtail;
	int rc = sqlite3_prepare_v2( m_conn, stmstr, -1, &inst, &stmtail);

	if (rc != SQLITE_OK && rc != SQLITE_DONE) return status( rc, newstate);

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
	LOG_TRACE << "[sqlite3 statement] CALL begin()";
	if (m_state != Init)
	{
		return errorStatus( std::string( "call of begin not allowed in state '") + stateName(m_state) + "'");
	}
	return executeInstruction( "BEGIN TRANSACTION;", Transaction);
}

bool PreparedStatementHandler_sqlite3::commit()
{
	LOG_TRACE << "[sqlite3 statement] CALL commit()";
	if (m_state == Transaction)
	{
		LOG_WARNING << "executed transaction is empty";
	}
	else if (m_state != Executed && m_state != Prepared)
	{
		return errorStatus( std::string( "call of commit not allowed in state '") + stateName(m_state) + "'");
	}
	return executeInstruction( "COMMIT TRANSACTION;", Init);
}

bool PreparedStatementHandler_sqlite3::rollback()
{
	LOG_TRACE << "[sqlite3 statement] CALL rollback()";
	bool rt = executeInstruction( "ROLLBACK TRANSACTION;", Init);
	clear();
	return rt;
}

bool PreparedStatementHandler_sqlite3::status( int rc, State newstate)
{
	if (rc != SQLITE_OK && rc != SQLITE_DONE && rc != SQLITE_ROW)
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
		m_lasterror.reset( new DatabaseError( log::LogLevel::LOGLEVEL_ERROR, 0, m_dbname.c_str(), m_curstm->second.c_str(), "INTERNAL", message.c_str(), "internal logic error (prepared statement)"));
		m_state = Error;
	}
	return false;
}

bool PreparedStatementHandler_sqlite3::start( const std::string& stmname)
{
	LOG_TRACE << "[sqlite3 statement] CALL start (" << stmname << ")";
	m_hasResult = false;
	m_curstm = m_stmmap->find( stmname);
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
	if (m_curstm == m_stmmap->end())
	{
		return errorStatus( std::string( "statement not found '") + stmname + "'");
	}
	const char *stmtail;
	int rc = sqlite3_prepare_v2( m_conn, m_curstm->second.c_str(), -1, &m_stm, &stmtail);
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

bool PreparedStatementHandler_sqlite3::bind( std::size_t idx, const types::Variant& value)
{
	if (value.defined())
	{
		LOG_TRACE << "[sqlite3 statement] CALL bind( " << idx << ", '" << value << "' )";
	}
	else
	{
		LOG_TRACE << "[sqlite3 statement] CALL bind( " << idx << ", NULL)";
	}
	if (m_state != Prepared && m_state != Executed)
	{
		return errorStatus( std::string( "call of bind not allowed in state '") + stateName(m_state) + "'");
	}
	if (idx == 0 || idx >= (std::size_t)std::numeric_limits<int>::max())
	{
		return errorStatus( std::string( "bind index out of range (") + boost::lexical_cast<std::string>(idx) + ")");
	}
	std::size_t stmcnt = (std::size_t)sqlite3_bind_parameter_count( m_stm);
	if (idx > stmcnt)
	{
		return errorStatus( std::string( "bind parameter index bigger than number of parameters in prepared statement (") + boost::lexical_cast<std::string>(idx) + " in " + m_curstm->second + ")");
	}
	if (value.defined())
	{
		std::string strval = value.tostring();
		return status( sqlite3_bind_text( m_stm, (int)idx, strval.c_str(), strval.size(),  SQLITE_STATIC), Prepared);
	}
	else
	{
		return status( sqlite3_bind_null( m_stm, (int)idx), Prepared);
	}
}

bool PreparedStatementHandler_sqlite3::execute()
{
	LOG_TRACE << "[sqlite3 statement] CALL execute()";
	if (m_state != Prepared)
	{
		return errorStatus( std::string( "call of execute not allowed in state '") + stateName(m_state) + "'");
	}
	int rc = sqlite3_step( m_stm);
	m_hasResult = (rc == SQLITE_ROW);
	return status( rc, Executed);
}

bool PreparedStatementHandler_sqlite3::hasResult()
{
	return m_hasResult;
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
		errorStatus( std::string( "column index out of range (") + boost::lexical_cast<std::string>(idx) + ")");
		return 0;
	}
	const char* rt = sqlite3_column_name( m_stm, (int)idx-1);
	if (!rt && idx <= (std::size_t)sqlite3_column_count( m_stm))
	{
		throw std::logic_error( "array bound read accessing column name");
	}
	return rt;
}

const db::DatabaseError* PreparedStatementHandler_sqlite3::getLastError()
{
	return m_lasterror.get();
}

types::VariantConst PreparedStatementHandler_sqlite3::get( std::size_t idx)
{
	LOG_TRACE << "[sqlite3 statement] CALL get(" << idx << ")";
	if (m_state != Executed)
	{
		errorStatus( std::string( "number of columns not available in state '") + stateName(m_state) + "'");
		return types::VariantConst();
	}
	if (idx == 0 || idx >= (std::size_t)std::numeric_limits<int>::max())
	{
		errorStatus( std::string( "column index out of range (") + boost::lexical_cast<std::string>(idx) + ")");
		return types::VariantConst();
	}
	if (sqlite3_column_type( m_stm, (int)idx-1) == SQLITE_NULL)
	{
		return types::VariantConst();
	}
	const char* rt = (const char*)sqlite3_column_text( m_stm, (int)idx-1);
	if (!rt && idx <= (std::size_t)sqlite3_column_count( m_stm))
	{
		errorStatus( std::string( "cannot convert result value to string for column (") + boost::lexical_cast<std::string>(idx) + ")");
		return types::VariantConst();
	}
	return types::VariantConst( rt);
}

bool PreparedStatementHandler_sqlite3::next()
{
	LOG_TRACE << "[sqlite3 statement] [sqlite3 statement] CALL next()";
	if (m_state != Executed)
	{
		return errorStatus( std::string( "command not executed, next result of not available in state '") + stateName(m_state) + "'");
	}
	int rc = sqlite3_step( m_stm);
	if (rc == SQLITE_ROW) return true;
	if (rc == SQLITE_DONE) return false;
	return status( rc, Executed);
}


