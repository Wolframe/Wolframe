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
// Wolframe SQLite client
//

#ifndef _SQLITE_HPP_INCLUDED
#define _SQLITE_HPP_INCLUDED

#include "database/database.hpp"
#include "config/configurationBase.hpp"
#include "constructor.hpp"

#include <list>
#include "objectPool.hpp"
#include "sqlite3.h"

#include "../dbTransaction.hpp"

#ifdef _WIN32
#pragma warning(disable:4250)
#endif

namespace _Wolframe {
namespace db {

static const char* SQLite_DB_CLASS_NAME = "SQLite";

/// SQLite database configuration
class SQLiteConfig : public config::NamedConfiguration
{
	friend class SQLiteConstructor;
public:
	const char* className() const		{ return SQLite_DB_CLASS_NAME; }

	SQLiteConfig( const char* name, const char* logParent, const char* logName );
	~SQLiteConfig()				{}

	bool parse( const config::ConfigurationTree& pt, const std::string& node,
		    const module::ModulesDirectory* modules );
	bool check() const;
	void print( std::ostream& os, size_t indent ) const;
	void setCanonicalPathes( const std::string& referencePath );
private:
	std::string	m_ID;
	std::string	filename;
	bool		flag;
	std::string	programFile;
};


class SQLiteDBunit;
class SQLiteDatabase;

class SQLiteTransaction : public BaseTransaction, public virtual Transaction
{
public:
	SQLiteTransaction( SQLiteDatabase& database );
	 ~SQLiteTransaction()			{}

	const std::string& databaseID() const;

	void execute();

	void close();
private:
	SQLiteDatabase&	m_db;			///< parent database
	SQLiteDBunit&	m_unit;			///< parent database unit
};


class SQLiteDatabase : public Database
{
public:
	SQLiteDatabase() : m_unit( NULL )	{}
	 ~SQLiteDatabase()			{}

	void setUnit( SQLiteDBunit* unit )	{ m_unit = unit; }
	bool hasUnit() const			{ return m_unit != NULL; }
	SQLiteDBunit& dbUnit() const		{ return *m_unit; }

	const std::string& ID() const;
	PreparedStatementHandler* getPreparedStatementHandler()
						{ return 0; }	//undefined
	/// more of a placeholder for now
	Transaction* transaction( const std::string& name );

	void closeTransaction( Transaction* t );
private:
	SQLiteDBunit*	m_unit;			///< parent database unit
};


class SQLiteDBunit : public DatabaseUnit
{
	friend class SQLiteTransaction;
public:
	SQLiteDBunit( const std::string& id,
		      const std::string& filename, unsigned short connections, bool flag );
	~SQLiteDBunit();

	const std::string& ID() const		{ return m_ID; }
	const char* className() const		{ return SQLite_DB_CLASS_NAME; }
	Database* database();
private:
	const std::string	m_ID;
	const std::string	m_filename;
	bool			m_flag;
	std::list< sqlite3* >	m_connections;		///< list of DB connections
	ObjectPool< sqlite3* >	m_connPool;		///< pool of connections

	SQLiteDatabase		m_db;
};

//***  SQLite database constructor  *******************************************
class SQLiteConstructor : public ConfiguredObjectConstructor< db::DatabaseUnit >
{
public:
	ObjectConstructorBase::ObjectType objectType() const
						{ return DATABASE_OBJECT; }
	const char* objectClassName() const	{ return SQLite_DB_CLASS_NAME; }
	SQLiteDBunit* object( const config::NamedConfiguration& conf );
};

}} // _Wolframe::db

#endif // _SQLITE_HPP_INCLUDED
