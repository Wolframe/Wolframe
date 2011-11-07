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
// loadModules.cpp
//

#include "logger-v1.hpp"
#include "moduleInterface.hpp"

/****  Impersonating the module loader  ******************************************************/
#include "processor/echoProcessor.hpp"

#include "AAAA/TextFileAuthentication.hpp"
#include "AAAA/DBauthentication.hpp"
#include "AAAA/FileAudit.hpp"
#include "AAAA/DBaudit.hpp"

#ifdef WITH_PGSQL
#include "database/PostgreSQL.hpp"
#endif
#ifdef WITH_SQLITE3
#include "database/SQLite.hpp"
#endif

using namespace _Wolframe;

bool module::LoadModules( ModulesDirectory& modules )
{
	bool retVal = true;

#ifdef WITH_PGSQL
	setModuleLogger( &_Wolframe::log::LogBackend::instance() );
	modules.addContainer( reinterpret_cast<module::ModuleContainer*(*)()>( PostgreSQLmodule )() );
#endif
#ifdef WITH_SQLITE3
	modules.addContainer( reinterpret_cast<module::ModuleContainer*(*)()>( SQLiteModule )() );
#endif
	modules.addContainer( reinterpret_cast<module::ModuleContainer*(*)()>( echoProcessorModule )() );

	modules.addContainer( reinterpret_cast<module::ModuleContainer*(*)()>( TextFileAuthModule )() );
	modules.addContainer( reinterpret_cast<module::ModuleContainer*(*)()>( DBauthModule )() );

	modules.addContainer( reinterpret_cast<module::ModuleContainer*(*)()>( FileAuditModule )() );
	modules.addContainer( reinterpret_cast<module::ModuleContainer*(*)()>( DBauditModule )() );

	return retVal;
}

/****  End impersonating the module loader  **************************************************/
