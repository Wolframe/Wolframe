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
// This is doing just module loader impersonation
// It's just a hardcoded possible result of the module loader

#include <boost/algorithm/string.hpp>
#include "logger-v1.hpp"
#include "moduleInterface.hpp"

namespace _Wolframe {
namespace module {

bool ModulesDirectory::addConfiguration( ConfigDescriptionBase* description )
{
	for ( std::list< ConfigDescriptionBase* >::const_iterator it = m_config.begin();
								it != m_config.end(); it++ )	{
		if ( boost::algorithm::iequals( (*it)->section, description->section ) &&
				boost::algorithm::iequals( (*it)->keyword, description->keyword ))	{
			LOG_ALERT << "A configuration module for section '" << description->section
				  << "' keyword '" << description->keyword << "' already exists";
			return false;
		}
	}
	m_config.push_back( description );
	LOG_DEBUG << "Configuration for section '" << description->section
		  << "' keyword '" << description->keyword << "' registered";
	return true;
}

ConfigDescriptionBase* ModulesDirectory::getConfig( const std::string& section,
						  const std::string& keyword ) const
{
	for ( std::list< ConfigDescriptionBase* >::const_iterator it = m_config.begin();
								it != m_config.end(); it++ )	{
		if ( boost::algorithm::iequals( (*it)->keyword, keyword ) &&
				boost::algorithm::iequals( (*it)->section, section ))
			return *it;
	}
	return NULL;
}

}} // namespace _Wolframe::module


/****  Impersonating the module loader  ******************************************************/
#include "config/ConfigurationTree.hpp"
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
	modules.addConfiguration( new module::ConfigurationDescription< db::PostgreSQLconfig >
				  ( "PostgreSQL database", "database", "PostgreSQL" ));
#endif
#ifdef WITH_SQLITE3
	modules.addConfiguration( new module::ConfigurationDescription< db::SQLiteConfig >
				  ( "SQLite database", "database", "SQLite" ));
#endif
	modules.addConfiguration( new module::ConfigurationDescription< EchoProcConfig >
				  ( "Echo Processor", "processor", "echoProcessor" ));

	modules.addConfiguration( new module::ConfigurationDescription< AAAA::TextFileAuthConfig >
				  ( "Authentication file", "Authentication", "file" ));
	modules.addConfiguration( new module::ConfigurationDescription< AAAA::DatabaseAuthConfig >
				  ( "Authentication database", "Authentication", "database" ));
	modules.addConfiguration( new module::ConfigurationDescription< AAAA::FileAuditConfig >
				  ( "Audit file", "Audit", "file" ));
	modules.addConfiguration( new module::ConfigurationDescription< AAAA::DBauditConfig >
				  ( "Audit database", "Audit", "database" ));
	return retVal;
}

/****  End impersonating the module loader  **************************************************/
