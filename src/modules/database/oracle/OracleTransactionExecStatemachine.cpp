/************************************************************************

 Copyright (C) 2011 - 2014 Project Wolframe.
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
///\file OracleTransactionExecStatemachine.cpp

#include "OracleTransactionExecStatemachine.hpp"
#include "Oracle.hpp"
#include "OracleStatement.hpp"
#include "logger-v1.hpp"
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/replace.hpp>

using namespace _Wolframe;
using namespace _Wolframe::db;

TransactionExecStatemachine_oracle::TransactionExecStatemachine_oracle( OracleEnvirenment *env_, const std::string& name_, OracleDbUnit *dbUnit_)
	: TransactionExecStatemachine(name_)
	,m_state(Init)
	,m_env(env_)
	,m_lastresult(0)
	,m_nof_cols(0)
	,m_statement( new OracleStatement( m_env ) )
	,m_hasResult(false)
	,m_hasRow(false)
	,m_dbUnit(dbUnit_)
	,m_conn(0)
	{}

TransactionExecStatemachine_oracle::~TransactionExecStatemachine_oracle()
{
	if (m_conn) delete m_conn;
	clear();
}

void TransactionExecStatemachine_oracle::clear()
{
	if (m_lastresult)
	{
		(void)OCIHandleFree( m_lastresult, OCI_HTYPE_STMT );
		m_lastresult = 0;
		m_colDescr.clear( );
		m_nof_cols = 0;
	}
	m_lasterror.reset();
	m_statement->clear();
	m_state = Init;
	m_hasResult = false;
	m_hasRow = false;
}

static const char* getErrorType( sword errorcode )
{
	// TODO: find OCI error code list and map it
	switch( errorcode ) {
		case 1:
			return "CONSTRAINT";
		case 900:
		case 933:
			return "SYNTAX";
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
			(void)OCIErrorGet( (dvoid *)(*m_conn)->errhp, (ub4)1, (text *)NULL,
				&errcode, (text *)errmsg, (ub4)sizeof( errmsg ), OCI_HTYPE_ERROR );
			break;
	}
	
	const char *errtype = getErrorType( errcode );
	
	// TODO: map OCI codes to severity levels, so far everything is ERROR
	log::LogLevel::Level severity = log::LogLevel::LOGLEVEL_ERROR;
	
	m_lasterror.reset( new DatabaseError( severity, errcode, m_dbname.c_str(), m_statement->originalSQL().c_str(), errtype, errmsg, errmsg));
}

bool TransactionExecStatemachine_oracle::status( sword status_, State newstate )
{
	bool rt;

	if( status_ == OCI_SUCCESS || status_ == OCI_NO_DATA ) {
		m_state = newstate;
		rt = true;
	} else {
		setDatabaseErrorMessage( status_ );
		m_state = Error;
		rt = false;
	}
	
	return rt;
}

bool TransactionExecStatemachine_oracle::begin()
{
	LOG_TRACE << "[oracle statement] CALL begin()";
	if (m_state != Init)
	{
		return errorStatus( std::string( "call of begin not allowed in state '") + stateName(m_state) + "'");
	}
	if (m_conn) delete m_conn;
	m_conn = m_dbUnit->newConnection();
	static_cast<OracleStatement *>( m_statement )->setConnection( m_conn->object( ) );

	return status( OCITransStart( (*m_conn)->svchp, (*m_conn)->errhp, (uword)0, (ub4)OCI_TRANS_NEW ), Transaction );
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
	bool rt = status( OCITransCommit( (*m_conn)->svchp, (*m_conn)->errhp, OCI_DEFAULT ), Init );
	if (rt)
	{
		delete m_conn;
		m_conn = 0;
	}
	return rt;
}

bool TransactionExecStatemachine_oracle::rollback()
{
	LOG_TRACE << "[oracle statement] CALL rollback()";
	if (m_conn)
	{
		bool rt = status( OCITransRollback( (*m_conn)->svchp, (*m_conn)->errhp, OCI_DEFAULT ), Init );
		if (rt)
		{
			delete m_conn;
			m_conn = 0;
		}
		return rt;
	}
	return true;
}

bool TransactionExecStatemachine_oracle::errorStatus( const std::string& message)
{
	if (m_state != Error)
	{
		m_lasterror.reset( new DatabaseError( log::LogLevel::LOGLEVEL_ERROR, 0, m_dbname.c_str(), m_statement->originalSQL().c_str(), "INTERNAL", message.c_str(), "internal logic error"));
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
	m_statement->init( statement);

	if (m_lastresult)
	{
		(void)OCIHandleFree( m_lastresult, OCI_HTYPE_STMT );
		m_lastresult = 0;
	}

	sword status_ = OCIHandleAlloc( m_env->envhp, (dvoid **)&m_lastresult,
		OCI_HTYPE_STMT, (size_t)0, (dvoid **)0 );
	bool rt = status( status_, CommandReady );
	if( !rt ) return rt;

	// we must do it here otherwise bind doesn't know about the placeholders,
	// this also means we must NOT depend on anything when generating our
	// native SQL placeholders but the current index!
	m_statement->substitute( false );
	std::string stmstr = m_statement->nativeSQL();
	status_ = OCIStmtPrepare( m_lastresult, (*m_conn)->errhp, 
		(text *)const_cast<char *>( stmstr.c_str( ) ),
		(ub4)stmstr.length( ), (ub4)OCI_NTV_SYNTAX, (ub4)OCI_DEFAULT );
	rt = status( status_, CommandReady );
	if( !rt ) return rt;

	static_cast<OracleStatement *>( m_statement )->setStatement( m_lastresult );

	return true;
}

bool TransactionExecStatemachine_oracle::bind( std::size_t idx, const types::VariantConst& value)
{
	if (value.defined())
	{
		LOG_TRACE << "[oracle statement] CALL bind( " << idx << ", '" << value << "', " << value.typeName( ) << " )";
	}
	else
	{
		LOG_TRACE << "[oracle statement] CALL bind( " << idx << ", NULL)";
	}
	if (m_state != CommandReady && m_state != Executed)
	{
		return errorStatus( std::string( "call of bind not allowed in state '") + stateName(m_state) + "'");
	}

	try {
		m_statement->bind( idx, value );
		sword status_ = static_cast<OracleStatement *>( m_statement )->getLastStatus( );
		if( !status( status_, CommandReady ) ) return false;
	} catch( const std::runtime_error &e ) {
		return errorStatus( e.what( ) );
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

	m_statement->substitute( true );
	std::string stmstr = m_statement->nativeSQL();
	LOG_TRACE << "[oracle statement] CALL execute(" << stmstr << ")";
	
	// find out whether we have results in this statement
	// TODO: is there another/better way to do this?
	ub2 stmtTypehp;
	sword status_ = OCIAttrGet( m_lastresult, OCI_HTYPE_STMT,
		(dvoid *)&stmtTypehp, (ub4 *)0, (ub4)OCI_ATTR_STMT_TYPE,
		(*m_conn)->errhp );
	bool rt = status( status_, Executed );
	if( !rt ) return rt;

	m_hasResult = ( stmtTypehp == OCI_STMT_SELECT );

	status_ = OCIStmtExecute( (*m_conn)->svchp, m_lastresult, (*m_conn)->errhp, 
		(ub4)(m_hasResult) ? 0 : 1, (ub4)0,
		NULL, NULL, OCI_DEFAULT );

	rt = status( status_, Executed);
	if( rt && m_hasResult ) {
		ub4 counter = 1;
		OCIParam *paraDesc = 0;
		status_ = OCIParamGet( (dvoid *)m_lastresult, OCI_HTYPE_STMT,
			(*m_conn)->errhp, (dvoid **)&paraDesc, (ub4)counter );
		rt = status( status_, Executed );
		if( !rt ) return rt;
		
		while( status_ == OCI_SUCCESS ) {
			
			OracleColumnDescriptionPtr descrRef = OracleColumnDescriptionPtr( new OracleColumnDescription( ) );
			
			/* data type of the result column: used to allocate the
			 * correct size for the data container. could also be
			 * used later to create the best-fitting type of variant..
			 */
			status_ = OCIAttrGet( (dvoid *)paraDesc, (ub4)OCI_DTYPE_PARAM,
				(dvoid *)&descrRef->dataType, (ub4 *)0, (ub4)OCI_ATTR_DATA_TYPE,
				(*m_conn)->errhp );
			rt = status( status_, Executed );
			if( !rt ) return rt;

			// name of column
			ub4 colNameLen;
			char *colName;
			status_ = OCIAttrGet( (dvoid *)paraDesc, (ub4)OCI_DTYPE_PARAM,
				(dvoid *)&colName, (ub4 *)&colNameLen, (ub4)OCI_ATTR_NAME,
				(*m_conn)->errhp );
			rt = status( status_, Executed );
			if( !rt ) return rt;
			descrRef->name = std::string( colName, colNameLen );
			
			// determine how many bytes we have to allocate to hold the
			// data of this column in one row fetch
			int sizeInChars = 0;
			status_ = OCIAttrGet( (dvoid *)paraDesc, (ub4)OCI_DTYPE_PARAM,
				(dvoid *)&sizeInChars, (ub4 *)0, (ub4)OCI_ATTR_CHAR_USED,
				(*m_conn)->errhp );
			rt = status( status_, Executed );
			if( !rt ) return rt;
			
			ub2 col_width = (ub2)0;
			if( sizeInChars ) {
				status_ = OCIAttrGet( (dvoid *)paraDesc, (ub4)OCI_DTYPE_PARAM,
					(dvoid *)&col_width, (ub4 *)0, (ub4)OCI_ATTR_CHAR_SIZE,
					(*m_conn)->errhp );
			} else {
				status_ = OCIAttrGet( (dvoid *)paraDesc, (ub4)OCI_DTYPE_PARAM,
					(dvoid *)&col_width, (ub4 *)0, (ub4)OCI_ATTR_DATA_SIZE,
					(*m_conn)->errhp );
			}
			rt = status( status_, Executed );
			if( !rt ) return rt;

			// numbers have scales and precisions
			ub2 precision = (ub2)0;
			status_ = OCIAttrGet( (dvoid *)paraDesc, (ub4)OCI_DTYPE_PARAM,
				(dvoid *)&precision, (ub4 *)0, (ub4)OCI_ATTR_PRECISION,
				(*m_conn)->errhp );
			rt = status( status_, Executed );
			if( !rt ) return rt;

			sb1 scale = (sb1)0;
			status_ = OCIAttrGet( (dvoid *)paraDesc, (ub4)OCI_DTYPE_PARAM,
				(dvoid *)&scale, (ub4 *)0, (ub4)OCI_ATTR_SCALE,
				(*m_conn)->errhp );
			rt = status( status_, Executed );
			if( !rt ) return rt;
			
