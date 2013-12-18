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
///\brief Implementation of the standard database transaction execution statemechine with Oracle
///\file OracletransactionExecStatemachine.cpp

#include "OracletransactionExecStatemachine.hpp"
#include "Oracle.hpp"
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

TransactionExecStatemachine_oracle::TransactionExecStatemachine_oracle( OracleEnvirenment *env_, OracleConnection* conn_, const std::string& dbname_, bool inTransactionContext)
	:m_state(inTransactionContext?Transaction:Init)
	,m_env(env_)
	,m_conn(conn_)
	,m_dbname(dbname_)
	,m_lastresult(0)
	,m_nof_rows(0)
	,m_idx_row(0)
	,m_hasResult(false){}

TransactionExecStatemachine_oracle::~TransactionExecStatemachine_oracle()
{
	clear();
}

void TransactionExecStatemachine_oracle::clear()
{
	if (m_lastresult)
	{
		(void)OCIHandleFree( m_lastresult, OCI_HTYPE_STMT );
		m_lastresult = 0;
	}
	m_lasterror.reset();
	m_statement.clear();
	m_state = Init;
	m_nof_rows = 0;
	m_idx_row = 0;
	m_hasResult = false;
}

static const char* getErrorType( sword errorcode )
{
	// TODO: enum of types, not strings!
	// PRIVILEGE, STATEMENT, CONNECTION, EXCEPTION, PARAMETER,
	// CONSTRAINT, SYNTAX, RESOURCE, SYSTEM, CONFIGURATION,
	// PLSQL (bad naming!), INTERNAL
	// TODO: explain what means what
	// TODO: are those the right classes?!
	
	// TODO: find OCI error code list and map it
	switch( errorcode ) {
		case 1017:
			return "PRIVILEGE";
		default:
			return "INTERNAL";
	}
}

void TransactionExecStatemachine_oracle::setDatabaseErrorMessage( sword status_ )
{
	sb4 errcode = 0;
	char errmsg[512];

	switch( status_ ) {
		case OCI_SUCCESS:
			strcpy( errmsg, "OCI_SUCCESS" );
			break;

		case OCI_SUCCESS_WITH_INFO:
			strcpy( errmsg, "OCI_SUCCESS_WITH_INFO" );
			break;
		
		case OCI_NEED_DATA:
			strcpy( errmsg, "OCI_NEED_DATA" );
			break;
		
		case OCI_NO_DATA:
			strcpy( errmsg, "OCI_NO_DATA" );
			break;
		
		case OCI_INVALID_HANDLE:
			strcpy( errmsg, "OCI_INVALID_HANDLE" );
			break;
		
		case OCI_STILL_EXECUTING:
			strcpy( errmsg, "OCI_STILL_EXECUTING" );
			break;
		
		case OCI_CONTINUE:
			strcpy( errmsg, "OCI_CONTINUE" );
			break;
			
		case OCI_ERROR:
			(void)OCIErrorGet( (dvoid *)m_conn->errhp, (ub4)1, (text *)NULL,
				&errcode, (text *)errmsg, (ub4)sizeof( errmsg ), OCI_HTYPE_ERROR );
			break;
	}
	
	const char *usermsg = 0;
	const char *errtype = getErrorType( errcode );
	
	// TODO: map OCI codes to severity levels, so far everything is ERROR
	//~ const char* severitystr = m_lastresult?PQresultErrorField( m_lastresult, PG_DIAG_SEVERITY):"ERROR";
	//~ log::LogLevel::Level severity = OracledbUnit::getLogLevel( severitystr);
	log::LogLevel::Level severity = log::LogLevel::LOGLEVEL_ERROR;
	
	m_lasterror.reset( new DatabaseError( severity, errcode, m_dbname.c_str(), m_statement.string().c_str(), errtype, errmsg, usermsg));
}

