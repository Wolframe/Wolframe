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
#include "logger.hpp"
#include "moduleInterface.hpp"

namespace _Wolframe {
namespace module {

bool ModulesConfiguration::add( ConfigDescriptionBase* description )
{
	for ( std::list< ConfigDescriptionBase* >::const_iterator it = m_modules.begin();
								it != m_modules.end(); it++ )	{
		if ( boost::algorithm::iequals( (*it)->objectName(), description->objectName() ))	{
			LOG_ALERT << "A module for object '" << description->objectName()
				  << "' already exists";
			return false;
		}
	}
	m_modules.push_back( description );
	LOG_DEBUG << "Configuration for module '" << description->objectName() << "' registered";
	return true;
}

ConfigDescriptionBase* ModulesConfiguration::get( const std::string& name ) const
{
	for ( std::list< ConfigDescriptionBase* >::const_iterator it = m_modules.begin();
								it != m_modules.end(); it++ )	{
		if ( boost::algorithm::iequals( (*it)->objectName(), name ))
			return *it;
	}
	return NULL;
}

}} // namespace _Wolframe::module


/****  Impersonating the module loader  ******************************************************/
#include "config/configurationParser.hpp"
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

bool module::LoadModules( ModulesConfiguration& modules )
{
	bool retVal = true;

#ifdef WITH_PGSQL
	modules.add( new module::ConfigurationDescription< db::PostgreSQLconfig >
		     ( "PostgreSQL", "PostgreSQL database", "PostgreSQL",
		       &config::ConfigurationParser::parseBase<db::PostgreSQLconfig> ) );
#endif
#ifdef WITH_SQLITE3
	modules.add( new module::ConfigurationDescription< db::SQLiteConfig >
		     ( "SQLite", "SQLite database", "SQLite",
		       &config::ConfigurationParser::parseBase<db::SQLiteConfig> ) );
#endif
	modules.add( new module::ConfigurationDescription< EchoProcConfig >
		     ( "echoProcessor", "Echo Processor", "echoProcessor",
		       &config::ConfigurationParser::parseBase< EchoProcConfig > ) );

	modules.add( new module::ConfigurationDescription< AAAA::TextFileAuthConfig >
		     ( "TextFileAuth", "Authentication file", "file",
		       &config::ConfigurationParser::parseBase<AAAA::TextFileAuthConfig> ) );
	modules.add( new module::ConfigurationDescription< AAAA::DatabaseAuthConfig >
		     ( "DatabaseAuth", "Authentication database", "database",
		       &config::ConfigurationParser::parseBase<AAAA::DatabaseAuthConfig> ) );
	modules.add( new module::ConfigurationDescription< AAAA::FileAuditConfig >
		     ( "FileAudit", "Audit file", "file",
		       &config::ConfigurationParser::parseBase<AAAA::FileAuditConfig> ) );
	modules.add( new module::ConfigurationDescription< AAAA::DBauditConfig >
		     ( "DatabaseAudit", "Audit database", "database",
		       &config::ConfigurationParser::parseBase<AAAA::DBauditConfig> ) );
	return retVal;
}

/****  End impersonating the module loader  **************************************************/
