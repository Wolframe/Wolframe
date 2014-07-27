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
// Wolframe PostgreSQL client view implementation
//

#include "logger-v1.hpp"
#include "PostgreSQL.hpp"
#include "utils/fileUtils.hpp"
#include "PostgreSQLTransactionExecStatemachine.hpp"
#include <string>
#include <sstream>

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
				 const std::string& sslRootCert, const std::string& sslCRL,
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

_Wolframe::log::LogLevel::Level PostgreSQLDatabase::getLogLevel( const std::string& severity)
{
	struct LogMsgMap :public std::map<std::string,_Wolframe::log::LogLevel::Level>
	{
		LogMsgMap()
		{
			typedef _Wolframe::log::LogLevel LV;
			(*this)["WARNING"] = LV::LOGLEVEL_WARNING;
			(*this)["ERROR"] = LV::LOGLEVEL_ERROR;
			(*this)["FATAL"] = LV::LOGLEVEL_FATAL;
			(*this)["PANIC"] = LV::LOGLEVEL_ALERT;
			(*this)["NOTICE"] = LV::LOGLEVEL_NOTICE;
			(*this)["DETAIL"] = LV::LOGLEVEL_INFO;
			(*this)["INFO"] = LV::LOGLEVEL_INFO;
			(*this)["DEBUG"] = LV::LOGLEVEL_DEBUG;
			(*this)["LOG"] = LV::LOGLEVEL_TRACE;
		}
	};
	static LogMsgMap logMsgMap;
	LogMsgMap::const_iterator li = logMsgMap.find( severity);
	if (li == logMsgMap.end())
	{
		return log::LogLevel::LOGLEVEL_UNDEFINED;
	}
	else
	{
		return li->second;
	}
}

void PostgreSQLDatabase::noticeProcessor( void* this_void, const char * message)
{
	PostgreSQLDatabase* this_ = (PostgreSQLDatabase*)this_void;
	std::size_t ii=0;
	for (; message[ii] && message[ii] != ':'; ++ii);
	if (message[ii])
	{
		_Wolframe::log::LogLevel::Level lv = getLogLevel( std::string( message, ii));
		if (lv != log::LogLevel::LOGLEVEL_UNDEFINED)
		{
			_Wolframe::log::Logger( _Wolframe::log::LogBackend::instance() ).Get( lv) << "PostgreSQL database '" << ((this_)?this_->ID():"") << "': " << (message + ii + 1);
			return;
		}
	}
	LOG_ERROR << "Unknown log message type from PostgreSQL database '" << ((this_)?this_->ID():"") << "': " << message;
}

PostgreSQLDatabase::PostgreSQLDatabase( const PostgreSQLConfig& config)
	:m_ID(config.ID())
	,m_connections(0)
	,m_connPool(config.acquireTimeout())
{
	init( config);
}

PostgreSQLDatabase::PostgreSQLDatabase( const std::string& id_,
		  const std::string& host_, unsigned short port_, const std::string& dbName_,
		  const std::string& user_, const std::string& password_,
		  const std::string& sslMode_, const std::string& sslCert_,
		  const std::string& sslKey_, const std::string& sslRootCert_,
		  const std::string& sslCRL_,
		  unsigned short connectTimeout_,
		  unsigned short connections_, unsigned short acquireTimeout_,
		  unsigned statementTimeout_)
	:m_ID(id_)
	,m_connections(0)
	,m_connPool(acquireTimeout_)
{
	PostgreSQLConfig config( id_, host_, port_, dbName_, user_, password_,
		sslMode_, sslCert_, sslKey_, sslRootCert_, sslCRL_,
		connectTimeout_, connections_, acquireTimeout_, statementTimeout_);
	init( config);
}

