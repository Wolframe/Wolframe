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
//
// Wolframe Oracle client view implementation
//

#include "logger-v1.hpp"
#include "logger/logObject.hpp"
#include "Oracle.hpp"
#include "utils/fileUtils.hpp"
#include "OracletransactionExecStatemachine.hpp"
#include <string>
#include <sstream>

#include <boost/filesystem.hpp>

namespace _Wolframe {
namespace db {

//***  Oracle database functions  ***************************************

static std::string buildConnStr( const std::string& host, unsigned short port, const std::string& dbName )
{
	std::stringstream ss;
	
	// everything empty, assume default host (whatever that is)
	if( host.empty( ) && dbName.empty( ) ) {
		return "";
	}

	// only a dbName, assume this is a dblink in tnsnames.ora
	if( host.empty( ) && !dbName.empty( ) ) {
		return dbName;
	}
	
	// assume default Oracle listener port, if port is undefined
	if( port == 0 ) port = 1521;

	// otherwise compose a connection string
	// TODO: needs improvement!
	ss << "(DESCRIPTION=(ADDRESS=(PROTOCOL=TCP)"
		<< "(HOST=" << host << ")"
		<< "(PORT=" << port << "))"
		<< "(CONNECT_DATA=(SID=" << dbName << ")"
		<< "))";

	return ss.str( );
}

OracledbUnit::OracledbUnit(const std::string& id,
				    const std::string& host, unsigned short port,
				    const std::string& dbName,
				    const std::string& user, const std::string& password,
				    std::string sslMode, std::string sslCert, std::string sslKey,
				    std::string sslRootCert, std::string sslCRL ,
				    unsigned short connectTimeout,
				    size_t connections, unsigned short acquireTimeout,
				    unsigned statementTimeout,
				    const std::list<std::string>& programFiles_)
	: m_ID( id ), m_noConnections( 0 ), m_connPool( acquireTimeout ),
	  m_statementTimeout( statementTimeout ), m_programFiles(programFiles_)
{
	m_connStr = buildConnStr( host, port,  dbName );
	MOD_LOG_DATA << "Oracle database '" << m_ID << "' connection string '" << m_connStr << "'";

	sword status;
	
	// create an Oracle OCI environment (global per process), what
	// options do we really need (charset, mutex, threading, pooling)?
	status = OCIEnvCreate( &( m_db.m_env.envhp ), OCI_THREADED, (dvoid *)0,
		0, 0, 0, 0, (dvoid **)0 );
	if( status != OCI_SUCCESS ) {
		MOD_LOG_ALERT << "Failed to create Oracle environment for database '" << m_ID << "'";
		throw std::runtime_error( "Fatal error creating an Oracle environment" );
	}
	
	for( size_t i = 0; i < connections; i++ ) {
		OracleConnection *conn = new OracleConnection( );

		// a server handle
		status = OCIHandleAlloc( m_db.m_env.envhp, (dvoid **)&conn->srvhp, OCI_HTYPE_SERVER, (size_t)0, (dvoid **)0 );
		if( status != OCI_SUCCESS ) {
			MOD_LOG_ALERT << "Can't allocate OCI server handle for database '" << m_ID << "'";
			throw std::runtime_error( "Can't allocate OCI server handle for Oracle database" );
		}

		// an error handle
		status = OCIHandleAlloc( m_db.m_env.envhp, (dvoid **)&conn->errhp, OCI_HTYPE_ERROR, (size_t)0, (dvoid **)0 );
		if( status != OCI_SUCCESS ) {
			MOD_LOG_ALERT << "Can't allocate OCI error handle for database '" << m_ID << "'";
			OCIHandleFree( conn->srvhp, OCI_HTYPE_SERVER );
			throw std::runtime_error( "Can't allocate OCI error handle for Oracle database" );
		}

		// a service context handle
		status = OCIHandleAlloc( m_db.m_env.envhp, (dvoid **)&conn->svchp, OCI_HTYPE_SVCCTX, (size_t)0, (dvoid **)0 );
		if( status != OCI_SUCCESS ) {
			MOD_LOG_ALERT << "Can't allocate OCI service context handle for database '" << m_ID << "'";
			OCIHandleFree( conn->errhp, OCI_HTYPE_ERROR );
			OCIHandleFree( conn->srvhp, OCI_HTYPE_SERVER );
			throw std::runtime_error( "Can't allocate OCI service context handle for Oracle database" );
		}

		// attach to server
		status = OCIServerAttach( conn->srvhp, conn->errhp,
			m_connStr.empty( ) ? NULL : (CONST text *)( m_connStr.c_str( ) ),
			m_connStr.empty( ) ? (sb4)0 : (sb4)( m_connStr.length( ) ),
			OCI_DEFAULT );
		if( status != OCI_SUCCESS ) {
			MOD_LOG_ALERT << "Can't attach to Oracle server for database '" << m_ID << "'";
			OCIHandleFree( conn->svchp, OCI_HTYPE_SVCCTX );
			OCIHandleFree( conn->errhp, OCI_HTYPE_ERROR );
			OCIHandleFree( conn->srvhp, OCI_HTYPE_SERVER );
			throw std::runtime_error( "Can't attach to Oracle server for Oracle database" );
		}

		/* set attribute server context in the service context */
		status = OCIAttrSet( conn->svchp, OCI_HTYPE_SVCCTX,
			conn->srvhp, (ub4)0, OCI_ATTR_SERVER,
			(OCIError *)conn->errhp );		
		if( status != OCI_SUCCESS ) {
			MOD_LOG_ALERT << "Can't attach to Oracle server for database '" << m_ID << "'";
			OCIServerDetach( conn->srvhp, conn->errhp, OCI_DEFAULT );
			OCIHandleFree( conn->svchp, OCI_HTYPE_SVCCTX );
			OCIHandleFree( conn->errhp, OCI_HTYPE_ERROR );
			OCIHandleFree( conn->srvhp, OCI_HTYPE_SERVER );
			throw std::runtime_error( "Can't connect to Oracle server" );
		}
		
		// a user session handle
		status = OCIHandleAlloc( m_db.m_env.envhp, (dvoid **)&conn->authp,
			OCI_HTYPE_SESSION, (size_t)0, (dvoid **)0 );
		if( status != OCI_SUCCESS ) {
			MOD_LOG_ALERT << "Can't create handle for Oracle authentication credentials for database '" << m_ID << "'";
			OCIServerDetach( conn->srvhp, conn->errhp, OCI_DEFAULT );
			OCIHandleFree( conn->svchp, OCI_HTYPE_SVCCTX );
			OCIHandleFree( conn->errhp, OCI_HTYPE_ERROR );
			OCIHandleFree( conn->srvhp, OCI_HTYPE_SERVER );
			throw std::runtime_error( "Can't create handle for authentication credentials for Oracle database" );
		}
		
		// user and password credentials (TODO: others, charsets)
		status = OCIAttrSet( conn->authp, OCI_HTYPE_SESSION,
			(dvoid *)const_cast<char *>( user.c_str( ) ), (ub4)user.length( ),
			OCI_ATTR_USERNAME, conn->errhp );
		if( status != OCI_SUCCESS ) {
			MOD_LOG_ALERT << "Can't create handle for username for Oracle database '" << m_ID << "'";
			OCIServerDetach( conn->srvhp, conn->errhp, OCI_DEFAULT );
			OCIHandleFree( conn->svchp, OCI_HTYPE_SVCCTX );
			OCIHandleFree( conn->errhp, OCI_HTYPE_ERROR );
			OCIHandleFree( conn->srvhp, OCI_HTYPE_SERVER );
			throw std::runtime_error( "Can't create handle for username for Oracle database" );
		}
		status = OCIAttrSet( conn->authp, OCI_HTYPE_SESSION,
			(dvoid *)const_cast<char *>( password.c_str( ) ), (ub4)password.length( ),
			OCI_ATTR_PASSWORD, conn->errhp );
		if( status != OCI_SUCCESS ) {
			MOD_LOG_ALERT << "Can't create handle for username for Oracle database '" << m_ID << "'";
			OCIServerDetach( conn->srvhp, conn->errhp, OCI_DEFAULT );
			OCIHandleFree( conn->svchp, OCI_HTYPE_SVCCTX );
			OCIHandleFree( conn->errhp, OCI_HTYPE_ERROR );
			OCIHandleFree( conn->srvhp, OCI_HTYPE_SERVER );
			throw std::runtime_error( "Can't create handle for username for Oracle database" );
		}

		// open user session
		status = OCISessionBegin( conn->svchp, conn->errhp, conn->authp,
			OCI_CRED_RDBMS, OCI_DEFAULT );
		if( status != OCI_SUCCESS ) {
			MOD_LOG_ALERT << "Can't create user session for Oracle database '" << m_ID << "'";
			OCIServerDetach( conn->srvhp, conn->errhp, OCI_DEFAULT );
			OCIHandleFree( conn->authp, OCI_HTYPE_SESSION );
			OCIHandleFree( conn->svchp, OCI_HTYPE_SVCCTX );
			OCIHandleFree( conn->errhp, OCI_HTYPE_ERROR );
			OCIHandleFree( conn->srvhp, OCI_HTYPE_SERVER );
			throw std::runtime_error( "Can't create user session for Oracle database" );
		}

		// set user session in service context
		status = OCIAttrSet( conn->svchp, OCI_HTYPE_SVCCTX,
			conn->authp, (ub4)0, OCI_ATTR_SESSION, conn->errhp );
		if( status != OCI_SUCCESS ) {
			MOD_LOG_ALERT << "Can't set user session in service context for Oracle database '" << m_ID << "'";
			OCIServerDetach( conn->srvhp, conn->errhp, OCI_DEFAULT );
			OCIHandleFree( conn->authp, OCI_HTYPE_SESSION );
			OCIHandleFree( conn->svchp, OCI_HTYPE_SVCCTX );
			OCIHandleFree( conn->errhp, OCI_HTYPE_ERROR );
			OCIHandleFree( conn->srvhp, OCI_HTYPE_SERVER );
			throw std::runtime_error( "Can't create set user session in service context for Oracle database" );
		}

		// allocate and set transaction handle
		status = OCIHandleAlloc( m_db.m_env.envhp, (dvoid **)&conn->transhp,
			OCI_HTYPE_TRANS, (size_t)0, (dvoid **)0 );
		if( status != OCI_SUCCESS ) {
			MOD_LOG_ALERT << "Can't create transaction handle for Oracle database '" << m_ID << "'";
			OCIServerDetach( conn->srvhp, conn->errhp, OCI_DEFAULT );
			OCIHandleFree( conn->authp, OCI_HTYPE_SESSION );
			OCIHandleFree( conn->svchp, OCI_HTYPE_SVCCTX );
			OCIHandleFree( conn->errhp, OCI_HTYPE_ERROR );
			OCIHandleFree( conn->srvhp, OCI_HTYPE_SERVER );
			throw std::runtime_error( "Can't create transaction handle for Oracle database" );
		}
		status = OCIAttrSet( conn->svchp, OCI_HTYPE_SVCCTX,
			conn->transhp, (ub4)0, OCI_ATTR_TRANS, conn->errhp );
		if( status != OCI_SUCCESS ) {
			MOD_LOG_ALERT << "Can't set transaction handle in service context for Oracle database '" << m_ID << "'";
			OCIServerDetach( conn->srvhp, conn->errhp, OCI_DEFAULT );
			OCIHandleFree( conn->authp, OCI_HTYPE_SESSION );
			OCIHandleFree( conn->svchp, OCI_HTYPE_SVCCTX );
			OCIHandleFree( conn->errhp, OCI_HTYPE_ERROR );
			OCIHandleFree( conn->srvhp, OCI_HTYPE_SERVER );
			throw std::runtime_error( "Can't  transaction handle in service context for Oracle database" );
		}
		
		// add connection to pool of connections
		m_connPool.add( conn );
		m_noConnections++;
	}

	m_db.setUnit( this );

	MOD_LOG_DEBUG << "Oracle database '" << m_ID << "' created with a pool of " << m_noConnections << " connections";
}


// This function needs a lot of work and thinking...
OracledbUnit::~OracledbUnit()
{
	size_t connections = 0;
	bool hasErrors = false;

	m_db.setUnit( NULL );
	m_connPool.timeout( 3 );

	while( m_connPool.available( ) ) {
		OracleConnection *conn = m_connPool.get( );
		if( conn == NULL ) {
			hasErrors = true;
		}

		// intentionally ignoring errors here, we try hard to close everything
		// and not to leave things leaking in the Oracle database
		(void)OCISessionEnd( conn->svchp, conn->errhp, conn->authp, OCI_DEFAULT );
		(void)OCIServerDetach( conn->srvhp, conn->errhp, OCI_DEFAULT );
		(void)OCIHandleFree( conn->transhp, OCI_HTYPE_TRANS );
		(void)OCIHandleFree( conn->authp, OCI_HTYPE_SESSION );
		(void)OCIHandleFree( conn->svchp, OCI_HTYPE_SVCCTX );
		(void)OCIHandleFree( conn->errhp, OCI_HTYPE_ERROR );
		(void)OCIHandleFree( conn->srvhp, OCI_HTYPE_SERVER );
				
		m_noConnections--, connections++;
	}
	
	if( hasErrors ) {
		MOD_LOG_ALERT << "Oracle database '" << m_ID << "' destructor: NULL connection from pool";
		throw std::logic_error( "Oracle database destructor: NULL connection from pool" );
	}

	if ( m_noConnections != 0 )	{
		MOD_LOG_ALERT << "Oracle database unit '" << m_ID << "' destructor: "
			      << m_noConnections << " connections not destroyed";
		throw std::logic_error( "Oracle database unit destructor: not all connections destroyed" );
	}

	(void)OCIHandleFree( (dvoid *)m_db.m_env.envhp, OCI_HTYPE_ENV );
	
	MOD_LOG_TRACE << "Oracle database unit '" << m_ID << "' destroyed, " << connections << " connections destroyed";
}

void OracledbUnit::loadProgram( const std::string& filename )
{
	// No program file, do nothing
	if ( filename.empty())
		return;
	if ( !boost::filesystem::exists( filename ))	{
		MOD_LOG_ALERT << "Program file '" << filename
			      << "' does not exist (Oracle database '" << m_ID << "')";
		return;
	}
	try
	{
		addProgram( utils::readSourceFileContent( filename));
	}
	catch (const std::runtime_error& e)
	{
		throw std::runtime_error( std::string("error in program '") + utils::getFileStem(filename) + "':" + e.what());
	}
}

void OracledbUnit::loadAllPrograms()
{
	std::list<std::string>::const_iterator pi = m_programFiles.begin(), pe = m_programFiles.end();
	for (; pi != pe; ++pi)
	{
		MOD_LOG_DEBUG << "Load Program '" << *pi << "' for Oracle database unit '" << m_ID << "'";
		loadProgram( *pi);
	}
	MOD_LOG_DEBUG << "Programs for Oracle database unit '" << m_ID << "' loaded";
}


Database* OracledbUnit::database()
{
	return m_db.hasUnit() ? &m_db : NULL;
}



/*****  Oracle database  ******************************************/
const std::string& Oracledatabase::ID() const
{
	if ( m_unit )
		return m_unit->ID();
	else
		throw std::runtime_error( "Oracle database unit not initialized" );
}

void Oracledatabase::loadProgram( const std::string& filename )
{
	if ( !m_unit )
		throw std::runtime_error( "loadProgram: Oracle database unit not initialized" );
	m_unit->loadProgram( filename );
}

void Oracledatabase::loadAllPrograms()
{
	if ( !m_unit )
		throw std::runtime_error( "loadAllPrograms: Oracle database unit not initialized" );
	m_unit->loadAllPrograms();
}

void Oracledatabase::addProgram( const std::string& program )
{
	if ( !m_unit )
		throw std::runtime_error( "addProgram: Oracle database unit not initialized" );
	m_unit->addProgram( program );
}

Transaction* Oracledatabase::transaction( const std::string& name)
{
	return new Oracletransaction( *this, name);
}

void Oracledatabase::closeTransaction( Transaction *t )
{
	delete t;
}

/*****  Oracle transaction  ***************************************/
Oracletransaction::Oracletransaction( Oracledatabase& database, const std::string& name_)
	: m_db( database ), m_unit( database.dbUnit() ), m_name(name_), m_conn( 0)
{}

Oracletransaction::~Oracletransaction()
{
	if (m_conn) delete m_conn;
}

const std::string& Oracletransaction::databaseID() const
{
	return m_unit.ID();
}

std::string Oracletransaction::getErrorMsg( sword status )
{
	sb4 errcode = 0;
	std::ostringstream os;
	text errbuf[512];
	
	switch( status ) {
		case OCI_SUCCESS:
			os << "OCI_SUCCESS";
			break;

		case OCI_SUCCESS_WITH_INFO:
			os << "OCI_SUCCESS_WITH_INFO";
			break;
		
		case OCI_NEED_DATA:
			os << "OCI_NEED_DATA";
			break;
		
		case OCI_NO_DATA:
			os << "OCI_NO_DATA";
			break;
		
		case OCI_INVALID_HANDLE:
			os << "OCI_INVALID_HANDLE";
			break;
		
		case OCI_STILL_EXECUTING:
			os << "OCI_STILL_EXECUTING";
			break;
		
		case OCI_CONTINUE:
			os << "OCI_CONTINUE";
			break;
			
		case OCI_ERROR:
			(void)OCIErrorGet( (dvoid *)(*m_conn)->errhp, (ub4)1, (text *)NULL,
				&errcode, errbuf, (ub4)sizeof( errbuf ), OCI_HTYPE_ERROR );
			os << errbuf;
			break;
	}
	return os.str( );
}

void Oracletransaction::begin()
{
	if (m_conn) delete m_conn;
	m_conn = new PoolObject<OracleConnection *>( m_unit.m_connPool);

	MOD_LOG_TRACE << "Oracle Begin Transaction";
		
	sword status;
        status = OCITransStart( (*m_conn)->svchp, (*m_conn)->errhp, (uword)0, (ub4)OCI_TRANS_NEW );
	if( status != OCI_SUCCESS ) {
		std::ostringstream os;
		os << "Failed to call OCITransStart to start transaction: " << getErrorMsg( status );
		MOD_LOG_ALERT << os.str( );
		delete m_conn;
		m_conn = 0;
		throw std::runtime_error( os.str( ) );
	}
}

void Oracletransaction::commit()
{
	MOD_LOG_TRACE << "Oracle Commit";

	if (!m_conn) throw std::runtime_error( "Called commit without transaction context");
	
	sword status;
	status = OCITransCommit((*m_conn)->svchp, (*m_conn)->errhp, (ub4)OCI_DEFAULT );
	if( status != OCI_SUCCESS ) {
		std::ostringstream os;
		os << "Failed to call OCITransCommit to commit transaction: " << getErrorMsg( status );
		MOD_LOG_ALERT << os.str( );
		delete m_conn;
		m_conn = 0;
		throw std::runtime_error( os.str( ) );
	}
	delete m_conn;
	m_conn = 0;
}

void Oracletransaction::rollback()
{
	MOD_LOG_TRACE << "Oracle Rollback";
	
	if (!m_conn) throw std::runtime_error( "Called rollback without transaction context");
	
	sword status;
	status = OCITransRollback((*m_conn)->svchp, (*m_conn)->errhp, (ub4)OCI_DEFAULT );
	if( status != OCI_SUCCESS ) {
		std::ostringstream os;
		os << "Failed to call OCITransRollback to abort transaction: " << getErrorMsg( status );
		MOD_LOG_ALERT << os.str( );
		delete m_conn;
		m_conn = 0;
		throw std::runtime_error( os.str( ) );
	}
	delete m_conn;
	m_conn = 0;
}

void Oracletransaction::execute_as_transaction()
{
	try
	{
		PoolObject<OracleConnection*> conn( m_unit.m_connPool);
		TransactionExecStatemachine_oracle ph( *conn);
		try
		{
			if (!ph.begin()
			||  !ph.doTransaction( m_input, m_output)
			||  !ph.commit())
			{
				const db::DatabaseError* err = ph.getLastError();
				if (!err) throw std::runtime_error( "unknown database error");
				throw db::DatabaseErrorException( *err);
			}
		}
		catch (const db::DatabaseErrorException& e)
		{
			ph.rollback();
			throw db::DatabaseTransactionErrorException( db::DatabaseTransactionError( m_name, e));
		}
		catch (const std::runtime_error& e)
		{
			ph.rollback();
			throw std::runtime_error( std::string("transaction '") + m_name + "'failed: " + e.what());
		}
	}
	catch ( _Wolframe::ObjectPoolTimeout )
	{
		throw std::runtime_error("timeout in database connection pool object allocation");
	}
}

void Oracletransaction::execute_as_operation()
{
	TransactionExecStatemachine_oracle ph( **m_conn, true);
	try
	{
		if (!ph.doTransaction( m_input, m_output))
		{
			const db::DatabaseError* err = ph.getLastError();
			if (!err) throw std::runtime_error( "unknown database error");
			throw db::DatabaseErrorException( *err);
		}
	}
	catch (const db::DatabaseErrorException& e)
	{
		ph.rollback();
		throw db::DatabaseTransactionErrorException( db::DatabaseTransactionError( m_name, e));
	}
	catch (const std::runtime_error& e)
	{
		ph.rollback();
		throw std::runtime_error( std::string("transaction operation failed: ") + e.what());
	}
}

void Oracletransaction::execute()
{
	if (m_conn)
	{
		execute_as_operation();
	}
	else
	{
		execute_as_transaction();
	}
}

void Oracletransaction::close()
{
	if (m_conn) {
		MOD_LOG_ERROR << "closed transaction without 'begin' or rollback";
	}
	delete m_conn;
	m_conn = 0;
	m_db.closeTransaction( this );
}

}} // _Wolframe::db