bool TransactionExecStatemachine_oracle::status( sword status_, State newstate )
{
	bool rt;

	// TODO: check better for status
	
	if( status_ == OCI_SUCCESS ) {
		// TODO: m_hasResult = xx;
		m_hasResult = false;
		m_state = newstate;
		rt = true;
	} else {
		setDatabaseErrorMessage( status_ );
		m_state = Error;
		rt = false;
	}
	
	// TODO: close results sets, cleanup
		
	return rt;
}

bool TransactionExecStatemachine_oracle::begin()
{
	LOG_TRACE << "[oracle statement] CALL begin()";
	if (m_state != Init)
	{
		return errorStatus( std::string( "call of begin not allowed in state '") + stateName(m_state) + "'");
	}

	return status( OCITransStart( m_conn->svchp, m_conn->errhp, (uword)0, (ub4)OCI_TRANS_NEW ), Transaction );
}

bool TransactionExecStatemachine_oracle::commit()
{
	LOG_TRACE << "[oracle statement] CALL commit()";
	if (m_state == Transaction)
	{
		LOG_WARNING << "executed transaction is empty";
	}
	else if (m_state != Executed && m_state != CommandReady)
	{
		return errorStatus( std::string( "call of commit not allowed in state '") + stateName(m_state) + "'");
	}
	
	return status( OCITransCommit( m_conn->svchp, m_conn->errhp, OCI_DEFAULT ), Init );
}

bool TransactionExecStatemachine_oracle::rollback()
{
	LOG_TRACE << "[oracle statement] CALL rollback()";

	return status( OCITransRollback( m_conn->svchp, m_conn->errhp, OCI_DEFAULT ), Init );
}

bool TransactionExecStatemachine_oracle::errorStatus( const std::string& message)
{
	if (m_state != Error)
	{
		m_lasterror.reset( new DatabaseError( log::LogLevel::LOGLEVEL_ERROR, 0, m_dbname.c_str(), m_statement.string().c_str(), "INTERNAL", message.c_str(), "internal logic error"));
		m_state = Error;
	}
	return false;
}

bool TransactionExecStatemachine_oracle::start( const std::string& statement)
{
	LOG_TRACE << "[oracle statement] CALL start (" << statement << ")";
	if (m_state == Executed || m_state == CommandReady)
	{
		m_state = Transaction;
	}
	if (m_state != Transaction)
	{
		return errorStatus( std::string( "call of start not allowed in state '") + stateName(m_state) + "'");
	}
	clear();
	m_statement.init( statement);
	m_state = CommandReady;
	return true;
}

bool TransactionExecStatemachine_oracle::bind( std::size_t idx, const types::VariantConst& value)
{
	if (value.defined())
	{
		LOG_TRACE << "[oracle statement] CALL bind( " << idx << ", '" << value << "' )";
	}
	else
	{
		LOG_TRACE << "[oracle statement] CALL bind( " << idx << ", NULL)";
	}
	if (m_state != CommandReady && m_state != Executed)
	{
		return errorStatus( std::string( "call of bind not allowed in state '") + stateName(m_state) + "'");
	}
	if (idx == 0 || idx > m_statement.maxparam())
	{
		errorStatus( std::string( "index of parameter out of range (") + boost::lexical_cast<std::string>(idx) + " required to be in range 1.." + boost::lexical_cast<std::string>(m_statement.maxparam()) + " in statement '" + m_statement.string() + "'");
		return false;
	}
	if (idx == 0 || idx > m_statement.maxparam())
	{
		errorStatus( std::string( "index of bind parameter out of range (") + boost::lexical_cast<std::string>(idx) + " required to be in range 1.." + boost::lexical_cast<std::string>(m_statement.maxparam()) + " in statement '" + m_statement.string() + "'");
		return false;
	}
	if (value.defined())
	{
		std::string strval = value.tostring();
		char* encvalue = (char*)std::malloc( strval.size() * 2 + 3);
		encvalue[0] = '\'';
		boost::shared_ptr<void> encvaluer( encvalue, std::free);
		//~ int error = 0;
//		size_t encvaluesize = PQescapeStringConn( m_conn, encvalue+1, strval.c_str(), strval.size(), &error);
		size_t encvaluesize = 0;
		encvalue[encvaluesize+1] = '\'';
		std::string bindval( encvalue, encvaluesize+2);
		m_statement.bind( idx, bindval);
	}
	else
	{
		m_statement.bind( idx, "NULL");
	}
	m_state = CommandReady;
	return true;
}

