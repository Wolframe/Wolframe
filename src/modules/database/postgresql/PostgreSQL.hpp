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
// Wolframe PostgreSQL client
//

#ifndef _POSTGRESQL_HPP_INCLUDED
#define _POSTGRESQL_HPP_INCLUDED

#include "PostgreSQLTransactionExecStatemachine.hpp"
#include "PostgreSQLConfig.hpp"
#include "logger-v1.hpp"
#include <libpq-fe.h>
#include <list>
#include <vector>
#include "database/database.hpp"
#include "database/transaction.hpp"
#include "PostgreSQLServerSettings.hpp"
#include "module/constructor.hpp"
#include "system/objectPool.hpp"
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>

#ifdef _WIN32
#pragma warning(disable:4250)
#endif

namespace _Wolframe {
namespace db {

enum {
	POSTGRESQL_MIN_DB_VERSION = 80400,
	POSTGRESQL_MIN_PROTOCOL_VERSION = 3
};

struct PostgreSQLLanguageDescription : public LanguageDescription
{
	virtual std::string stm_argument_reference( int index) const
	{
		std::ostringstream rt;
		rt << "$" << index;
		return rt.str( );
	}
};

class PostgreSQLDatabase : public Database
{
public:
	PostgreSQLDatabase( const std::string& id,
			  const std::string& host, unsigned short port, const std::string& dbName,
			  const std::string& user, const std::string& password,
			  const std::string& sslMode, const std::string& sslCert,
			  const std::string& sslKey, const std::string& sslRootCert,
			  const std::string& sslCRL,
			  unsigned short connectTimeout,
			  unsigned short connections, unsigned short acquireTimeout,
			  unsigned statementTimeout);
	PostgreSQLDatabase( const PostgreSQLConfig& config);
	 ~PostgreSQLDatabase();

	const std::string& ID() const		{ return m_ID; }
	const char* className() const		{ return POSTGRESQL_DB_CLASS_NAME; }

	Transaction* transaction( const std::string& name_)
	{
		TransactionExecStatemachineR stm( new TransactionExecStatemachine_postgres( this));
		return new Transaction( name_, stm);
	}

	virtual const LanguageDescription* getLanguageDescription( ) const
	{
		static PostgreSQLLanguageDescription langdescr;
		return &langdescr;
	}

	boost::shared_ptr<PGconn> newConnection()
	{
		return boost::shared_ptr<PGconn>( m_connPool.get(), boost::bind( ObjectPool<PGconn*>::add, &m_connPool, _1));
	}

	PostgreSQLServerSettings serverSettings() const
						{ return m_serverSettings; }

private:
	void init( const PostgreSQLConfig& config);

	static _Wolframe::log::LogLevel::Level getLogLevel( const std::string& severity);

public:
	static void noticeProcessor( void* this_void, const char * message);

private:
	const std::string	m_ID;			//< database ID
	std::string		m_connStr;		//< connection string
	unsigned short		m_connections;		//< number of connections
	PostgreSQLServerSettings m_serverSettings;	//< data like protocol settings, OIDs, etc. loaded at initialization from server
	ObjectPool< PGconn* >	m_connPool;		//< pool of connections
};

}} // _Wolframe::db

#endif // _POSTGRESQL_HPP_INCLUDED
