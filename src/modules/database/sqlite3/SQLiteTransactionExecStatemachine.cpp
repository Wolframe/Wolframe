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
///\brief SQLite3 implementation of the standard database transaction execution statemechine
///\file SQLiteTransactionExecStatemachine.cpp
#include "SQLiteTransactionExecStatemachine.hpp"
#include "logger-v1.hpp"
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <limits>
#include <stdexcept>
#include <boost/lexical_cast.hpp>
#include <boost/cstdint.hpp>
#if SQLITE_VERSION_NUMBER < 3005000
#error This SQLite version is not supported by this module. It relies on the 'V2' interface
#endif

using namespace _Wolframe;
using namespace _Wolframe::db;

static int wrap_sqlite3_prepare_v2( sqlite3* c, const char* s, int n, sqlite3_stmt** stm, const char** t)
{
	LOG_DATA << "call sqlite3_prepare_v2 '" << ((n<0)?std::string(s):std::string(s,n)) << "'";
	return sqlite3_prepare_v2( c, s, n, stm, t);
}
static std::size_t wrap_sqlite3_bind_parameter_count( sqlite3_stmt* stm)
{
	std::size_t rt = sqlite3_bind_parameter_count(stm);
	LOG_DATA << "call sqlite3_bind_parameter_count returns " << rt;
	return rt;
}
static int wrap_sqlite3_bind_text( sqlite3_stmt* stm, int idx, const char* s, int n, void(*f)(void*))
{
	int rt = sqlite3_bind_text( stm, idx, s, n, f);
	LOG_DATA << "call sqlite3_bind_text " << idx << " '" << ((n<0)?std::string(s):std::string(s,n)) << "' returns " << rt;
	return rt;
}
static int wrap_sqlite3_bind_int( sqlite3_stmt* stm, int idx, int val)
{
	int rt = sqlite3_bind_int( stm, idx, val);
	LOG_DATA << "call sqlite3_bind_int " << idx << " " << val << " returns " << rt;
	return rt;
}
static int wrap_sqlite3_bind_int64( sqlite3_stmt* stm, int idx, types::Variant::Data::Int val)
{
	int rt = sqlite3_bind_int64( stm, idx, val);
	LOG_DATA << "call sqlite3_bind_int64 " << idx << " " << val << " returns " << rt;
	return rt;
}
static int wrap_sqlite3_bind_double( sqlite3_stmt* stm, int idx, double val)
{
	int rt = sqlite3_bind_double( stm, idx, val);
	LOG_DATA << "call sqlite3_bind_double " << idx << " " << val << " returns " << rt;
	return rt;
}
static int wrap_sqlite3_bind_null( sqlite3_stmt* stm, int idx)
{
	int rt = sqlite3_bind_null( stm, idx);
	LOG_DATA << "call sqlite3_bind_null " << idx << " returns " << rt;
	return rt;
}


TransactionExecStatemachine_sqlite3::TransactionExecStatemachine_sqlite3( sqlite3* conn, const std::string& dbname_, bool inTransactionContext)
	:m_state(inTransactionContext?Transaction:Init)
	,m_conn(conn)
	,m_dbname(dbname_)
	,m_hasResult(false)
	,m_hasRow(false)
	,m_stm(0){}

TransactionExecStatemachine_sqlite3::~TransactionExecStatemachine_sqlite3()
{
	clear();
}

void TransactionExecStatemachine_sqlite3::clear()
{
	m_lasterror.reset();
	if (m_stm)
	{
		sqlite3_finalize( m_stm);
		m_stm = 0;
	}
	m_hasResult = false;
	m_hasRow = false;
	m_curstm.clear();
	m_state = Init;
}

void TransactionExecStatemachine_sqlite3::setDatabaseErrorMessage()
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
	m_lasterror.reset( new DatabaseError( severity, extcode?extcode:errcode, m_dbname.c_str(), m_curstm.c_str(), errtype, errmsg, errmsg));
}

