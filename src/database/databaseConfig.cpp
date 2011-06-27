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
// database configuration functions
//

#include "DBprovider.hpp"

#include "config/valueParser.hpp"
#include "config/configurationParser.hpp"
#include "logger.hpp"

#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
#include "miscUtils.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/algorithm/string.hpp>

#include <ostream>
#include <string>

/****  Impersonating the module loader  ******************************************************/
#ifdef WITH_PGSQL
#include "PostgreSQL.hpp"
#endif
#ifdef WITH_SQLITE3
#include "SQLite.hpp"
#endif

using namespace _Wolframe;

static const size_t noDBconfigs = 2;
static module::ModuleConfigConstructorDescript
dbConfig[ noDBconfigs ] = { module::ModuleConfigConstructorDescript( "PostgreSQL", "PostgreSQL database", "PostgreSQL",
			    &db::PostgreSQLconfig::create,
			    &config::ConfigurationParser::parseBase<db::PostgreSQLconfig> ),
			    module::ModuleConfigConstructorDescript( "SQLite", "SQLite database", "SQLite",
			    &db::SQLiteConfig::create,
			    &config::ConfigurationParser::parseBase<db::SQLiteConfig> ) };
/****  End impersonating the module loader  **************************************************/

namespace _Wolframe {
namespace config {

template<>
bool ConfigurationParser::parse( db::DBproviderConfig& cfg,
				 const boost::property_tree::ptree& pt, const std::string& /*node*/ )
{
	using namespace _Wolframe::config;
	bool retVal = true;

	for ( boost::property_tree::ptree::const_iterator L1it = pt.begin(); L1it != pt.end(); L1it++ )	{
		size_t i;
		for ( i = 0; i < noDBconfigs; i++ )	{
			if ( boost::algorithm::iequals( dbConfig[i].typeName, L1it->first ))	{
				db::DatabaseConfig* conf = dynamic_cast< db::DatabaseConfig* >( dbConfig[i].createFunc( dbConfig[i].sectionTitle,
															cfg.logPrefix().c_str(),
															dbConfig[i].sectionName ));
				if ( dbConfig[i].parseFunc( *conf, L1it->second, L1it->first ))
					cfg.m_dbConfig.push_back( conf );
				else	{
					delete conf;
					retVal = false;
				}
				break;
			}
		}
		if ( i >= noDBconfigs )
			LOG_WARNING << cfg.logPrefix() << "unknown configuration option: '"
				    << L1it->first << "'";
	}
	return retVal;
}

} // namespace config

namespace db {

//***  Generic database functions  **************************************
DBproviderConfig::~DBproviderConfig()
{
	for ( std::list<DatabaseConfig*>::const_iterator it = m_dbConfig.begin();
								it != m_dbConfig.end(); it++ )
		delete *it;
}

void DBproviderConfig::print( std::ostream& os, size_t /* indent */ ) const
{
	os << sectionName() << std::endl;
	if ( m_dbConfig.size() > 0 )	{
		for ( std::list<DatabaseConfig*>::const_iterator it = m_dbConfig.begin();
								it != m_dbConfig.end(); it++ )	{
			(*it)->print( os, 3 );
		}
	}
	else
		os << "   None configured" << std::endl;
}


/// Check if the database configuration makes sense
bool DBproviderConfig::check() const
{
	bool correct = true;
	for ( std::list<DatabaseConfig*>::const_iterator it = m_dbConfig.begin();
								it != m_dbConfig.end(); it++ )	{
		if ( !(*it)->check() )
			correct = false;
	}
	return correct;
}

void DBproviderConfig::setCanonicalPathes( const std::string& refPath )
{
	for ( std::list<DatabaseConfig*>::const_iterator it = m_dbConfig.begin();
								it != m_dbConfig.end(); it++ )	{
		(*it)->setCanonicalPathes( refPath );
	}
}

}} // namespace _Wolframe::db
