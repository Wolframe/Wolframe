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
// Wolframe SQLite client
//

#ifndef _SQLITE_HPP_INCLUDED
#define _SQLITE_HPP_INCLUDED

#include "database/database.hpp"
#include "database/transaction.hpp"
#include "database/transactionExecStatemachine.hpp"
#include "config/configurationBase.hpp"
#include "config/structSerialize.hpp"
#include "types/keymap.hpp"
#include "module/constructor.hpp"
#include "SQLiteProgram.hpp"
#include <list>
#include <vector>
#include "system/objectPool.hpp"
#include "sqlite3.h"

#ifdef _WIN32
#pragma warning(disable:4250)
#endif

namespace _Wolframe {
namespace db {

static const char* SQLite_DB_CLASS_NAME = "SQLite";

struct SQLiteConfigStruct
{
	SQLiteConfigStruct();

	std::string	m_ID;
	std::string	m_filename;
	bool		m_foreignKeys;
	bool		m_profiling;
	unsigned short	m_connections;
	std::vector< std::string > m_extensionFiles;		//< list of Sqlite extension modules to load

	//\brief Structure description for serialization/parsing
	static const serialize::StructDescriptionBase* getStructDescription();
};


//\brief SQLite database configuration
class SQLiteConfig
	:public config::NamedConfiguration
	,public SQLiteConfigStruct
{
public:
	const char* className() const				{ return SQLite_DB_CLASS_NAME; }

	SQLiteConfig( const char* name, const char* logParent, const char* logName );
	~SQLiteConfig(){}

	bool parse( const config::ConfigurationNode& pt, const std::string& node,
		    const module::ModulesDirectory* modules );
	bool check() const;
	void print( std::ostream& os, size_t indent ) const;
	void setCanonicalPathes( const std::string& referencePath );

	const std::string& ID() const				{ return m_ID; }
	const std::string& filename() const			{ return m_filename; }
	bool foreignKeys() const				{ return m_foreignKeys; }
	bool profiling() const					{ return m_profiling; }
	unsigned short connections() const			{ return m_connections; }
	const std::vector< std::string > extensionFiles() const	{ return m_extensionFiles; }
private:
	config::ConfigurationTree::Position m_config_pos;
};



struct SQLiteLanguageDescription :public LanguageDescription
{
	///\brief String used for declaring a reference to an argument by index (starting with 1).
	virtual std::string stm_argument_reference( int index) const
	{
		std::ostringstream rt;
		rt << "$" << index;
		return rt.str();
	}
};

class SQLiteDBunit;

class SQLiteDatabase : public Database
{
public:
	SQLiteDatabase() : m_unit( NULL )	{}
	 ~SQLiteDatabase()			{}

	void setUnit( SQLiteDBunit* unit )	{ m_unit = unit; }
	bool hasUnit() const			{ return m_unit != NULL; }
	SQLiteDBunit& dbUnit() const		{ return *m_unit; }

	const std::string& ID() const;
	const char* className() const		{ return SQLite_DB_CLASS_NAME; }

	Transaction* transaction( const std::string& name );
	void closeTransaction( Transaction* t );

	virtual const LanguageDescription* getLanguageDescription() const
	{
		static SQLiteLanguageDescription langdescr;
		return &langdescr;
	}

private:
	SQLiteDBunit*	m_unit;			///< parent database unit
};


class SQLiteDBunit : public DatabaseUnit
{
	friend class SQLiteTransaction;
	friend class SQLiteUIlibrary;
public:
	SQLiteDBunit( const std::string& id, const std::string& filename,
		      bool foreignKeys, bool profiling,
		      unsigned short connections,
		      const std::vector<std::string>& extensionFiles_ );
	~SQLiteDBunit();

	const std::string& ID() const		{ return m_ID; }
	const char* className() const		{ return SQLite_DB_CLASS_NAME; }
	Database* database();

	PoolObject<sqlite3*>* newConnection()	{return new PoolObject<sqlite3*>( m_connPool);}

private:
	const std::string	m_ID;
	const std::string	m_filename;
	std::list< sqlite3* >	m_connections;		///< list of DB connections
	ObjectPool< sqlite3* >	m_connPool;		///< pool of connections

	SQLiteProgram		m_program;		///< database programs
	SQLiteDatabase		m_db;
	std::vector<std::string>m_extensionFiles;
};


///\class SQLiteConstructor
///\brief SQLite database constructor
class SQLiteConstructor : public ConfiguredObjectConstructor< db::DatabaseUnit >
{
public:
	ObjectConstructorBase::ObjectType objectType() const
						{ return DATABASE_OBJECT; }
	const char* objectClassName() const	{ return SQLite_DB_CLASS_NAME; }
	SQLiteDBunit* object( const config::NamedConfiguration& conf );
};


///\class SQLiteTransaction
class SQLiteTransaction
	:public Transaction
{
public:
	SQLiteTransaction( SQLiteDatabase& database, const std::string& name_);
	virtual ~SQLiteTransaction(){}
};



}} // _Wolframe::db

#endif // _SQLITE_HPP_INCLUDED
