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

#ifndef _POSTGRESQL_HPP_INCLUDED
#define _POSTGRESQL_HPP_INCLUDED

#include "database/database.hpp"
#include "container.hpp"
#include "moduleInterface.hpp"

#include <list>
#include "objectPool.hpp"
#include "libpq-fe.h"

namespace _Wolframe {
namespace db {

/// PostgreSQL server connection configuration
class PostgreSQLconfig : public config::ObjectConfiguration
{
	friend class config::ConfigurationParser;
	friend class PostgreSQLunit;
public:
	const char* objectName() const			{ return "PostgreSQL"; }

	PostgreSQLconfig( const char* name, const char* logParent, const char* logName );
	~PostgreSQLconfig()				{}

	virtual bool check() const;
	virtual void print( std::ostream& os, size_t indent ) const;
private:
	std::string	m_ID;
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
			    size_t connections, unsigned short acquireTimeout );
	virtual ~PostgreSQLdatabase();

	virtual const std::string& ID() const		{ return m_ID; }
	virtual const char* typeName() const		{ return "PostgreSQL"; }
	virtual bool doTransaction( DatabaseRequest&, DatabaseAnswer&,
			    unsigned short, unsigned short ){ return true; }
private:
	const std::string	m_ID;			//< database ID
	std::string		m_connStr;		//< connection string
	size_t			m_noConnections;	//< number of connections
	std::list< PGconn* >	m_connections;		//< list of DB connections
	ObjectPool< PGconn* >	m_connPool;		//< pool of connections
};


class PostgreSQLunit : public DatabaseUnit
{
public:
	PostgreSQLunit( const PostgreSQLconfig& conf );
	~PostgreSQLunit();

	virtual const Database& database() const	{ return m_db; }
private:
	const PostgreSQLdatabase	m_db;
};


class PostgreSQLcontainer : public module::ModuleContainer< PostgreSQLcontainer, PostgreSQLconfig,
		db::DatabaseUnit >
{
public:
	PostgreSQLcontainer( const PostgreSQLconfig& conf ) : m_unit( conf )	{}
	~PostgreSQLcontainer()				{}

	virtual const std::string& ID() const		{ return m_unit.database().ID(); }
	virtual const char* typeName() const		{ return m_unit.database().typeName(); }
	virtual const DatabaseUnit& object() const	{ return m_unit; }
private:
	const PostgreSQLunit		m_unit;
};

}} // _Wolframe::db

#endif // _POSTGRESQL_HPP_INCLUDED
