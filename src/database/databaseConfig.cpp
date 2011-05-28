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

#include "database/database.hpp"
#include "database/PostgreSQL.hpp"
#include "database/SQLite.hpp"
#include "database/DBreference.hpp"

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

namespace _Wolframe {
namespace config {

template<>
bool ConfigurationParser::parse( db::Configuration& cfg,
				 const boost::property_tree::ptree& pt, const std::string& /*node*/ )
{
	using namespace _Wolframe::config;
	bool retVal = true;

	for ( boost::property_tree::ptree::const_iterator L1it = pt.begin(); L1it != pt.end(); L1it++ )	{
		if ( boost::algorithm::iequals( L1it->first, "PostgreSQL" ))	{
			db::PostgreSQLconfig* conf = new db::PostgreSQLconfig( "PostgreSQL server", cfg.logPrefix().c_str(), "PostgreSQL" );
			if ( ConfigurationParser::parse( *conf, L1it->second, L1it->first ))
				cfg.m_dbConfig.push_back( conf );
			else	{
				delete conf;
				retVal = false;
			}
		}
		else if ( boost::algorithm::iequals( L1it->first, "SQLite" ))	{
			db::SQLiteConfig* conf = new db::SQLiteConfig( "SQLite database", cfg.logPrefix().c_str(), "SQLite" );
			if ( ConfigurationParser::parse( *conf, L1it->second, L1it->first ))
				cfg.m_dbConfig.push_back( conf );
			else	{
				delete conf;
				retVal = false;
			}
		}
		else
			LOG_WARNING << cfg.logPrefix() << "unknown configuration option: '"
				    << L1it->first << "'";
	}
	return retVal;
}

template<>
bool ConfigurationParser::parse( db::SingleDBConfiguration& cfg,
				 const boost::property_tree::ptree& pt, const std::string& node )
{
	using namespace _Wolframe::config;
	bool retVal = true;

	std::string label = pt.get_value<std::string>();
	if ( ! label.empty() )	{
		db::ReferenceConfig* conf = new db::ReferenceConfig( "Reference", cfg.logPrefix().c_str(), "reference" );
		if ( ConfigurationParser::parse( *conf, pt, node ))
			cfg.m_dbConfig = conf;
		else	{
			delete conf;
			retVal = false;
		}
	}
	else	{
		for ( boost::property_tree::ptree::const_iterator L1it = pt.begin(); L1it != pt.end(); L1it++ )	{
			if ( cfg.m_dbConfig != NULL )	{
				LOG_ERROR << cfg.logPrefix() << "database already defined: '"
					    << L1it->first << "'";
				retVal = false;
			}
			else	{
				if ( boost::algorithm::iequals( L1it->first, "PostgreSQL" ))	{
					db::PostgreSQLconfig* conf = new db::PostgreSQLconfig( "PostgreSQL server",
											 cfg.logPrefix().c_str(), "PostgreSQL" );
					if ( ConfigurationParser::parse( *conf, L1it->second, L1it->first ))
						cfg.m_dbConfig = conf;
					else	{
						delete conf;
						retVal = false;
					}
				}
				else if ( boost::algorithm::iequals( L1it->first, "SQLite" ))	{
					db::SQLiteConfig* conf = new db::SQLiteConfig( "SQLite database", cfg.logPrefix().c_str(), "SQLite" );
					if ( ConfigurationParser::parse( *conf, L1it->second, L1it->first ))
						cfg.m_dbConfig = conf;
					else	{
						delete conf;
						retVal = false;
					}
				}
				else
					LOG_WARNING << cfg.logPrefix() << "unknown configuration option: '"
						    << L1it->first << "'";
			}
		}
	}

	if ( ! cfg.m_dbConfig )	{
		LOG_ERROR << cfg.logPrefix() << "database label without definition";
		retVal = false;
	}

	return retVal;
}

} // namespace config

namespace db {

//***  Generic database functions  **************************************
Configuration::~Configuration()
{
	for ( std::list<DatabaseConfig*>::const_iterator it = m_dbConfig.begin();
								it != m_dbConfig.end(); it++ )
		delete *it;
}

void Configuration::print( std::ostream& os, size_t /* indent */ ) const
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
bool Configuration::check() const
{
	bool correct = true;
	for ( std::list<DatabaseConfig*>::const_iterator it = m_dbConfig.begin();
								it != m_dbConfig.end(); it++ )	{
		if ( !(*it)->check() )
			correct = false;
	}
	return correct;
}

void Configuration::setCanonicalPathes( const std::string& refPath )
{
	for ( std::list<DatabaseConfig*>::const_iterator it = m_dbConfig.begin();
								it != m_dbConfig.end(); it++ )	{
		(*it)->setCanonicalPathes( refPath );
	}
}

//***************************************************
SingleDBConfiguration::~SingleDBConfiguration()
{
	if ( m_dbConfig )
		delete m_dbConfig;
}

void SingleDBConfiguration::print( std::ostream& os, size_t indent ) const
{
	if ( ! sectionName().empty() )	{
		std::string indStr( indent, ' ' );
		os << indStr << sectionName() << ":" << std::endl;
		indent += 3;
	}
	if ( m_dbConfig )	{
		m_dbConfig->print( os, indent );
	}
	else
		os << "   None configured" << std::endl;
}

/// Check if the database configuration makes sense
bool SingleDBConfiguration::check() const
{
	if ( m_dbConfig )
		return m_dbConfig->check();
	return false;
}

void SingleDBConfiguration::setCanonicalPathes( const std::string& refPath )
{
	m_dbConfig->setCanonicalPathes( refPath );
}

}} // namespace _Wolframe::db
