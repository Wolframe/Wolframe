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
	const char* className() const		{ return SQLite_DB_CLASS_NAME; }

	SQLiteConfig( const char* name, const char* logParent, const char* logName );
	~SQLiteConfig(){}

	bool parse( const config::ConfigurationTree& pt, const std::string& node,
		    const module::ModulesDirectory* modules );
	bool check() const;
	void print( std::ostream& os, size_t indent ) const;
	void setCanonicalPathes( const std::string& referencePath );

	const std::string& ID() const				{return m_ID;}
	const std::string& filename() const			{return m_filename;}
	bool flag() const					{return m_flag;}
	unsigned short connections() const			{return m_connections;}
	const std::list< std::string > programFiles() const	{return m_programFiles;}

private:
	std::string	m_ID;
	std::string	m_filename;
	bool		m_flag;
	unsigned short	m_connections;
	std::list< std::string > m_programFiles;		//< list of program files
};


class SQLiteDBunit;
class SQLiteDatabase;

class SQLiteTransaction : public Transaction
{
public:
	SQLiteTransaction( SQLiteDatabase& database );
	 ~SQLiteTransaction()			{}

	virtual const std::string& databaseID() const;

	virtual void putInput( const TransactionInput& input_)		{m_input = input_;}
	virtual const TransactionOutput& getResult() const		{return m_output;}

	virtual void execute();

	virtual void close();
private:
	SQLiteDatabase&		m_db;		///< parent database
	SQLiteDBunit&		m_unit;		///< parent database unit
	TransactionInput	m_input;	///< input data structure
	TransactionOutput	m_output;	///< output data structure
};


class SQLiteUIlibrary : public UI::UserInterfaceLibrary
{
public:
	SQLiteUIlibrary( const SQLiteDatabase& database );
	~SQLiteUIlibrary()			{}

	virtual const std::list< UI::UIform::Info > formInfos( const std::string& platform ) const;
	virtual const std::list< UI::UIform::Info > formInfos( const std::string& platform,
							       const std::string& role ) const;
	virtual const std::list< UI::UIform::Info > formInfos( const std::string& platform,
							       const std::list< std::string >& roles ) const;

	virtual const std::list< UI::UIform::Info > formVersions( const std::string& platform,
								  const std::string& name ) const;

	virtual const UI::UIform form( const std::string& platform,
				       const std::string& name ) const;
	virtual const UI::UIform form( const std::string& platform,
				       const std::string& name, const Version& version ) const;
	virtual const UI::UIform form( const std::string& platform,
				       const UI::UIform::Info& info ) const;

	virtual void close()			{ delete this; }
private:
	SQLiteDBunit&		m_unit;		///< parent database unit
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
	virtual void addProgram( const std::string& program );
	virtual const std::string* getProgram( const std::string& name) const;

	Transaction* transaction( const std::string& name );
	void closeTransaction( Transaction* t );

	///\brief Get a user interface library
	virtual const UI::UserInterfaceLibrary* UIlibrary() const;
private:
	SQLiteDBunit*	m_unit;			///< parent database unit
};


class SQLiteDBunit : public DatabaseUnit
{
	friend class SQLiteTransaction;
public:
	SQLiteDBunit( const std::string& id,
		      const std::string& filename, bool flag,
		      unsigned short connections );
	~SQLiteDBunit();

	const std::string& ID() const		{ return m_ID; }
	const char* className() const		{ return SQLite_DB_CLASS_NAME; }
	Database* database();

	virtual void loadProgram( const std::string& filename );
	/// MBa: to be defined after some more cleaning...
	virtual void loadAllPrograms()		{}

	virtual void addProgram( const std::string& program )
						{ m_program.load( program ); }

	virtual const std::string* getProgram( const std::string& name) const
	{
		const types::keymap<std::string>* mm = m_program.statementmap();
		if (!mm) return 0;
		types::keymap<std::string>::const_iterator mi = mm->find( name);
		if (mi == mm->end()) return 0;
		return &mi->second;
	}

	const types::keymap<std::string>* stmmap() const
	{
		return m_program.statementmap();
	}

private:
	const std::string	m_ID;
	const std::string	m_filename;
	bool			m_flag;
	std::list< sqlite3* >	m_connections;		///< list of DB connections
	ObjectPool< sqlite3* >	m_connPool;		///< pool of connections

	SQLiteProgram		m_program;		///< database programs
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