bool TransactionExecStatemachine_oracle::execute()
{
	if (m_state != CommandReady)
	{
		return errorStatus( std::string( "call of execute not allowed in state '") + stateName(m_state) + "'");
	}
	if (m_lastresult)
	{
		(void)OCIHandleFree( m_lastresult, OCI_HTYPE_STMT );
		m_lastresult = 0;
	}
	std::string stmstr = m_statement.expanded();
	LOG_TRACE << "[oracle statement] CALL execute(" << stmstr << ")";

	sword status_ = OCIHandleAlloc( m_env->envhp, (dvoid **)&m_lastresult,
		OCI_HTYPE_STMT, (size_t)0, (dvoid **)0 );
	bool rt = status( status_, Executed );
	if( !rt ) return rt;

	status_ = OCIStmtPrepare( m_lastresult, m_conn->errhp, 
		(text *)const_cast<char *>( stmstr.c_str( ) ),
		(ub4)stmstr.length( ), (ub4)OCI_NTV_SYNTAX, (ub4)OCI_DEFAULT );
	rt = status( status_, Executed );
	if( !rt ) return rt;

	// TODO: here I would like to bind a parameter of type variant
	// as a bind parameter, I would also like to prepare the statement
	// above only once. Currently they are escaped and mapped into
	// an expanded statement.

	status_ = OCIStmtExecute( m_conn->svchp, m_lastresult, m_conn->errhp, (ub4)1, (ub4)0,
		NULL, NULL, OCI_DEFAULT );

	rt = status( status_, Executed);
	if (rt)
	{
		ub4 counter = 1;
		OCIParam *paraDesc = 0;
		status_ = OCIParamGet( (dvoid *)m_lastresult, OCI_HTYPE_STMT,
			m_conn->errhp, (dvoid **)&paraDesc, (ub4)counter );
		rt = status( status_, Executed );
		if( !rt ) return rt;
		
		while( status_ == OCI_SUCCESS ) {
			
			/* data type of the result column: used to allocate the
			 * correct size for the data container. could also be
			 * used later to create the best-fitting type of variant..
			 */
			ub2 dataType;
			status_ = OCIAttrGet( (dvoid *)paraDesc, (ub4)OCI_DTYPE_PARAM,
				(dvoid *)&dataType, (ub4 *)0, (ub4)OCI_ATTR_DATA_TYPE,
				m_conn->errhp );
			rt = status( status_, Executed );
			if( !rt ) return rt;
			
			// get next column descriptor (if there is any)
			counter++;
			status_ = OCIParamGet( (dvoid *)m_lastresult, OCI_HTYPE_STMT,
				m_conn->errhp, (dvoid **)&paraDesc, (ub4)counter );
			rt = status( status_, Executed );
			if( !rt ) return rt;

#if 0
		/* Retrieve the column name attribute */
   col_name_len = 0;
   checkerr(errhp, OCIAttrGet((dvoid*) mypard, (ub4) OCI_DTYPE_PARAM,
           (dvoid**) &col_name, (ub4 *) &col_name_len, (ub4) OCI_ATTR_NAME,
           (OCIError *) errhp ));	
   /* Retrieve the column name attribute */
   col_name_len = 0;
   checkerr(errhp, OCIAttrGet((dvoid*) mypard, (ub4) OCI_DTYPE_PARAM,
           (dvoid**) &col_name, (ub4 *) &col_name_len, (ub4) OCI_ATTR_NAME,
           (OCIError *) errhp ));

   /* Retrieve the length semantics for the column */
   char_semantics = 0;
   checkerr(errhp, OCIAttrGet((dvoid*) mypard, (ub4) OCI_DTYPE_PARAM,
           (dvoid*) &char_semantics,(ub4 *) 0, (ub4) OCI_ATTR_CHAR_USED,
           (OCIError *) errhp  ));
   col_width = 0;
   if (char_semantics)
       /* Retrieve the column width in characters */
       checkerr(errhp, OCIAttrGet((dvoid*) mypard, (ub4) OCI_DTYPE_PARAM,
               (dvoid*) &col_width, (ub4 *) 0, (ub4) OCI_ATTR_CHAR_SIZE,
               (OCIError *) errhp  ));
   else
       /* Retrieve the column width in bytes */
       checkerr(errhp, OCIAttrGet((dvoid*) mypard, (ub4) OCI_DTYPE_PARAM,
               (dvoid*) &col_width,(ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,
               (OCIError *) errhp  ));

   /* increment counter and get next descriptor, if there is one */
   counter++;
   parm_status = OCIParamGet((dvoid *)stmthp, OCI_HTYPE_STMT, errhp,
          (dvoid **)&mypard, (ub4) counter);

/* The next two statements describe the select-list item, dname, and
   return its length */

  /* Use the retrieved length of dname to allocate an output buffer, and
   then define the output variable. If the define call returns an error,
   exit the application */
  dept = (text *) malloc((int) deptlen + 1);
  if (status = OCIDefineByPos(stmthp, &defnp, errhp,
             1, (dvoid *) dept, (sb4) deptlen+1,
             SQLT_STR, (dvoid *) 0, (ub2 *) 0,
             (ub2 *) 0, OCI_DEFAULT))
  {

#endif			
		}
		
		m_nof_cols = counter - 1;			
		
		if (m_hasResult)
		{
//			m_nof_rows = (std::size_t)PQntuples( m_lastresult);
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

bool TransactionExecStatemachine_oracle::hasResult()
{
	return m_hasResult && m_nof_rows > 0;
}

std::size_t TransactionExecStatemachine_oracle::nofColumns()
{
	if (m_state != Executed)
	{
		return errorStatus( std::string( "inspect command result not possible in state '") + stateName(m_state) + "'");
	}
	if (!m_lastresult)
	{
		return errorStatus( "command result is empty");
	}
	return m_nof_cols;
}

const char* TransactionExecStatemachine_oracle::columnName( std::size_t idx)
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
	if( idx >= m_nof_cols ) {
		errorStatus( std::string( "index of column out of range (") + boost::lexical_cast<std::string>(idx) + ")");
		return 0;
	}
	
	const char *rt = m_colDescr[idx].name.c_str( );
	return rt;
}

const DatabaseError* TransactionExecStatemachine_oracle::getLastError()
{
	return m_lasterror.get();
}

types::VariantConst TransactionExecStatemachine_oracle::get( std::size_t idx)
{
	if (m_state != Executed)
	{
		errorStatus( std::string( "inspect command result not possible in state '") + stateName(m_state) + "'");
		LOG_DATA << "[oracle statement] CALL get(" << idx << ") => NULL";
		return types::VariantConst();
	}
	if (!m_lastresult)
	{
		errorStatus( "command result is empty");
		LOG_DATA << "[oracle statement] CALL get(" << idx << ") => NULL";
		return types::VariantConst();
	}
	if (m_idx_row >= m_nof_rows) return types::VariantConst();
//	char* resval = PQgetvalue( m_lastresult, (int)m_idx_row, (int)idx-1);
	char *resval = "";
	if (!resval || resval[0] == '\0')
	{
//		if (PQgetisnull( m_lastresult, (int)m_idx_row, (int)idx-1))
		{
			return types::VariantConst();
		}
	}
	types::VariantConst rt( resval);
	LOG_DATA << "[oracle statement] CALL get(" << idx << ") => " << rt.typeName() << " '" << rt.tostring() << "'";
	return rt;
}

bool TransactionExecStatemachine_oracle::next()
{
	LOG_TRACE << "[oracle statement] CALL next()";
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


