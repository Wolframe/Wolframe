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

#include "configurationBase.hpp"

namespace _Wolframe	{
namespace db	{

/// database type
enum DatabaseType	{
	DBTYPE_REFERENCE,
	DBTYPE_POSTGRESQL,
	DBTYPE_SQLITE,
	DBTYPE_UNKNOWN
};


class	DatabaseConfig : public _Wolframe::config::ConfigurationBase
{
public:
	DatabaseConfig( DatabaseType Type,
			const char* name, const char* logParent, const char* logName )
		: ConfigurationBase( name, logParent, logName ), m_type( Type )  {}
//     virtual ~DatabaseConfigBase();

	DatabaseType type() const			{ return m_type; }
	void ID( const std::string& id )		{ m_ID = id; }
	const std::string& ID() const			{ return m_ID; }
private:
	const DatabaseType	m_type;
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
	PostgreSQLconfig( const char* name, const char* logParent, const char* logName );
	bool parse( const boost::property_tree::ptree& pt, const std::string& node );
	bool check() const;
	void print( std::ostream& os, size_t indent ) const;
};


/// SQLite database configuration
struct	SQLiteConfig : public DatabaseConfig
{
	std::string	filename;
	bool		flag;
public:
	SQLiteConfig( const char* name, const char* logParent, const char* logName );
	bool parse( const boost::property_tree::ptree& pt, const std::string& node );
	bool check() const;
	void print( std::ostream& os, size_t indent ) const;
	virtual void setCanonicalPathes( const std::string& referencePath );
};


/// database reference class
struct ReferenceConfig : public DatabaseConfig
{
	std::string	m_ref;
public:
	ReferenceConfig( const char* name, const char* logParent, const char* logName )
		: DatabaseConfig( DBTYPE_REFERENCE, name, logParent, logName )	{}
	bool parse( const boost::property_tree::ptree& pt, const std::string& node );
	bool check() const;
	void print( std::ostream& os, size_t indent ) const;
};


/// database configurations
struct Configuration : public _Wolframe::config::ConfigurationBase
{
public:
	std::list<DatabaseConfig*>	dbConfig_;

	/// constructor & destructor
	Configuration() : ConfigurationBase( "Database(s)", NULL, "Database configuration" )	{}
	~Configuration();

	/// methods
	bool parse( const boost::property_tree::ptree& pt, const std::string& node );
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
	bool parse( const boost::property_tree::ptree& pt, const std::string& node );
	bool check() const;
	void print( std::ostream& os, size_t indent ) const;
	virtual void setCanonicalPathes( const std::string& referencePath );
};


/// database base classes
class Database	{
public:
	Database( DatabaseType t, const std::string id ) : m_type( t ), m_id( id )	{}
	static DatabaseType strToType( const char *str );
	static std::string& typeToStr( DatabaseType type );

	const std::string& ID() const	{ return m_id; }
	DatabaseType type() const	{ return m_type; }
private:
	const DatabaseType	m_type;
	const std::string	m_id;
};


class PostgreSQLDatabase : public Database	{
public:
	PostgreSQLDatabase( const PostgreSQLconfig* config );
private:
};


class SQLiteDatabase : public Database	{
public:
	SQLiteDatabase( const SQLiteConfig* config );
private:
};


class DBprovider	{
public:
	DBprovider( const Configuration& config );
	~DBprovider();

	Database* database( std::string& ID ) const;

private:
	std::list<Database*>	m_db;
};

}} // namespace _Wolframe::db

#endif // _DATABASE_HPP_INCLUDED
