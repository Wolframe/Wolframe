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
// Wolframe PostgreSQL client
//

#ifndef _POSTGRESQL_HPP_INCLUDED
#define _POSTGRESQL_HPP_INCLUDED

#include <libpq-fe.h>
#include <list>

#include "database/database.hpp"
#include "config/configurationBase.hpp"
#include "constructor.hpp"

#include "objectPool.hpp"

namespace _Wolframe {
namespace db {

static const char* POSTGRESQL_DB_CLASS_NAME = "PostgreSQL";

static const unsigned int POSTGRESQL_MIN_DB_VERSION = 80400;
static const unsigned short POSTGRESQL_MIN_PROTOCOL_VERSION = 3;

/// PostgreSQL server connection configuration
class PostgreSQLconfig : public config::NamedConfiguration
{
	friend class PostgreSQLconstructor;
public:
	const char* className() const		{ return POSTGRESQL_DB_CLASS_NAME; }

	PostgreSQLconfig( const char* name, const char* logParent, const char* logName );
	~PostgreSQLconfig()			{}

	bool parse( const config::ConfigurationTree& pt, const std::string& node,
		    const module::ModulesDirectory* modules );
	virtual bool check() const;
	virtual void print( std::ostream& os, size_t indent ) const;
private:
	std::string	m_ID;			///< database identifier
	std::string	host;			///< server host
	unsigned short	port;			///< server port
	std::string	dbName;			///< database name on server
	std::string	user;			///< database user
	std::string	password;		///< and password
	unsigned short	connectTimeout;		///< connection timeout
	unsigned short	connections;		///< number of database connection (pool size)
	unsigned short	acquireTimeout;		///< timeout when acquiring a connection from the pool
	unsigned	statementTimeout;	///< default timeout when executin a statement
	std::string	sslMode;		///< SSL connection mode
	std::string	sslCert;		///< client SSL certificate file
	std::string	sslKey;			///< client SSL key file
	std::string	sslRootCert;		///< root SSL certificate file
	std::string	sslCRL;			///< SSL certificate revocation list
};


class PostgreSQLdatabase : public Database, public DatabaseUnit
{
public:
	PostgreSQLdatabase( const std::string& id,
			    const std::string& host, unsigned short port, const std::string& dbName,
			    const std::string& user, const std::string& password,
			    unsigned short connectTimeout,
			    size_t connections, unsigned short acquireTimeout );
	~PostgreSQLdatabase();

	const std::string& ID() const		{ return m_ID; }
	const char* className() const		{ return POSTGRESQL_DB_CLASS_NAME; }
	Database& database()			{ return *this; }

	bool doTransaction( DatabaseRequest&, DatabaseAnswer&,
			    unsigned short, unsigned short )
						{ return true; }
private:
	const std::string	m_ID;			///< database ID
	std::string		m_connStr;		///< connection string
	size_t			m_noConnections;	///< number of connections
	ObjectPool< PGconn* >	m_connPool;		///< pool of connections
};


//***  PostgreSQL database constructor  ***************************************
class PostgreSQLconstructor : public ConfiguredObjectConstructor< db::DatabaseUnit >
{
public:
	virtual ObjectConstructorBase::ObjectType objectType() const
						{ return DATABASE_OBJECT; }
	const char* objectClassName() const	{ return POSTGRESQL_DB_CLASS_NAME; }
	PostgreSQLdatabase* object( const config::NamedConfiguration& conf );
};

}} // _Wolframe::db

#endif // _POSTGRESQL_HPP_INCLUDED
