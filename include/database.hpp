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
// database.hpp - Wolframe base database class
//

#ifndef _DATABASE_HPP_INCLUDED
#define _DATABASE_HPP_INCLUDED

#include <list>
#include "config/configurationBase.hpp"
#include "logger.hpp"

namespace _Wolframe	{
namespace db	{

/// database type
enum DatabaseType	{
	DBTYPE_REFERENCE,
	DBTYPE_POSTGRESQL,
	DBTYPE_SQLITE
};


class	DatabaseConfig : public _Wolframe::config::ConfigurationBase
{
public:
	DatabaseConfig( const char* name, const char* logParent, const char* logName )
		: ConfigurationBase( name, logParent, logName ){}
	virtual ~DatabaseConfig()			{}

	virtual DatabaseType type() const = 0;
	void ID( const std::string& id )		{ m_ID = id; }
	const std::string& ID() const			{ return m_ID; }
private:
	std::string		m_ID;
};


/// PostgreSQL server connection configuration
struct	PostgreSQLconfig : public DatabaseConfig
{
	std::string	host;
	unsigned short	port;
	std::string	dbName;
	std::string	user;
	std::string	password;
	unsigned short	connections;
	unsigned short	acquireTimeout;
public:
	DatabaseType type() const			{ return DBTYPE_POSTGRESQL; }

	PostgreSQLconfig( const char* name, const char* logParent, const char* logName );
	~PostgreSQLconfig()				{}

	bool check() const;
	void print( std::ostream& os, size_t indent ) const;
};


/// SQLite database configuration
struct	SQLiteConfig : public DatabaseConfig
{
	std::string	filename;
	bool		flag;
public:
	DatabaseType type() const			{ return DBTYPE_SQLITE; }

	SQLiteConfig( const char* name, const char* logParent, const char* logName );
	~SQLiteConfig()					{}

	bool check() const;
	void print( std::ostream& os, size_t indent ) const;
	virtual void setCanonicalPathes( const std::string& referencePath );
};


/// database reference class
struct ReferenceConfig : public DatabaseConfig
{
	std::string	m_ref;
public:
	DatabaseType type() const			{ return DBTYPE_REFERENCE; }

	ReferenceConfig( const char* name, const char* logParent, const char* logName )
		: DatabaseConfig( name, logParent, logName )	{}
	~ReferenceConfig()					{}

	bool check() const;
	void print( std::ostream& os, size_t indent ) const;
};


/// database configurations
struct Configuration : public _Wolframe::config::ConfigurationBase
{
public:
	std::list<DatabaseConfig*>	m_dbConfig;

	/// constructor & destructor
	Configuration() : ConfigurationBase( "Database(s)", NULL, "Database configuration" )	{}
	~Configuration();

	/// methods
	bool check() const;
	void print( std::ostream& os, size_t indent ) const;
	virtual void setCanonicalPathes( const std::string& referencePath );

//	Not implemented yet, inherited from base for the time being
//	bool test() const;
};

struct SingleDBConfiguration : public _Wolframe::config::ConfigurationBase
{
public:
	DatabaseConfig*	m_dbConfig;

	/// constructor & destructor
	SingleDBConfiguration( const char* name, const char* logParent, const char* logName )
		: ConfigurationBase( name, logParent, logName )	{ m_dbConfig = NULL; }
	~SingleDBConfiguration();

	/// methods
	bool check() const;
	void print( std::ostream& os, size_t indent ) const;
	virtual void setCanonicalPathes( const std::string& referencePath );
};


/// database base classes
class Database
{
public:
	Database( const std::string id ) : m_id( id )	{}
	virtual ~Database()				{}

	const std::string& ID() const			{ return m_id; }
	virtual DatabaseType type() const = 0;

	static DatabaseType strToType( const char *str );
	static std::string& typeToStr( DatabaseType type );
private:
	const std::string	m_id;
};


class _DatabaseChannel_
{
public:
	_DatabaseChannel_()				{}
	~_DatabaseChannel_()				{}

	void close()					{}
};


class PostgreSQLDatabase : public Database
{
public:
	PostgreSQLDatabase( const PostgreSQLconfig* conf );
	~PostgreSQLDatabase()				{}

	DatabaseType type() const			{ return DBTYPE_POSTGRESQL; }
private:
};


class SQLiteDatabase : public Database
{
public:
	SQLiteDatabase( const SQLiteConfig* conf );
	~SQLiteDatabase()				{}

	DatabaseType type() const			{ return DBTYPE_SQLITE; }
private:
};


class DBprovider
{
public:
	DBprovider( const Configuration& conf );
	~DBprovider();

	const Database* database( std::string& ID ) const;

	_DatabaseChannel_* channel() const		{ return NULL; }

private:
	std::list<Database*>	m_db;
};

}} // namespace _Wolframe::db

#endif // _DATABASE_HPP_INCLUDED
