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
//
// Wolframe Oracle client view implementation
//

#include "logger-v1.hpp"
#include "Oracle.hpp"
#include "utils/fileUtils.hpp"
#include "OracleTransactionExecStatemachine.hpp"
#include <string>
#include <sstream>

#include <boost/filesystem.hpp>

namespace _Wolframe {
namespace db {

//***  Oracle database functions  ***************************************
static std::string escConnElement( std::string element )
{
	std::string esc;
	for ( std::string::const_iterator it = element.begin(); it != element.end(); it++ )	{
		if ( *it == '\'' )
			esc += "\\'";
		else if ( *it == '\\' )
			esc += "\\\\";
		else
			esc += *it;
	}
	return esc;
}

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
		<< "(CONNECT_DATA=(SID=" << escConnElement( dbName ) << ")"
		<< "))";

	return ss.str( );
}

OracleDatabase::OracleDatabase( const OracleConfig& config)
	:m_ID(config.ID())
	,m_connections(0)
	,m_connPool(config.acquireTimeout())
{
	init( config);
}

OracleDatabase::OracleDatabase( const std::string& id_,
			  const std::string& host_, unsigned short port_, const std::string& dbName_,
			  const std::string& user_, const std::string& password_,
			  size_t connections_, unsigned short acquireTimeout_)
	:m_ID(id_)
	,m_connections(0)
	,m_connPool(acquireTimeout_)
{
	OracleConfig config( id_, host_, port_, dbName_, user_, password_,
		connections_, acquireTimeout_);
	init( config);
}		  
			
void OracleDatabase::init( const OracleConfig& config)
{
	int connections = config.connections();

	m_connStr = buildConnStr( config.host(), config.port(), config.dbName() );
	LOG_DATA << "Oracle database '" << m_ID << "' connection string '" << m_connStr << "'";

	sword status;
	
	// create an Oracle OCI environment (global per process), what
	// options do we really need (charset, mutex, threading, pooling)?
	status = OCIEnvCreate( &( m_env.envhp ), OCI_THREADED, (dvoid *)0,
		0, 0, 0, 0, (dvoid **)0 );
	if( status != OCI_SUCCESS ) {
		LOG_ALERT << "Failed to create Oracle environment for database '" << m_ID << "'";
		throw std::runtime_error( "Fatal error creating an Oracle environment" );
	}
	
	for( unsigned short i = 0; i < connections; i++ ) {
		OracleConnection *conn = new OracleConnection( );

		// a server handle
		status = OCIHandleAlloc( m_env.envhp, (dvoid **)&conn->srvhp, OCI_HTYPE_SERVER, (size_t)0, (dvoid **)0 );
		if( status != OCI_SUCCESS ) {
			LOG_ALERT << "Can't allocate OCI server handle for database '" << m_ID << "'";
			throw std::runtime_error( "Can't allocate OCI server handle for Oracle database" );
		}

		// an error handle
		status = OCIHandleAlloc( m_env.envhp, (dvoid **)&conn->errhp, OCI_HTYPE_ERROR, (size_t)0, (dvoid **)0 );
		if( status != OCI_SUCCESS ) {
			LOG_ALERT << "Can't allocate OCI error handle for database '" << m_ID << "'";
			OCIHandleFree( conn->srvhp, OCI_HTYPE_SERVER );
			throw std::runtime_error( "Can't allocate OCI error handle for Oracle database" );
		}

		// a service context handle
		status = OCIHandleAlloc( m_env.envhp, (dvoid **)&conn->svchp, OCI_HTYPE_SVCCTX, (size_t)0, (dvoid **)0 );
		if( status != OCI_SUCCESS ) {
			LOG_ALERT << "Can't allocate OCI service context handle for database '" << m_ID << "'";
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
			LOG_ALERT << "Can't attach to Oracle server for database '" << m_ID << "'";
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
			LOG_ALERT << "Can't attach to Oracle server for database '" << m_ID << "'";
			OCIServerDetach( conn->srvhp, conn->errhp, OCI_DEFAULT );
			OCIHandleFree( conn->svchp, OCI_HTYPE_SVCCTX );
			OCIHandleFree( conn->errhp, OCI_HTYPE_ERROR );
			OCIHandleFree( conn->srvhp, OCI_HTYPE_SERVER );
			throw std::runtime_error( "Can't connect to Oracle server" );
		}
		
		// a user session handle
		status = OCIHandleAlloc( m_env.envhp, (dvoid **)&conn->authp,
			OCI_HTYPE_SESSION, (size_t)0, (dvoid **)0 );
		if( status != OCI_SUCCESS ) {
			LOG_ALERT << "Can't create handle for Oracle authentication credentials for database '" << m_ID << "'";
			OCIServerDetach( conn->srvhp, conn->errhp, OCI_DEFAULT );
			OCIHandleFree( conn->svchp, OCI_HTYPE_SVCCTX );
			OCIHandleFree( conn->errhp, OCI_HTYPE_ERROR );
			OCIHandleFree( conn->srvhp, OCI_HTYPE_SERVER );
			throw std::runtime_error( "Can't create handle for authentication credentials for Oracle database" );
		}
		
		// user and password credentials (TODO: others, charsets)
		status = OCIAttrSet( conn->authp, OCI_HTYPE_SESSION,
			(dvoid *)const_cast<char *>( config.user( ).c_str( ) ), (ub4)config.user( ).length( ),
			OCI_ATTR_USERNAME, conn->errhp );
		if( status != OCI_SUCCESS ) {
			LOG_ALERT << "Can't create handle for username for Oracle database '" << m_ID << "'";
			OCIServerDetach( conn->srvhp, conn->errhp, OCI_DEFAULT );
			OCIHandleFree( conn->svchp, OCI_HTYPE_SVCCTX );
			OCIHandleFree( conn->errhp, OCI_HTYPE_ERROR );
			OCIHandleFree( conn->srvhp, OCI_HTYPE_SERVER );
			throw std::runtime_error( "Can't create handle for username for Oracle database" );
		}
		status = OCIAttrSet( conn->authp, OCI_HTYPE_SESSION,
			(dvoid *)const_cast<char *>( config.password( ).c_str( ) ), (ub4)config.password( ).length( ),
			OCI_ATTR_PASSWORD, conn->errhp );
		if( status != OCI_SUCCESS ) {
			LOG_ALERT << "Can't create handle for username for Oracle database '" << m_ID << "'";
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
			LOG_ALERT << "Can't create user session for Oracle database '" << m_ID << "'";
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
			LOG_ALERT << "Can't set user session in service context for Oracle database '" << m_ID << "'";
			OCIServerDetach( conn->srvhp, conn->errhp, OCI_DEFAULT );
			OCIHandleFree( conn->authp, OCI_HTYPE_SESSION );
			OCIHandleFree( conn->svchp, OCI_HTYPE_SVCCTX );
			OCIHandleFree( conn->errhp, OCI_HTYPE_ERROR );
			OCIHandleFree( conn->srvhp, OCI_HTYPE_SERVER );
			throw std::runtime_error( "Can't create set user session in service context for Oracle database" );
		}

		// allocate and set transaction handle
		status = OCIHandleAlloc( m_env.envhp, (dvoid **)&conn->transhp,
			OCI_HTYPE_TRANS, (size_t)0, (dvoid **)0 );
		if( status != OCI_SUCCESS ) {
			LOG_ALERT << "Can't create transaction handle for Oracle database '" << m_ID << "'";
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
			LOG_ALERT << "Can't set transaction handle in service context for Oracle database '" << m_ID << "'";
			OCIServerDetach( conn->srvhp, conn->errhp, OCI_DEFAULT );
			OCIHandleFree( conn->authp, OCI_HTYPE_SESSION );
			OCIHandleFree( conn->svchp, OCI_HTYPE_SVCCTX );
			OCIHandleFree( conn->errhp, OCI_HTYPE_ERROR );
			OCIHandleFree( conn->srvhp, OCI_HTYPE_SERVER );
			throw std::runtime_error( "Can't  transaction handle in service context for Oracle database" );
		}
		
		// add connection to pool of connections
		m_connPool.add( conn );
		m_connections++;
	}
	LOG_DEBUG << "Oracle database '" << m_ID << "' created with a pool of " << m_connections << " connections";
}

OracleDatabase::~OracleDatabase()
{
	size_t connections = 0;
	bool hasErrors = false;

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
				
		m_connections--, connections++;
	}
	
	if( hasErrors ) {
		LOG_ALERT << "Oracle database '" << m_ID << "' destructor: NULL connection from pool";
		throw std::logic_error( "Oracle database destructor: NULL connection from pool" );
	}

	if ( m_connections != 0 )	{
		LOG_ALERT << "Oracle database '" << m_ID << "' destructor: "
			      << m_connections << " connections not destroyed";
		throw std::logic_error( "Oracle database destructor: not all connections destroyed" );
	}

	(void)OCIHandleFree( (dvoid *)m_env.envhp, OCI_HTYPE_ENV );
	
	LOG_TRACE << "Oracle database '" << m_ID << "' destroyed, " << connections << " connections destroyed";
}

}} // _Wolframe::db

