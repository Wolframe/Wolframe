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

#include "database.hpp"
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

static const unsigned short DEFAULT_DB_CONNECTIONS = 4;

namespace _Wolframe {

namespace config {

/// Parse the PostgreSQL configuration
template<>
bool ConfigurationParser::parse( db::PostgreSQLconfig& cfg,
				 const boost::property_tree::ptree& pt, const std::string& /*node*/ )
{
	using namespace _Wolframe::config;
	bool retVal = true;
	bool portDefined, connDefined, aTdefined;
	portDefined = connDefined = aTdefined = false;

	for ( boost::property_tree::ptree::const_iterator L1it = pt.begin(); L1it != pt.end(); L1it++ )	{
		if ( boost::algorithm::iequals( L1it->first, "identifier" ))	{
			bool isDefined = ( !cfg.ID().empty() );
			std::string id;
			if ( !Parser::getValue( cfg.logPrefix().c_str(), *L1it, id, &isDefined ))
				retVal = false;
			else
				cfg.ID( id );
		}
		else if ( boost::algorithm::iequals( L1it->first, "host" ))	{
			bool isDefined = ( !cfg.host.empty());
			if ( !Parser::getValue( cfg.logPrefix().c_str(), *L1it, cfg.host, &isDefined ))
				retVal = false;
		}
		else if ( boost::algorithm::iequals( L1it->first, "port" ))	{
			if ( !Parser::getValue( cfg.logPrefix().c_str(), *L1it, cfg.port,
						Parser::RangeDomain<unsigned short>( 1 ), &portDefined ))
				retVal = false;
		}
		else if ( boost::algorithm::iequals( L1it->first, "name" ))	{
			bool isDefined = ( !cfg.dbName.empty());
			if ( !Parser::getValue( cfg.logPrefix().c_str(), *L1it, cfg.dbName, &isDefined ))
				retVal = false;
		}
		else if ( boost::algorithm::iequals( L1it->first, "user" ))	{
			bool isDefined = ( !cfg.user.empty());
			if ( !Parser::getValue( cfg.logPrefix().c_str(), *L1it, cfg.user, &isDefined ))
				retVal = false;
		}
		else if ( boost::algorithm::iequals( L1it->first, "password" ))	{
			bool isDefined = ( !cfg.password.empty());
			if ( !Parser::getValue( cfg.logPrefix().c_str(), *L1it, cfg.password, &isDefined ))
				retVal = false;
		}
		else if ( boost::algorithm::iequals( L1it->first, "connections" ))	{
			if ( !Parser::getValue( cfg.logPrefix().c_str(), *L1it, cfg.connections, &connDefined ))
				retVal = false;
		}
		else if ( boost::algorithm::iequals( L1it->first, "acquireTimeout" ))	{
			if ( !Parser::getValue( cfg.logPrefix().c_str(), *L1it, cfg.acquireTimeout, &aTdefined ))
				retVal = false;
		}
		else	{
			LOG_WARNING << cfg.logPrefix() << "unknown configuration option: '"
				    << L1it->first << "'";
		}
	}
	if ( cfg.connections == 0 )
		cfg.connections = DEFAULT_DB_CONNECTIONS;

	return retVal;
}

/// Parse the SQLite configuration
template<>
bool ConfigurationParser::parse( db::SQLiteConfig& cfg,
				 const boost::property_tree::ptree& pt, const std::string& /*node*/ )
{
	using namespace _Wolframe::config;
	bool retVal = true;

	for ( boost::property_tree::ptree::const_iterator L1it = pt.begin(); L1it != pt.end(); L1it++ )	{
		if ( boost::algorithm::iequals( L1it->first, "identifier" ))	{
			bool isDefined = ( !cfg.ID().empty() );
			std::string id;
			if ( !Parser::getValue( cfg.logPrefix().c_str(), *L1it, id, &isDefined ))
				retVal = false;
			else
				cfg.ID( id );
		}
		else if ( boost::algorithm::iequals( L1it->first, "file" ) ||
			  boost::algorithm::iequals( L1it->first, "filename" ))	{
			bool isDefined = ( !cfg.filename.empty() );
			if ( !Parser::getValue( cfg.logPrefix().c_str(), *L1it, cfg.filename, &isDefined ))
				retVal = false;
			else	{
				if ( ! boost::filesystem::path( cfg.filename ).is_absolute() )
					LOG_WARNING << cfg.logPrefix() << "database file path is not absolute: "
						    << cfg.filename;
			}
		}
		else if ( boost::algorithm::iequals( L1it->first, "flag" ))	{
			if ( !Parser::getValue( cfg.logPrefix().c_str(), *L1it, cfg.flag, Parser::BoolDomain() ))
				retVal = false;
		}
		else	{
			LOG_WARNING << cfg.logPrefix() << "unknown configuration option: '"
				    << L1it->first << "'";
		}
	}
	return retVal;
}

/// Parse the reference to db configuration
template<>
bool ConfigurationParser::parse( db::ReferenceConfig& cfg,
				 const boost::property_tree::ptree& pt, const std::string& node )
{
	bool labelDefined = ( ! cfg.m_ref.empty() );
	if ( !config::Parser::getValue( cfg.logPrefix().c_str(), node.c_str(),
					pt.get_value<std::string>(), cfg.m_ref, &labelDefined ))
		return false;
	if ( cfg.m_ref.empty() )	{
		LOG_ERROR << cfg.logPrefix() << "Database reference label is empty";
		return false;
	}
	return true;
}

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
				cfg.dbConfig_.push_back( conf );
			else	{
				delete conf;
				retVal = false;
			}
		}
		else if ( boost::algorithm::iequals( L1it->first, "SQLite" ))	{
			db::SQLiteConfig* conf = new db::SQLiteConfig( "SQLite database", cfg.logPrefix().c_str(), "SQLite" );
			if ( ConfigurationParser::parse( *conf, L1it->second, L1it->first ))
				cfg.dbConfig_.push_back( conf );
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

//***  PostgreSQL functions  ********************************************
PostgreSQLconfig::PostgreSQLconfig( const char* cfgName, const char* logParent, const char* logName )
	: DatabaseConfig( cfgName, logParent, logName )
{
	port = 0;
	connections = 0;
	acquireTimeout = 0;
}

void PostgreSQLconfig::print( std::ostream& os, size_t indent ) const
{
	std::string indStr( indent, ' ' );

	os << indStr << sectionName() << ":" << std::endl;
	if ( ! ID().empty() )
		os << indStr << "   ID: " << ID() << std::endl;
	if ( host.empty())
		os << indStr << "   Database host: local unix domain socket" << std::endl;
	else
		os << indStr << "   Database host: " << host << ":" << port << std::endl;
	os << indStr << "   Database name: " << (dbName.empty() ? "(not specified - server user default)" : dbName) << std::endl;
	os << indStr << "   Database user: " << (user.empty() ? "(not specified - same as server user)" : user)
	   << ", password: " << (password.empty() ? "(not specified - no password used)" : password) << std::endl;
	os << indStr << "   Database connections: " << connections << std::endl;
	os << indStr << "   Acquire database connection timeout: " << acquireTimeout << std::endl;
}

bool PostgreSQLconfig::check() const
{
	if ( connections == 0 )	{
		LOG_ERROR << logPrefix() << "number of database connections cannot be 0";
		return false;
	}
	return true;
}


//***  SQLite functions  ************************************************
SQLiteConfig::SQLiteConfig( const char* name, const char* logParent, const char* logName )
	: DatabaseConfig( name, logParent, logName )
{
	flag = false;
}

void SQLiteConfig::print( std::ostream& os, size_t indent ) const
{
	std::string indStr( indent, ' ' );

	os << indStr << sectionName() << ":" << std::endl;
	if ( ! ID().empty() )
		os << indStr << "   ID: " << ID() << std::endl;
	os << indStr << "   Filename: " << filename << std::endl;
	os << indStr << "   Flags: " << (flag ? "True Flag" : "False Flag") << std::endl;
}

bool SQLiteConfig::check() const
{
	if ( filename.empty() )	{
		LOG_ERROR << logPrefix() << "SQLite database filename cannot be empty";
		return false;
	}
	return true;
}

void SQLiteConfig::setCanonicalPathes( const std::string& refPath )
{
	using namespace boost::filesystem;

	if ( ! filename.empty() )	{
		if ( ! path( filename ).is_absolute() )
			filename = resolvePath( absolute( filename,
							  path( refPath ).branch_path()).string());
		else
			filename = resolvePath( filename );
	}
}


//***  Database reference functions  **************************************
bool ReferenceConfig::check() const
{
	if ( m_ref.empty() )	{
		LOG_ERROR << "Database reference label is empty";
		return false;
	}
	return true;
}

void ReferenceConfig::print( std::ostream& os, size_t indent ) const
{
	std::string indStr( indent, ' ' );
	os << indStr << sectionName() << ": " << (m_ref.empty() ? "Undefined!" : m_ref) << std::endl;
}


//***  Generic database functions  **************************************
Configuration::~Configuration()
{
	for ( std::list<DatabaseConfig*>::const_iterator it = dbConfig_.begin();
								it != dbConfig_.end(); it++ )
		delete *it;
}

void Configuration::print( std::ostream& os, size_t /* indent */ ) const
{
	os << sectionName() << std::endl;
	if ( dbConfig_.size() > 0 )	{
		for ( std::list<DatabaseConfig*>::const_iterator it = dbConfig_.begin();
								it != dbConfig_.end(); it++ )	{
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
	for ( std::list<DatabaseConfig*>::const_iterator it = dbConfig_.begin();
								it != dbConfig_.end(); it++ )	{
		if ( !(*it)->check() )
			correct = false;
	}
	return correct;
}

void Configuration::setCanonicalPathes( const std::string& refPath )
{
	for ( std::list<DatabaseConfig*>::const_iterator it = dbConfig_.begin();
								it != dbConfig_.end(); it++ )	{
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
