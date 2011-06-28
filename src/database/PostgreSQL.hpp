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
// Wolframe PostgreSQL client
//

#include "database/database.hpp"
#include "database/databaseContainer.hpp"

#include <list>
#include "objectPool.hpp"
#include "libpq-fe.h"

#ifndef _POSTGRESQL_HPP_INCLUDED
#define _POSTGRESQL_HPP_INCLUDED

namespace _Wolframe {
namespace db {

/// PostgreSQL server connection configuration
class PostgreSQLconfig : public DatabaseConfig
{
	friend class config::ConfigurationParser;
	friend class PostgreSQLcontainer;
public:
	const char* typeName() const			{ return "PostgreSQL"; }

	PostgreSQLconfig( const char* name, const char* logParent, const char* logName );
	~PostgreSQLconfig()				{}

	static module::ModuleConfiguration* create( const char* name,
						    const char* logParent, const char* logName )	{
		return new PostgreSQLconfig( name, logParent, logName );
	}

	bool check() const;
	void print( std::ostream& os, size_t indent ) const;
private:
	std::string	host;
	unsigned short	port;
	std::string	dbName;
	std::string	user;
	std::string	password;
	unsigned short	connectTimeout;
	unsigned short	connections;
	unsigned short	acquireTimeout;
};


class PostgreSQLdatabase : public Database
{
public:
	PostgreSQLdatabase( const std::string& id,
			    const std::string& host, unsigned short port, const std::string& dbName,
			    const std::string& user, const std::string& password,
			    unsigned short connectTimeout,
			    unsigned short connections, unsigned short acquireTimeout );
	~PostgreSQLdatabase();

	const std::string& ID() const			{ return m_ID; }
	const char* typeName() const			{ return "PostgreSQL"; }
	bool isOpen() const				{ return true; }
	void close()					{}
	bool doTransaction( DatabaseRequest&, DatabaseAnswer& ){ return true; }
private:
	const std::string	m_ID;			//< database ID
	std::string		m_connStr;		//< connection string
	std::list< PGconn* >	m_connections;		//< list of DB connections
	ObjectPool< PGconn* >	m_connPool;		//< pool of connections
};


class PostgreSQLcontainer : public module::ModuleContainer< PostgreSQLcontainer, PostgreSQLconfig,
		DatabaseContainer, DatabaseConfig >
{
public:
	PostgreSQLcontainer( const PostgreSQLconfig& conf );
	~PostgreSQLcontainer();

	const std::string& ID() const			{ return m_db.ID(); }
	const char* typeName() const			{ return m_db.typeName(); }
	virtual const Database& database()		{ return m_db; }
private:
	PostgreSQLdatabase	m_db;
};

}} // _Wolframe::db

#endif // _POSTGRESQL_HPP_INCLUDED
