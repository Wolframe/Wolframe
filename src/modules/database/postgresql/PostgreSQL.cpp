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
//
// Wolframe PostgreSQL client view implementation
//

#include "logger-v1.hpp"
#include "PostgreSQL.hpp"

#include <string>
#include <sstream>

#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

namespace _Wolframe {
namespace db {

//***  PostgreSQL database functions  ***************************************
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

static std::string buildConnStr( const std::string& host, unsigned short port, const std::string& dbName,
				 const std::string& user, const std::string& password,
				 const std::string& sslMode, const std::string& sslCert, const std::string& sslKey,
				 std::string& sslRootCert, std::string& sslCRL,
				 unsigned short connectTimeout )
{
	std::stringstream ss;

	if ( ! host.empty())
		ss << "host = '" << host << "'";
	if ( port != 0 )	{
		if ( ! ss.str().empty())
			ss << " ";
		ss << "port = " << port;
	}
	if ( ! dbName.empty())	{
		if ( ! ss.str().empty())
			ss << " ";
		ss << "dbname = '" << escConnElement( dbName ) << "'";
	}
	if ( ! user.empty())	{
		if ( ! ss.str().empty())
			ss << " ";
		ss << "user = '" << escConnElement( user ) << "'";
		if ( ! password.empty())	{
			if ( ! ss.str().empty())
				ss << " ";
			ss << "password = '" << escConnElement( password ) << "'";
		}
	}
	if ( ! sslMode.empty())	{
		if ( ! ss.str().empty())
			ss << " ";
		ss << "sslmode = '" << escConnElement( sslMode ) << "'";
	}
	if ( ! sslCert.empty())	{
		if ( ! ss.str().empty())
			ss << " ";
		ss << "sslcert = '" << escConnElement( sslCert ) << "'";
		if ( ! sslKey.empty())	{
			if ( ! ss.str().empty())
				ss << " ";
			ss << "sslkey = '" << escConnElement( sslKey ) << "'";
		}
	}
	if ( ! sslRootCert.empty())	{
		if ( ! ss.str().empty())
			ss << " ";
		ss << "sslrootcert = '" << escConnElement( sslRootCert ) << "'";
	}
	if ( ! sslCRL.empty())	{
		if ( ! ss.str().empty())
			ss << " ";
		ss << "sslcrl = '" << escConnElement( sslCRL ) << "'";
	}
	if ( connectTimeout != 0 )	{
		if ( ! ss.str().empty())
			ss << " ";
		ss << "connect_timeout = " << connectTimeout;
	}
	return ss.str();
}


// This function also needs a lot of work
PostgreSQLdbUnit::PostgreSQLdbUnit( const std::string& id,
				    const std::string& host, unsigned short port,
				    const std::string& dbName,
				    const std::string& user, const std::string& password,
				    std::string sslMode, std::string sslCert, std::string sslKey,
				    std::string sslRootCert, std::string sslCRL ,
				    unsigned short connectTimeout,
				    size_t connections, unsigned short acquireTimeout,
				    unsigned statementTimeout,
				    const std::string &programFile )
	: m_ID( id ), m_noConnections( 0 ), m_connPool( acquireTimeout ),
	  m_statementTimeout( statementTimeout ), m_programFile( programFile )
{
	m_connStr = buildConnStr( host, port,  dbName, user, password,
				  sslMode, sslCert, sslKey, sslRootCert, sslCRL,
				  connectTimeout );
	MOD_LOG_DATA << "PostgreSQL database '" << m_ID << "' connection string <" << m_connStr << ">";

	for ( size_t i = 0; i < connections; i++ )	{
		PGconn* conn = PQconnectdb( m_connStr.c_str() );
		if ( conn == NULL )
			MOD_LOG_ALERT << "Failed to connect to PostgreSQL database '" << m_ID << "'";
		else	{
			ConnStatusType stat = PQstatus( conn );
			switch( stat )	{
				case CONNECTION_OK:
					MOD_LOG_TRACE << "PostgreSQL database '" << m_ID << "' constructor: connection "
						      << i << " connected OK: '" << PQerrorMessage( conn ) << "'";
					break;
				case CONNECTION_BAD:
					MOD_LOG_TRACE << "PostgreSQL database '" << m_ID << "' constructor: connection "
						      << i << " connection BAD: '" << PQerrorMessage( conn ) << "'";
					break;
				case CONNECTION_STARTED:
					MOD_LOG_TRACE << "PostgreSQL database '" << m_ID << "' constructor: connection "
						      << i << " connection STARTED: '" << PQerrorMessage( conn ) << "'";
					break;
				case CONNECTION_MADE:
					MOD_LOG_TRACE << "PostgreSQL database '" << m_ID << "' constructor: connection "
						      << i << " connection MADE: '" << PQerrorMessage( conn ) << "'";
					break;
				case CONNECTION_AWAITING_RESPONSE:
					MOD_LOG_TRACE << "PostgreSQL database '" << m_ID << "' constructor: connection "
						      << i << " connection AWAITING RESPONSE: '" << PQerrorMessage( conn ) << "'";
					break;
				case CONNECTION_AUTH_OK:
					MOD_LOG_TRACE << "PostgreSQL database '" << m_ID << "' constructor: connection "
						      << i << " connection AUTH OK: '" << PQerrorMessage( conn ) << "'";
					break;
				case CONNECTION_SSL_STARTUP:
					MOD_LOG_TRACE << "PostgreSQL database '" << m_ID << "' constructor: connection "
						      << i << " connection SSL start: '" << PQerrorMessage( conn ) << "'";
					break;
				case CONNECTION_SETENV:
					MOD_LOG_TRACE << "PostgreSQL database '" << m_ID << "' constructor: connection "
						      << i << " connection SETENV: '" << PQerrorMessage( conn ) << "'";
					break;
				case CONNECTION_NEEDED:
					MOD_LOG_TRACE << "PostgreSQL database '" << m_ID << "' constructor: connection "
						      << i << " connection NEEDED: '" << PQerrorMessage( conn ) << "'";
					break;
			}
			m_connPool.add( conn );
			m_noConnections++;
		}
	}

	m_db.setUnit( this );

	MOD_LOG_DEBUG << "PostgreSQL database '" << m_ID << "' created with a pool of " << m_noConnections << " connections";
}


// This function needs a lot of work and thinking...
PostgreSQLdbUnit::~PostgreSQLdbUnit()
{
	size_t connections = 0;

	m_db.setUnit( NULL );
	m_connPool.timeout( 3 );

	while ( m_connPool.available() )	{
		PGconn* conn = m_connPool.get();
		if ( conn == NULL )	{
			MOD_LOG_ALERT << "PostgreSQL database '" << m_ID << "' destructor: NULL connection from pool";
			throw std::logic_error( "PostgreSQL database destructor: NULL connection from pool" );
		}
		PGTransactionStatusType stat = PQtransactionStatus( conn );
		switch( stat )	{
			case PQTRANS_IDLE:
				PQfinish( conn );
				MOD_LOG_TRACE << "PostgreSQL database '" << m_ID << "' destructor: Connection " << connections << " idle";
				m_noConnections--, connections++;
				break;
			case PQTRANS_ACTIVE:
				PQfinish( conn );
				MOD_LOG_TRACE << "PostgreSQL database '" << m_ID << "' destructor: Connection " << connections << " active";
				m_noConnections--, connections++;
				break;
			case PQTRANS_INTRANS:
				PQfinish( conn );
				MOD_LOG_TRACE << "PostgreSQL database '" << m_ID << "' destructor: Connection " << connections << " in transaction";
				m_noConnections--, connections++;
				break;
			case PQTRANS_INERROR:
				PQfinish( conn );
				MOD_LOG_TRACE << "PostgreSQL database '" << m_ID << "' destructor: Connection " << connections << " in transaction error";
				m_noConnections--, connections++;
				break;
			case PQTRANS_UNKNOWN:
				PQfinish( conn );
				MOD_LOG_TRACE << "PostgreSQL database '" << m_ID << "' destructor: Connection " << connections << " status unknown";
				m_noConnections--, connections++;
				break;
		}
	}
	if ( m_noConnections != 0 )	{
		MOD_LOG_ALERT << "PostgreSQL database unit '" << m_ID << "' destructor: "
			      << m_noConnections << " connections not destroyed";
		throw std::logic_error( "PostgreSQL database unit destructor: not all connections destroyed" );
	}
	MOD_LOG_TRACE << "PostgreSQL database unit '" << m_ID << "' destroyed, " << connections << " connections destroyed";
}

bool PostgreSQLdbUnit::loadProgram()
{
	// No program file, do nothing
	if ( m_programFile.empty())
		return true;
	if ( !boost::filesystem::exists( m_programFile ))	{
		MOD_LOG_ALERT << "Program file '" << m_programFile
			      << "' does not exist (PostgreSQL database '" << m_ID << "')";
		return false;
	}
	return true;
}

Database* PostgreSQLdbUnit::database()
{
	return m_db.hasUnit() ? &m_db : NULL;
}


/*****  PostgreSQL database  ******************************************/
const std::string& PostgreSQLdatabase::ID() const
{
	if ( m_unit )
		return m_unit->ID();
	else
		throw std::runtime_error( "SQL database unit not initialized" );
}

Transaction* PostgreSQLdatabase::transaction( const std::string& /*name*/ )
{
	return new PostgreSQLtransaction( *this );
}

void PostgreSQLdatabase::closeTransaction( Transaction *t )
{
	delete t;
}

/*****  PostgreSQL transaction  ***************************************/
PostgreSQLtransaction::PostgreSQLtransaction( PostgreSQLdatabase& database )
	: m_db( database ), m_unit( database.dbUnit() )
{
}

const std::string& PostgreSQLtransaction::databaseID() const
{
	return m_unit.ID();
}

void PostgreSQLtransaction::execute()
{
	try	{
		_Wolframe::PoolObject<  PGconn* > conn( m_unit.m_connPool );
		int ver = PQprotocolVersion( *conn );
		MOD_LOG_DEBUG << "PostgreSQL protocol version: " << ver;
	}
	catch ( _Wolframe::ObjectPoolTimeout )
	{
	}
}

void PostgreSQLtransaction::close()
{
	m_db.closeTransaction( this );
}

}} // _Wolframe::db