// This function also needs a lot of work
void PostgreSQLDatabase::init( const PostgreSQLConfig& config)
{
	int connections = config.connections();

	m_connStr = buildConnStr( config.host(), config.port(), config.dbName(),
				  config.user(), config.password(),
				  config.sslMode(), config.sslCert(), config.sslKey(),
				  config.sslRootCert(), config.sslCRL(),
				  config.connectTimeout() );
	LOG_DATA << "PostgreSQL database '" << m_ID << "' connection string <" << m_connStr << ">";

	for ( unsigned short i = 0; i < connections; i++ )	{
		PGconn* conn = PQconnectdb( m_connStr.c_str() );
		if ( conn == NULL )
			LOG_ALERT << "Failed to connect to PostgreSQL database '" << m_ID << "'";
		else	{
			if (i == 0)
			{
				//... the first connection is used to load some info
				m_serverSettings.load( conn);
			}

			PQsetNoticeProcessor( conn, &noticeProcessor, this);

			ConnStatusType stat = PQstatus( conn );
			std::string err = PQerrorMessage( conn );
			while ( !err.empty() && ( err[ err.size() - 1 ] == '\r' ||  err[ err.size() - 1 ] == '\n' ))
				err.erase( err.size() - 1 );
			switch( stat )	{
				case CONNECTION_OK:
					LOG_TRACE << "PostgreSQL database '" << m_ID << "' constructor: connection "
						      << i << " connected OK.";
					break;
				case CONNECTION_BAD:
					if ( err.compare( 0, 6, "FATAL:" ) == 0 )	{
						LOG_ALERT << "PostgreSQL database '" << m_ID << "' (connection "
							      << i << ") error: '" << err << "'";
						throw std::runtime_error( "Fatal error connecting to the PostgreSQL database" );
					}
					LOG_WARNING << "PostgreSQL database '" << m_ID << "' connection "
							<< i << " error: '" << err << "'";
					break;
				case CONNECTION_STARTED:
					LOG_ALERT << "PostgreSQL database '" << m_ID << "' constructor: connection "
						      << i << " connection STARTED: '" << err << "'";
					throw std::range_error( "PostgreSQL database: CONNECTION_STARTED in synchronous connection" );
					break;
				case CONNECTION_MADE:
					LOG_ALERT << "PostgreSQL database '" << m_ID << "' constructor: connection "
						      << i << " connection MADE: '" << err << "'";
					throw std::range_error( "PostgreSQL database: CONNECTION_MADE in synchronous connection" );
					break;
				case CONNECTION_AWAITING_RESPONSE:
					LOG_ALERT << "PostgreSQL database '" << m_ID << "' constructor: connection "
						      << i << " connection AWAITING RESPONSE: '" << err << "'";
					throw std::range_error( "PostgreSQL database: CONNECTION_AWAITING_RESPONSE in synchronous connection" );
					break;
				case CONNECTION_AUTH_OK:
					LOG_ALERT << "PostgreSQL database '" << m_ID << "' constructor: connection "
						      << i << " connection AUTH OK: '" << err << "'";
					throw std::range_error( "PostgreSQL database: CONNECTION_AUTH_OK in synchronous connection" );
					break;
				case CONNECTION_SSL_STARTUP:
					LOG_ALERT << "PostgreSQL database '" << m_ID << "' constructor: connection "
						      << i << " connection SSL start: '" << err << "'";
					throw std::range_error( "PostgreSQL database: CONNECTION_SSL_STARTUP in synchronous connection" );
					break;
				case CONNECTION_SETENV:
					LOG_ALERT << "PostgreSQL database '" << m_ID << "' constructor: connection "
						      << i << " connection SETENV: '" << err << "'";
					throw std::range_error( "PostgreSQL database: CONNECTION_SETENV in synchronous connection" );
					break;
				case CONNECTION_NEEDED:
					LOG_ALERT << "PostgreSQL database '" << m_ID << "' constructor: connection "
						      << i << " connection NEEDED: '" << err << "'";
					throw std::range_error( "PostgreSQL database: CONNECTION_NEEDED in synchronous connection" );
					break;
			}

			std::stringstream statement_timeout_s;
			statement_timeout_s << "SET statement_timeout = " << config.statementTimeout();
			PQexec( conn, statement_timeout_s.str( ).c_str( ) );

			m_connPool.add( conn );
			m_connections++;
		}
	}
	LOG_DEBUG << "PostgreSQL database '" << m_ID << "' created with a pool of " << m_connections << " connections";
}


// This function needs a lot of work and thinking...
PostgreSQLDatabase::~PostgreSQLDatabase()
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
				PQfinish( conn );
				LOG_TRACE << "PostgreSQL database '" << m_ID << "' destructor: Connection " << connections << " idle";
				m_connections--, connections++;
				break;
			case PQTRANS_ACTIVE:
				PQfinish( conn );
				LOG_TRACE << "PostgreSQL database '" << m_ID << "' destructor: Connection " << connections << " active";
				m_connections--, connections++;
				break;
			case PQTRANS_INTRANS:
				PQfinish( conn );
				LOG_TRACE << "PostgreSQL database '" << m_ID << "' destructor: Connection " << connections << " in transaction";
				m_connections--, connections++;
				break;
			case PQTRANS_INERROR:
				PQfinish( conn );
				LOG_TRACE << "PostgreSQL database '" << m_ID << "' destructor: Connection " << connections << " in transaction error";
				m_connections--, connections++;
				break;
			case PQTRANS_UNKNOWN:
				PQfinish( conn );
				LOG_TRACE << "PostgreSQL database '" << m_ID << "' destructor: Connection " << connections << " status unknown";
				m_connections--, connections++;
				break;
		}
	}
	if ( m_connections != 0 )	{
		LOG_ALERT << "PostgreSQL database '" << m_ID << "' destructor: "
			      << m_connections << " connections not destroyed";
		throw std::logic_error( "PostgreSQL database destructor: not all connections destroyed" );
	}
	LOG_TRACE << "PostgreSQL database '" << m_ID << "' destroyed, " << connections << " connections destroyed";
	LOG_TRACE << "PostgreSQL database '" << m_ID << "' destroyed, " << connections << " connections destroyed";
}

}} // _Wolframe::db

