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
///\file mod_db_sqlite3.cpp
///\brief Database interface module for Sqlite3
#include "appdevel/databaseModuleMacros.hpp"
#include "appdevel/moduleFrameMacros.hpp"
#include "SQLite.hpp"

WF_MODULE_BEGIN( "Sqlite3Database", "Database interface module for Sqlite3")
 WF_SIMPLE_DATABASE( "sqlite3", _Wolframe::db::SQLiteDatabase, _Wolframe::db::SQLiteConfig)
WF_MODULE_END

namespace _Wolframe {
namespace module {

static BuilderBase* createSQLiteModule( void )
{
	static module::ConfiguredBuilderDescription< db::SQLiteConstructor,
			db::SQLiteConfig > mod( "SQLite database", "database",
						"SQLite", "SQLite" );
	return &mod;
}

static BuilderBase* (*containers[])() = {
	createSQLiteModule, NULL
};

ModuleEntryPoint entryPoint( 0, "SQLite database", containers);

SQLiteDBunit* SQLiteConstructor::object( const config::NamedConfiguration& conf )
{
	const SQLiteConfig& cfg = dynamic_cast< const SQLiteConfig& >( conf );

	SQLiteDBunit* m_db = new SQLiteDBunit( cfg.ID(), cfg.filename(),
					       cfg.foreignKeys(),
					       cfg.profiling(),
					       cfg.connections(),
					       cfg.extensionFiles());
	LOG_TRACE << "SQLite database unit for '" << cfg.ID() << "' created";
	return m_db;
}

}} // namespace _Wolframe::db
