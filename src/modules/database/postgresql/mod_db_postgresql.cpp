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
// PostgreSQL module
//

#include "PostgreSQL.hpp"
#include "module/moduleInterface.hpp"
#include "logger-v1.hpp"

namespace _Wolframe {
namespace module {

static BuilderBase* createPGSQLmodule( void )
{
	static ConfiguredBuilderDescription< db::PostgreSQLConstructor,
			db::PostgreSQLConfig > mod( "PostgreSQL database", "database",
						    "PostgreSQL", "PostgreSQL" );
	return &mod;
}

static BuilderBase* (*containers[])() = {
	createPGSQLmodule, NULL
};

ModuleEntryPoint entryPoint( 0, "PostgreSQL database", containers);

}} // namespace _Wolframe::module
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
///\file mod_db_postgresql.cpp
///\brief Database interface module for PostgreSQL
#include "appdevel/databaseModuleMacros.hpp"
#include "appdevel/moduleFrameMacros.hpp"
#include "PostgreSQL.hpp"

WF_MODULE_BEGIN( "PostgreSQLDatabase", "Database interface module for PostgreSQL")
 WF_SIMPLE_DATABASE( "postgresql", _Wolframe::db::PostgreSQLDatabase, _Wolframe::db::PostgreSQLConfig)
WF_MODULE_END


namespace _Wolframe {
namespace db {

PostgreSQLdbUnit* PostgreSQLConstructor::object( const config::NamedConfiguration& conf )
{
	const PostgreSQLConfig& cfg = dynamic_cast< const PostgreSQLConfig& >( conf );

	PostgreSQLdbUnit* m_db = new PostgreSQLdbUnit( cfg.m_ID, cfg.host(), cfg.port(), cfg.dbName(),
						       cfg.user(), cfg.password(),
						       cfg.sslMode, cfg.sslCert, cfg.sslKey,
						       cfg.sslRootCert, cfg.sslCRL,
						       cfg.connectTimeout,
						       cfg.connections, cfg.acquireTimeout,
						       cfg.statementTimeout);
	LOG_TRACE << "PostgreSQL database unit for '" << cfg.m_ID << "' created";
	return m_db;
}

}} // namespace _Wolframe::db
