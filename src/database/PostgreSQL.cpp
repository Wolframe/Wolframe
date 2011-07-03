/************************************************************************

 Copyright (C) 2011 Project Wolframe.
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

#include "logger.hpp"
#include "PostgreSQL.hpp"
#include "database/databaseOperation.hpp"

#include <string>
#include <sstream>

namespace _Wolframe {
namespace db {

//***  PostgreSQL configuration functions  **********************************
PostgreSQLconfig::PostgreSQLconfig( const char* cfgName, const char* logParent, const char* logName )
	: module::ModuleConfiguration< PostgreSQLconfig, DatabaseConfig >( cfgName, logParent, logName )
{
	port = 0;
	connections = 0;
	acquireTimeout = 0;
}

void PostgreSQLconfig::print( std::ostream& os, size_t indent ) const
{
	std::string indStr( indent, ' ' );

	os << indStr << sectionName() << ":" << std::endl;
	if ( ! ID().empty() )
		os << indStr << "   ID: " << ID() << std::endl;
	if ( host.empty())
		os << indStr << "   Database host: local unix domain socket" << std::endl;
	else
		os << indStr << "   Database host: " << host << ":" << port << std::endl;
	os << indStr << "   Database name: " << (dbName.empty() ? "(not specified - server user default)" : dbName) << std::endl;
	os << indStr << "   Database user: " << (user.empty() ? "(not specified - same as server user)" : user)
	   << ", password: " << (password.empty() ? "(not specified - no password used)" : password) << std::endl;
	if ( connectTimeout == 0 )
		os << indStr << "   Connect timeout: 0 (wait indefinitely)" << std::endl;
	else
		os << indStr << "   Connect timeout: " << connectTimeout << "s" << std::endl;
	os << indStr << "   Database connections: " << connections << std::endl;
	if ( connectTimeout == 0 )
		os << indStr << "   Acquire database connection timeout: 0 (wait indefinitely)" << std::endl;
	else
		os << indStr << "   Acquire database connection timeout: " << acquireTimeout << "s" << std::endl;
}

bool PostgreSQLconfig::check() const
{
	if ( connections == 0 )	{
		LOG_ERROR << logPrefix() << "number of database connections cannot be 0";
		return false;
	}
	return true;
}


//***  PostgreSQL database container  ***************************************
PostgreSQLcontainer::PostgreSQLcontainer( const PostgreSQLconfig& conf )
	: m_db( conf.ID(), conf.host, conf.port, conf.dbName,
		  conf.user, conf.password, conf.connectTimeout,
		  conf.connections, conf.acquireTimeout )
{
	LOG_NOTICE << "PostgreSQL database container for '" << conf.ID() << "' created";
}

PostgreSQLcontainer::~PostgreSQLcontainer()
{
}


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
	if ( connectTimeout != 0 )	{
		if ( ! ss.str().empty())
			ss << " ";
		ss << "connect_timeout = " << connectTimeout;
	}
	return ss.str();
}

// This function also needs a lot of work
PostgreSQLdatabase::PostgreSQLdatabase( const std::string& id,
					const std::string& host, unsigned short port,
					const std::string& dbName,
					const std::string& user, const std::string& password,
					unsigned short connectTimeout,
					size_t connections, unsigned short acquireTimeout )
	: m_ID( id ), m_noConnections( 0 ), m_connPool( acquireTimeout )
{
	m_connStr = buildConnStr( host, port,  dbName, user, password, connectTimeout );
	LOG_DATA << "PostgreSQL database '" << m_ID << "' connection string <" << m_connStr << ">";

	for ( size_t i = 0; i < connections; i++ )	{
		PGconn* conn = PQconnectdb( m_connStr.c_str() );
		if ( conn == NULL )
			LOG_ALERT << "PostgreSQL PQconnectdb returned NULL";
		else	{
			ConnStatusType stat = PQstatus( conn );
			switch( stat )	{
			case CONNECTION_OK:
				LOG_DATA << "PostgreSQL database '" << m_ID << "' constructor: connection "
					 << i << " connected OK. Message: " << PQerrorMessage( conn );
				break;
			case CONNECTION_BAD:
				LOG_DATA << "PostgreSQL database '" << m_ID << "' constructor: connection "
					 << i << " connection BAD. Message: " << PQerrorMessage( conn );
				break;
			case CONNECTION_STARTED:
				LOG_DATA << "PostgreSQL database '" << m_ID << "' constructor: connection "
					 << i << " connection STARTED. Message: " << PQerrorMessage( conn );
				break;
			case CONNECTION_MADE:
				LOG_DATA << "PostgreSQL database '" << m_ID << "' constructor: connection "
					 << i << " connection MADE. Message: " << PQerrorMessage( conn );
				break;
			case CONNECTION_AWAITING_RESPONSE:
				LOG_DATA << "PostgreSQL database '" << m_ID << "' constructor: connection "
					 << i << " connection AWAITING RESPONSE. Message: " << PQerrorMessage( conn );
				break;
			case CONNECTION_AUTH_OK:
				LOG_DATA << "PostgreSQL database '" << m_ID << "' constructor: connection "
					 << i << " connection AUTH OK. Message: " << PQerrorMessage( conn );
				break;
			case CONNECTION_SSL_STARTUP:
				LOG_DATA << "PostgreSQL database '" << m_ID << "' constructor: connection "
					 << i << " connection SSL start. Message: " << PQerrorMessage( conn );
				break;
			case CONNECTION_SETENV:
				LOG_DATA << "PostgreSQL database '" << m_ID << "' constructor: connection "
					 << i << " connection SETENV. Message: " << PQerrorMessage( conn );
				break;
			case CONNECTION_NEEDED:
				LOG_DATA << "PostgreSQL database '" << m_ID << "' constructor: connection "
					 << i << " connection NEEDED. Message: " << PQerrorMessage( conn );
				break;
			}
			m_connPool.add( conn );
			m_noConnections++;
		}
	}
	LOG_TRACE << "PostgreSQL database '" << m_ID << "' created with a pool of " << m_noConnections << " connections";
}


// This function needs a lot of work and thinking...
PostgreSQLdatabase::~PostgreSQLdatabase()
{
	size_t connections = 0;
	m_connPool.timeout( 3 );

	while ( m_connPool.available() )	{
		PGconn* conn = m_connPool.get();
		if ( conn == NULL )	{
			LOG_ALERT << "PostgreSQL database '" << m_ID << "' destructor: NULL connection from pool";
			throw std::logic_error( "PostgreSQL database destructor: NULL connection from pool" );
		}
		PGTransactionStatusType stat = PQtransactionStatus( conn );
		switch( stat )	{
		case PQTRANS_IDLE:
			LOG_DATA << "PostgreSQL database '" << m_ID << "' destructor: Connection " << connections << " idle";
			PQfinish( conn );
			m_noConnections--, connections++;
			break;
		case PQTRANS_ACTIVE:
			LOG_DATA << "PostgreSQL database '" << m_ID << "' destructor: Connection " << connections << " active";
			PQfinish( conn );
			m_noConnections--, connections++;
			break;
		case PQTRANS_INTRANS:
			LOG_DATA << "PostgreSQL database '" << m_ID << "' destructor: Connection " << connections << " in transaction";
			PQfinish( conn );
			m_noConnections--, connections++;
			break;
		case PQTRANS_INERROR:
			LOG_DATA << "PostgreSQL database '" << m_ID << "' destructor: Connection " << connections << " in transaction error";
			PQfinish( conn );
			m_noConnections--, connections++;
			break;
		case PQTRANS_UNKNOWN:
			LOG_DATA << "PostgreSQL database '" << m_ID << "' destructor: Connection " << connections << " status unknown";
			PQfinish( conn );
			m_noConnections--, connections++;
			break;
		}
	}
	if ( m_noConnections != 0 )	{
		LOG_ALERT << "PostgreSQL database '" << m_ID << "' destructor: not all connections destroyed";
		throw std::logic_error( "PostgreSQL database destructor: not all connections destroyed" );
	}
	LOG_TRACE << "PostgreSQL database '" << m_ID << "' destructor: " << connections << " connections destroyed";
}

}} // _Wolframe::db

