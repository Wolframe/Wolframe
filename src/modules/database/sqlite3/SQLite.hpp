/************************************************************************

 Copyright (C) 2011 - 2013 Project Wolframe.
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
#include "UserInterface.hpp"
#include "config/configurationBase.hpp"
#include "types/keymap.hpp"
#include "constructor.hpp"
#include "SQLiteProgram.hpp"
#include <list>
#include "objectPool.hpp"
#include "sqlite3.h"

#ifdef _WIN32
#pragma warning(disable:4250)
#endif

namespace _Wolframe {
namespace db {

static const char* SQLite_DB_CLASS_NAME = "SQLite";

/// SQLite database configuration
class SQLiteConfig : public config::NamedConfiguration
{
public:
	const char* className() const				{ return SQLite_DB_CLASS_NAME; }

	SQLiteConfig( const char* name, const char* logParent, const char* logName );
	~SQLiteConfig(){}

	bool parse( const config::ConfigurationTree& pt, const std::string& node,
		    const module::ModulesDirectory* modules );
	bool check() const;
	void print( std::ostream& os, size_t indent ) const;
	void setCanonicalPathes( const std::string& referencePath );

	const std::string& ID() const				{ return m_ID; }
	const std::string& filename() const			{ return m_filename; }
	bool foreignKeys() const				{ return m_foreignKeys; }
	bool profiling() const					{ return m_profiling; }
	unsigned short connections() const			{ return m_connections; }
	const std::list< std::string > programFiles() const	{ return m_programFiles; }

private:
	std::string	m_ID;
	std::string	m_filename;
	bool		m_foreignKeys;
	bool		m_profiling;
	unsigned short	m_connections;
	std::list< std::string > m_programFiles;		//< list of program files
};


class SQLiteDBunit;
class SQLiteDatabase;

class SQLiteTransaction : public Transaction
{
public:
	SQLiteTransaction( SQLiteDatabase& database );
	 ~SQLiteTransaction();

	virtual const std::string& databaseID() const;

	virtual void putInput( const TransactionInput& input_)		{ m_input = input_; }
	virtual const TransactionOutput& getResult() const		{ return m_output; }

	virtual void execute();
	virtual void begin();
	virtual void commit();
	virtual void rollback();
	virtual void close();

private:
	void execute_statement( const char* stmstr);
	void execute_with_autocommit();
	void execute_transaction_operation();

private:
	SQLiteDatabase&		m_db;		//< parent database
	SQLiteDBunit&		m_unit;		//< parent database unit
	TransactionInput	m_input;	//< input data structure
	TransactionOutput	m_output;	//< output data structure
	PoolObject<sqlite3*>* m_conn;		//< connection object from pool
};


class SQLiteUIlibrary : public UI::UserInterfaceLibrary
{
public:
	SQLiteUIlibrary( const SQLiteDatabase& database );
	~SQLiteUIlibrary()			{}

	virtual const std::list< UI::InterfaceObject::Info > interface( const std::string& platform,
									const std::list< std::string >& roles,
									const std::string& culture,
									const std::string& tag = "" ) const;

	virtual const std::list< UI::InterfaceObject::Info > interface( const std::string& platform,
									const std::string& roles,
									const std::string& culture,
									const std::string& tag = "" ) const;

	virtual const UI::InterfaceObject object( const UI::InterfaceObject::Info& info ) const;

	virtual void addObject( const UI::InterfaceObject& newObject ) const;

	virtual bool deleteObject( const UI::InterfaceObject::Info& info ) const;

	virtual void close()			{ delete this; }
private:
	SQLiteDBunit&		m_unit;		///< parent database unit
};


struct SQLiteLanguageDescription :public LanguageDescription
{
	///\brief String used for declaring a reference to an argument by index (starting with 1).
	virtual std::string stm_argument_reference( int index)
	{
		std::ostringstream rt;
		rt << "?" << index;
		return rt.str();
	}
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

	virtual void loadProgram( const std::string& filename );
	virtual void loadAllPrograms();
	virtual void addProgram( const std::string& program );

	Transaction* transaction( const std::string& name );
	void closeTransaction( Transaction* t );

	///\brief Add a set of named statements to the sqlite program
	virtual void addStatements( const types::keymap<std::string>& stmmap_);

	virtual const LanguageDescription* getLanguageDescription() const
	{
		static SQLiteLanguageDescription langdescr;
		return &langdescr;
	}

	///\brief Get a user interface library
	virtual const UI::UserInterfaceLibrary* UIlibrary() const;
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
		      const std::list<std::string>& programFiles_);
	~SQLiteDBunit();

	const std::string& ID() const		{ return m_ID; }
	const char* className() const		{ return SQLite_DB_CLASS_NAME; }
	Database* database();

	virtual void loadProgram( const std::string& filename );
	/// MBa: to be defined after some more cleaning...
	virtual void loadAllPrograms();

	virtual void addProgram( const std::string& program )
						{ m_program.load( program ); }

	///\brief Add a set of named statements to the sqlite program
	virtual void addStatements( const types::keymap<std::string>& stmmap_)
	{
		m_program.addStatements( stmmap_);
	}


	const types::keymap<std::string>* stmmap() const
	{
		return m_program.statementmap();
	}

private:
	const std::string	m_ID;
	const std::string	m_filename;
	std::list< sqlite3* >	m_connections;		///< list of DB connections
	ObjectPool< sqlite3* >	m_connPool;		///< pool of connections

	SQLiteProgram		m_program;		///< database programs
	SQLiteDatabase		m_db;
	std::list<std::string>	m_programFiles;
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