//			ub2 char_size = (ub2) ( (OCILib.nls_utf8 == TRUE) ? UTF8_BYTES_PER_CHAR : sizeof(dtext) );

			// set buffer size depending on data type of column
			switch( descrRef->dataType ) {
				case SQLT_CHR:
					descrRef->fetchType = SQLT_STR;
					// TODO: depends on NLS_LANG and some other things for sure
					descrRef->bufsize = ( col_width + 1 ) * 4;
					descrRef->len = ( col_width + 1 ) * 4;
					break;
					
				case SQLT_NUM:
					descrRef->fetchType = SQLT_VNU;
					descrRef->bufsize = sizeof( OCINumber );
					descrRef->len = sizeof( OCINumber );
					break;
					
				case SQLT_IBDOUBLE:
					descrRef->fetchType = SQLT_BDOUBLE;
					descrRef->bufsize = sizeof( double );
					descrRef->len = sizeof( double );
					break;
					
				default:
					errorStatus( std::string( "unknown data type '" + boost::lexical_cast<std::string>( descrRef->dataType ) + "' returned in statement '" + m_statement->originalSQL() + "'" ) );
					return false;
			}
			
			// allocate buffer for column and register it at the column position
			descrRef->buf = (char *)calloc( descrRef->bufsize, sizeof( char ) );
			descrRef->ind = 0;
			descrRef->len = 0;
			descrRef->errcode = 0;
			status_ = OCIDefineByPos( m_lastresult, &descrRef->defhp,
				(*m_conn)->errhp, counter, (dvoid *)descrRef->buf,
				(sb4)descrRef->bufsize, descrRef->fetchType,
				&descrRef->ind, &descrRef->len, &descrRef->errcode, OCI_DEFAULT );
			rt = status( status_, Executed );
			if( !rt ) return rt;

			// some character set tweaking, convert without loss and
			// set desired character set (one which doesn't get changed
			// by funny variables like NLS_LANG en passant)
			switch( descrRef->dataType ) {
				case SQLT_CHR: {
					ub1 cform = SQLCS_NCHAR;
					status_ = OCIAttrSet( (dvoid *)descrRef->defhp, (ub4)OCI_HTYPE_DEFINE,
						(void *)&cform, (ub4)0, (ub4)OCI_ATTR_CHARSET_FORM,
						(*m_conn)->errhp );
					rt = status( status_, Executed );
					if( !rt ) return rt;
					
					//~ ub2 csid = OCI_UTF16ID;
					ub2 csid = 871; // UTF8
					status_ = OCIAttrSet( (dvoid *)descrRef->defhp, (ub4)OCI_HTYPE_DEFINE,
						(void *)&csid, (ub4)0, (ub4)OCI_ATTR_CHARSET_ID,
						(*m_conn)->errhp );
					rt = status( status_, Executed );
					if( !rt ) return rt;

					break;
				}
			}
					
			LOG_TRACE << "Column " << counter << ", name: " << descrRef->name
				<< ", type: " << descrRef->dataType
				<< ", sizeInChars: " << sizeInChars
				<< ", precision: " << precision
				<< ", scale: " << scale
				<< ", size: " << descrRef->bufsize
				<< ", len: " << descrRef->len;

			m_colDescr.push_back( descrRef );
			
			// get next column descriptor (if there is any)
			counter++;
			status_ = OCIParamGet( (dvoid *)m_lastresult, OCI_HTYPE_STMT,
				(*m_conn)->errhp, (dvoid **)&paraDesc, (ub4)counter );
		}

		m_nof_cols = counter - 1;

		// fetch first row
		status_ = OCIStmtFetch( m_lastresult, (*m_conn)->errhp, (ub4)1,
			(ub2)OCI_FETCH_NEXT, (ub4)OCI_DEFAULT );
		rt = status( status_, Executed );
		if( !rt ) return rt;
		
		m_hasRow = ( status_ != OCI_NO_DATA );
		
	} else if( status_ == OCI_NO_DATA ) {
		m_hasRow = false;
	}
	
	return rt;
}