bool TransactionExecStatemachine_sqlite3::executeInstruction( const char* stmstr, State newstate)
{
	m_hasResult = false;
	m_hasRow = false;
	m_curstm.clear();
	sqlite3_stmt* inst = 0;
	const char *stmtail;
	int rc = wrap_sqlite3_prepare_v2( m_conn, stmstr, -1, &inst, &stmtail);

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

bool TransactionExecStatemachine_sqlite3::begin()
{
	LOG_TRACE << "[sqlite3 statement] CALL begin()";
	if (m_state != Init)
	{
		return errorStatus( std::string( "call of begin not allowed in state '") + stateName(m_state) + "'");
	}
	return executeInstruction( "BEGIN TRANSACTION;", Transaction);
}

bool TransactionExecStatemachine_sqlite3::commit()
{
	LOG_TRACE << "[sqlite3 statement] CALL commit()";
	if (m_state == Transaction)
	{
		LOG_WARNING << "executed transaction is empty";
	}
	else if (m_state != Executed && m_state != CommandReady)
	{
		return errorStatus( std::string( "call of commit not allowed in state '") + stateName(m_state) + "'");
	}
	return executeInstruction( "COMMIT TRANSACTION;", Init);
}

bool TransactionExecStatemachine_sqlite3::rollback()
{
	LOG_TRACE << "[sqlite3 statement] CALL rollback()";
	bool rt = executeInstruction( "ROLLBACK TRANSACTION;", Init);
	clear();
	return rt;
}

bool TransactionExecStatemachine_sqlite3::status( int rc, State newstate)
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

bool TransactionExecStatemachine_sqlite3::errorStatus( const std::string& message)
{
	if (m_state != Error)
	{
		m_lasterror.reset( new DatabaseError( log::LogLevel::LOGLEVEL_ERROR, 0, m_dbname.c_str(), m_curstm.c_str(), "INTERNAL", message.c_str(), "internal logic error (prepared statement)"));
		m_state = Error;
	}
	return false;
}

bool TransactionExecStatemachine_sqlite3::start( const std::string& statement)
{
	LOG_TRACE << "[sqlite3 statement] CALL start (" << statement << ")";
	m_hasResult = false;
	m_hasRow = false;
	m_curstm = statement;
	if (m_state == Executed || m_state == CommandReady)
	{
		sqlite3_finalize( m_stm);
		m_stm = 0;
		m_state = Transaction;
	}
	if (m_state != Transaction)
	{
		return errorStatus( std::string( "call of start not allowed in state '") + stateName(m_state) + "'");
	}
	const char *stmtail = 0;

	int rc = wrap_sqlite3_prepare_v2( m_conn, m_curstm.c_str(), m_curstm.size(), &m_stm, &stmtail);
	if (rc == SQLITE_OK)
	{
		if (stmtail != 0)
		{
			std::size_t ii=0;
			while (stmtail[ii] && stmtail[ii]<32) ++ii;
			if (stmtail[ii])
			{
				return errorStatus( std::string( "prepared statement '") + statement + "' consists of more than one SQL statement");
			}
		}
	}
	return status( rc, CommandReady);
}

bool TransactionExecStatemachine_sqlite3::bind( std::size_t idx, const types::VariantConst& value)
{
	if (value.defined())
	{
		LOG_TRACE << "[sqlite3 statement] CALL bind( " << idx << ", '" << value << "', " << types::Variant::typeName( value.type()) << " )";
	}
	else
	{
		LOG_TRACE << "[sqlite3 statement] CALL bind( " << idx << ", NULL)";
	}
	if (m_state != CommandReady && m_state != Executed)
	{
		return errorStatus( std::string( "call of bind not allowed in state '") + stateName(m_state) + "'");
	}
	if (idx == 0 || idx >= (std::size_t)std::numeric_limits<int>::max())
	{
		return errorStatus( std::string( "bind index out of range (") + boost::lexical_cast<std::string>(idx) + ")");
	}
	std::size_t stmcnt = (std::size_t)wrap_sqlite3_bind_parameter_count( m_stm);
	if (idx > stmcnt)
	{
		return errorStatus( std::string( "bind parameter index bigger than number of parameters in prepared statement (") + boost::lexical_cast<std::string>(idx) + " in " + m_curstm + ")");
	}
	switch (value.type())
	{
		case types::Variant::Null:
			return status( wrap_sqlite3_bind_null( m_stm, (int)idx), CommandReady);
		case types::Variant::Bool:
			return status( wrap_sqlite3_bind_int( m_stm, (int)idx, value.tobool()), CommandReady);
		case types::Variant::Int:
			return status( wrap_sqlite3_bind_int( m_stm, (int)idx, value.toint()), CommandReady);
		case types::Variant::UInt:
			if (value.touint() < (types::Variant::Data::UInt)( std::numeric_limits<types::Variant::Data::Int>::max() ))
			{
				return status( wrap_sqlite3_bind_int64( m_stm, (int)idx, value.touint()), CommandReady);
			}
			else
			{
				std::string strval( value.tostring());
				return status( wrap_sqlite3_bind_text( m_stm, (int)idx, strval.c_str(), strval.size(), SQLITE_STATIC), CommandReady);
			}
		case types::Variant::Double:
			return status( wrap_sqlite3_bind_double( m_stm, (int)idx, value.todouble()), CommandReady);
		case types::Variant::String:
			return status( wrap_sqlite3_bind_text( m_stm, (int)idx, value.charptr(), value.charsize(), SQLITE_STATIC), CommandReady);
	}
	return errorStatus( std::string( "cannot bind parameter of this type '") + types::Variant::typeName( value.type()) + "'");
}

bool TransactionExecStatemachine_sqlite3::execute()
{
	LOG_TRACE << "[sqlite3 statement] CALL execute()";
	if (m_state != CommandReady)
	{
		return errorStatus( std::string( "call of execute not allowed in state '") + stateName(m_state) + "'");
	}
	int rc = sqlite3_step( m_stm);
	if (rc == SQLITE_ROW)
	{
		m_hasResult = true;
		m_hasRow = true;
		while (firstResultIsNullRow())
		{
			if (!next())
			{
				m_hasRow = false;
				break; 
			}
		}
	}
	else
	{
		m_hasResult = (rc == SQLITE_DONE);
		m_hasRow = false;
	}
	if (!m_hasResult)
	{
		LOG_TRACE << "[sqlite3 statement] CALL rows_affected(" << sqlite3_changes( m_conn) << ")";
	}
	else if (m_hasRow)
	{
		LOG_TRACE << "[sqlite3 statement] CALL got results";
	}
	else
	{
		LOG_TRACE << "[sqlite3 statement] CALL got empty set as result";
	}
	return status( rc, Executed);
}

bool TransactionExecStatemachine_sqlite3::hasResult()
{
	return m_hasRow;
}

std::size_t TransactionExecStatemachine_sqlite3::nofColumns()
{
	if (m_state != Executed)
	{
		errorStatus( std::string( "number of columns not available in state '") + stateName(m_state) + "'");
		return 0;
	}
	return (std::size_t)sqlite3_column_count( m_stm);
}

const char* TransactionExecStatemachine_sqlite3::columnName( std::size_t idx)
{
	if (m_state != Executed)
	{
		errorStatus( std::string( "name of columns not available in state '") + stateName(m_state) + "'");
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

const db::DatabaseError* TransactionExecStatemachine_sqlite3::getLastError()
{
	return m_lasterror.get();
}

bool TransactionExecStatemachine_sqlite3::firstResultIsNullRow() const
{
	if (!m_hasRow || m_state != Executed) return false;
	int ii=0, nn=(std::size_t)sqlite3_column_count( m_stm);
	for (; ii<nn; ++ii)
	{
		if (sqlite3_column_type( m_stm, ii) != SQLITE_NULL) return false;
	}
	return true;
}

types::VariantConst TransactionExecStatemachine_sqlite3::get( std::size_t idx)
{
	if (m_state != Executed)
	{
		errorStatus( std::string( "data of result columns not available in state '") + stateName(m_state) + "'");
		LOG_DATA << "[sqlite3 statement] CALL get(" << idx << ") => NULL";
		return types::VariantConst();
	}
	if (idx == 0 || idx > (std::size_t)sqlite3_column_count( m_stm))
	{
		errorStatus( std::string( "column index out of range (") + boost::lexical_cast<std::string>(idx) + ")");
		LOG_DATA << "[sqlite3 statement] CALL get(" << idx << ") => NULL";
		return types::VariantConst();
	}
	int restype = sqlite3_column_type( m_stm, (int)idx-1);
	if (restype == SQLITE_INTEGER)
	{
		sqlite3_int64 resval = sqlite3_column_int64( m_stm, (int)idx-1);
		LOG_DATA << "[sqlite3 statement] CALL get(" << idx << ") => SQLITE_INTEGER " << resval;
		return types::VariantConst( (types::Variant::Data::Int)resval);
	}
	else if (restype == SQLITE_FLOAT)
	{
		double resval = sqlite3_column_double( m_stm, (int)idx-1);
		LOG_DATA << "[sqlite3 statement] CALL get(" << idx << ") => SQLITE_FLOAT " << resval;
		return types::VariantConst( resval);
	}
	else if (restype == SQLITE_TEXT)
	{
		const char* resval = (const char*)sqlite3_column_text( m_stm, (int)idx-1);
		if (!resval) resval = "";
		LOG_DATA << "[sqlite3 statement] CALL get(" << idx << ") => SQLITE_TEXT '" << resval << "'";
		return types::VariantConst( resval);
	}
	else  if (restype == SQLITE_BLOB)
	{
		const char* resval = (const char*)sqlite3_column_blob( m_stm, (int)idx-1);
		int ressize = resval?sqlite3_column_bytes( m_stm, (int)idx-1):0;
		LOG_DATA << "[sqlite3 statement] CALL get(" << idx << ") => SQLITE_BLOB -binary data-";
		return types::VariantConst( resval, ressize);
	}
	else  if (restype == SQLITE_NULL)
	{
		LOG_DATA << "[sqlite3 statement] CALL get(" << idx << ") => SQLITE_NULL";
		return types::VariantConst();
	}
	else
	{
		errorStatus( std::string( "cannot handle result of this type (SQLITE type ") + boost::lexical_cast<std::string>(restype) + "'");
		return types::VariantConst();
	}
}

bool TransactionExecStatemachine_sqlite3::next()
{
	LOG_TRACE << "[sqlite3 statement] [sqlite3 statement] CALL next()";
	if (m_state != Executed)
	{
		return errorStatus( std::string( "command not executed, next result of not available in state '") + stateName(m_state) + "'");
	}
	int rc = sqlite3_step( m_stm);
	m_hasRow = (rc == SQLITE_ROW);
	if (m_hasRow) return true;
	if (rc == SQLITE_DONE) return false;
	return status( rc, Executed);
}