bool TransactionExecStatemachine_oracle::hasResult()
{
	return m_hasResult && m_hasRow;
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
	if( idx < 1 || idx > m_nof_cols ) {
		errorStatus( std::string( "index of column out of range (") + boost::lexical_cast<std::string>(idx) + ")");
		return 0;
	}
	
	const char *rt = m_colDescr[idx-1]->name.c_str( );
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
	if( idx < 1 || idx > m_nof_cols ) {
		errorStatus( std::string( "index of column out of range (") + boost::lexical_cast<std::string>(idx) + ")");
		return types::VariantConst();
	}
	if( !m_hasRow ) return types::VariantConst( );
	
	OracleColumnDescriptionPtr descrRef = m_colDescr[idx-1];

	LOG_TRACE << "Data "
		<< ", type: " << descrRef->dataType
		<< ", fetchType: " << descrRef->fetchType
		<< ", ind: " << descrRef->ind
		<< ", errcode: " << descrRef->errcode;

	if( descrRef->errcode == 0 ) {
		// ok case, handled below
	} else if( descrRef->errcode == 1405 ) {
		// check for NULL value
		LOG_DATA << "[oracle statement] CALL get(" << idx << ") => NULL";
		return types::VariantConst();
	} else if( descrRef->errcode == 1406 ) {
		// column got truncated, show a message on how
		if( descrRef->ind == -2 ) {
			errorStatus( std::string( "value of column (") + boost::lexical_cast<std::string>(idx) + ") got truncated, we don not know how badly..");
			return types::VariantConst();		
		} else if( descrRef->ind > 0 ) {
			errorStatus( std::string( "value of column (") + boost::lexical_cast<std::string>(idx) + ") got truncated, got only " + boost::lexical_cast<std::string>( descrRef->ind ) + " bytes, errcode: " + boost::lexical_cast<std::string>( descrRef->errcode ) );
			return types::VariantConst();		
		} else {
			errorStatus( std::string( "value of column (" ) + boost::lexical_cast<std::string>(idx) + ") indicated as truncated (OCI-1406), but indicator is ok?!" );
			return types::VariantConst();		
		}
	} else {
		errorStatus( std::string( "error " ) + boost::lexical_cast<std::string>( descrRef->errcode ) + " in column (" + boost::lexical_cast<std::string>(idx) + ")" );
		return types::VariantConst();		
	}

	types::VariantConst rt;
	
	switch( descrRef->dataType ) {
		case SQLT_CHR: {
			//~ LOG_DATA << "[Oracle get SQLT_CHR]: " << descrRef->buf;
			//~ for (int i = 0; i < descrRef->bufsize ; i++){
				//~ printf(" %2x", descrRef->buf[i]);
			//~ }
			//~ putchar( '\n' );
			// TODO: enforce Oracle returning UTF8 or UTF16 and then
			// convert it, the variant takes UTF8 (also on Windows?)
			rt = descrRef->buf;
			break;
		}
		
		case SQLT_NUM: {
			sword status_;
			sword isInt = 0;		
			status_ = OCINumberIsInt( (*m_conn)->errhp, (OCINumber *)descrRef->buf, &isInt );
			if( !status( status_, Executed ) ) {
				errorStatus( std::string( "error in OCINumberIsInt for SQLT_NUM, error " ) + boost::lexical_cast<std::string>( descrRef->errcode ) + " in column (" + boost::lexical_cast<std::string>(idx) + ")" );
				return types::VariantConst( );
			}
			if( !isInt ) {
				double doubleval = 0;
				status_ = OCINumberToReal( (*m_conn)->errhp, (OCINumber *)descrRef->buf,
					(ub4)sizeof( doubleval ), (void *)&doubleval );
				if( status( status_, Executed ) ) {
					rt = doubleval;
				} else {
					rt = types::VariantConst( );
				}
			} else {
				sword sign = 0;
				status_ = OCINumberSign( (*m_conn)->errhp, (OCINumber *)descrRef->buf, &sign );
				if( !status( status_, Executed ) ) {
					errorStatus( std::string( "error in OCINumberSign for SQLT_NUM, error " ) + boost::lexical_cast<std::string>( descrRef->errcode ) + " in column (" + boost::lexical_cast<std::string>(idx) + ")" );
					return types::VariantConst( );
				}

				if( sign < 0 ) {
					_WOLFRAME_INTEGER intval = 0;
					status_ = OCINumberToInt( (*m_conn)->errhp, (OCINumber *)descrRef->buf,
						(ub4)sizeof( _WOLFRAME_INTEGER ), (ub4)OCI_NUMBER_SIGNED, (void *)&intval );
					if( status( status_, Executed ) ) {
						//~ LOG_DATA << "[Oracle get SQLT_NUM(signed)]: " << intval;
						rt = (types::Variant::Data::Int)intval;
					} else {
						errorStatus( std::string( "error in OCINumberToInt(signed) for SQLT_NUM, error " ) + boost::lexical_cast<std::string>( descrRef->errcode ) + " in column (" + boost::lexical_cast<std::string>(idx) + ")" );
						rt = types::VariantConst( );
					}
				} else {
					_WOLFRAME_UINTEGER uintval = 0;
					status_ = OCINumberToInt( (*m_conn)->errhp, (OCINumber *)descrRef->buf,
						(ub4)sizeof( _WOLFRAME_UINTEGER ), (ub4)OCI_NUMBER_UNSIGNED, (void *)&uintval );
					if( status( status_, Executed ) ) {
						//~ LOG_DATA << "[Oracle get SQLT_NUM(unsigned)]: " << uintval;
						if( uintval <= (_WOLFRAME_UINTEGER)std::numeric_limits<_WOLFRAME_INTEGER>::max( ) ) {
							rt = (types::Variant::Data::Int)uintval;
						} else {
							rt = (types::Variant::Data::UInt)uintval;
						}
					} else {
						errorStatus( std::string( "error in OCINumberToInt(unsigned) for SQLT_NUM, error " ) + boost::lexical_cast<std::string>( descrRef->errcode ) + " in column (" + boost::lexical_cast<std::string>(idx) + ")" );
						rt = types::VariantConst( );
					}
				}
			}
			break;
		}
		
		case SQLT_IBDOUBLE: {
			rt = types::VariantConst( *((double *)descrRef->buf) );
			break;
		}
					
		default:
			errorStatus( std::string( "[Oracle get] unknown data type '" + boost::lexical_cast<std::string>( descrRef->dataType ) + "' returned in statement '" + m_statement->originalSQL() + "'" ) );
			return types::VariantConst();
	}

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
	
	if( !m_hasRow ) return false;

	sword status_ = OCIStmtFetch( m_lastresult, (*m_conn)->errhp, (ub4)1,
		(ub2)OCI_FETCH_NEXT, (ub4)OCI_DEFAULT );
	bool rt = status( status_, Executed );
	if( !rt ) return rt;
	m_hasRow = ( status_ != OCI_NO_DATA );

	if( m_hasRow ) return true;
	return false;
}

const std::string& TransactionExecStatemachine_oracle::databaseID() const
{
	return m_dbUnit->ID();
}
