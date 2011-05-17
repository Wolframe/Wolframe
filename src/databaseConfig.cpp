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
#include "connectionBase.hpp"
#include "config/valueParser.hpp"
#include "logger.hpp"

#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
#include "miscUtils.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/algorithm/string.hpp>

#include <ostream>
#include <string>

static const unsigned short DEFAULT_DB_CONNECTIONS = 4;

namespace _Wolframe	{
namespace db	{

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

bool PostgreSQLconfig::parse( const boost::property_tree::ptree& pt, const std::string& /* nodeName */ )
{
	using namespace _Wolframe::config;
	bool retVal = true;
	bool portDefined, connDefined, aTdefined;
	portDefined = connDefined = aTdefined = false;

	for ( boost::property_tree::ptree::const_iterator L1it = pt.begin(); L1it != pt.end(); L1it++ )	{
		if ( boost::algorithm::iequals( L1it->first, "identifier" ))	{
			bool isDefined = ( ! ID().empty() );
			std::string id;
			if ( !Parser::getValue( logPrefix().c_str(), *L1it, id, &isDefined ))
				retVal = false;
			else
				ID( id );
		}
		else if ( boost::algorithm::iequals( L1it->first, "host" ))	{
			bool isDefined = ( !host.empty());
			if ( !Parser::getValue( logPrefix().c_str(), *L1it, host, &isDefined ))
				retVal = false;
		}
		else if ( boost::algorithm::iequals( L1it->first, "port" ))	{
			if ( !Parser::getValue( logPrefix().c_str(), *L1it, port,
						Parser::RangeDomain<unsigned short>( 1 ), &portDefined ))
				retVal = false;
		}
		else if ( boost::algorithm::iequals( L1it->first, "name" ))	{
			bool isDefined = ( !dbName.empty());
			if ( !Parser::getValue( logPrefix().c_str(), *L1it, dbName, &isDefined ))
				retVal = false;
		}
		else if ( boost::algorithm::iequals( L1it->first, "user" ))	{
			bool isDefined = ( !user.empty());
			if ( !Parser::getValue( logPrefix().c_str(), *L1it, user, &isDefined ))
				retVal = false;
		}
		else if ( boost::algorithm::iequals( L1it->first, "password" ))	{
			bool isDefined = ( !password.empty());
			if ( !Parser::getValue( logPrefix().c_str(), *L1it, password, &isDefined ))
				retVal = false;
		}
		else if ( boost::algorithm::iequals( L1it->first, "connections" ))	{
			if ( !Parser::getValue( logPrefix().c_str(), *L1it, connections, &connDefined ))
				retVal = false;
		}
		else if ( boost::algorithm::iequals( L1it->first, "acquireTimeout" ))	{
			if ( !Parser::getValue( logPrefix().c_str(), *L1it, acquireTimeout, &aTdefined ))
				retVal = false;
		}
		else	{
			LOG_WARNING << logPrefix() << "unknown configuration option: '"
				    << L1it->first << "'";
		}
	}
	if ( connections == 0 )
		connections = DEFAULT_DB_CONNECTIONS;

	return retVal;
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

bool SQLiteConfig::parse( const boost::property_tree::ptree& pt, const std::string& /* nodeName */ )
{
	using namespace _Wolframe::config;
	bool retVal = true;

	for ( boost::property_tree::ptree::const_iterator L1it = pt.begin(); L1it != pt.end(); L1it++ )	{
		if ( boost::algorithm::iequals( L1it->first, "identifier" ))	{
			bool isDefined = ( ! ID().empty() );
			std::string id;
			if ( !Parser::getValue( logPrefix().c_str(), *L1it, id, &isDefined ))
				retVal = false;
			else
				ID( id );
		}
		else if ( boost::algorithm::iequals( L1it->first, "file" ) ||
			  boost::algorithm::iequals( L1it->first, "filename" ))	{
			bool isDefined = ( ! filename.empty() );
			if ( !Parser::getValue( logPrefix().c_str(), *L1it, filename, &isDefined ))
				retVal = false;
			else	{
				if ( ! boost::filesystem::path( filename ).is_absolute() )
					LOG_WARNING << logPrefix() << "database file path is not absolute: "
						    << filename;
			}
		}
		else if ( boost::algorithm::iequals( L1it->first, "flag" ))	{
			if ( !Parser::getValue( logPrefix().c_str(), *L1it, flag, Parser::BoolDomain() ))
				retVal = false;
		}
		else	{
			LOG_WARNING << logPrefix() << "unknown configuration option: '"
				    << L1it->first << "'";
		}
	}
	return retVal;
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
bool ReferenceConfig::parse( const boost::property_tree::ptree& pt, const std::string& node )
{
	bool labelDefined = ( ! m_ref.empty() );
	if ( !config::Parser::getValue( logPrefix().c_str(), node.c_str(),
					pt.get_value<std::string>(), m_ref, &labelDefined ))
		return false;
	if ( m_ref.empty() )	{
		LOG_ERROR << logPrefix() << "Database reference label is empty";
		return false;
	}
	return true;
}

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

bool Configuration::parse( const boost::property_tree::ptree& pt, const std::string& /* nodeName */ )
{
	using namespace _Wolframe::config;
	bool retVal = true;

	for ( boost::property_tree::ptree::const_iterator L1it = pt.begin(); L1it != pt.end(); L1it++ )	{
		if ( boost::algorithm::iequals( L1it->first, "PostgreSQL" ))	{
			PostgreSQLconfig* cfg = new PostgreSQLconfig( "PostgreSQL server", logPrefix().c_str(), "PostgreSQL" );
			if ( cfg->parse( L1it->second, L1it->first ))
				dbConfig_.push_back( cfg );
			else	{
				delete cfg;
				retVal = false;
			}
		}
		else if ( boost::algorithm::iequals( L1it->first, "SQLite" ))	{
			SQLiteConfig* cfg = new SQLiteConfig( "SQLite database", logPrefix().c_str(), "SQLite" );
			if ( cfg->parse( L1it->second, L1it->first ))
				dbConfig_.push_back( cfg );
			else	{
				delete cfg;
				retVal = false;
			}
		}
		else
			LOG_WARNING << logPrefix() << "unknown configuration option: '"
				    << L1it->first << "'";
	}
	return retVal;
}

void Configuration::setCanonicalPathes( const std::string& refPath )
{
	for ( std::list<DatabaseConfig*>::const_iterator it = dbConfig_.begin();
								it != dbConfig_.end(); it++ )	{
		(*it)->setCanonicalPathes( refPath );
	}
}


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

bool SingleDBConfiguration::parse( const boost::property_tree::ptree& pt, const std::string& nodeName )
{
	using namespace _Wolframe::config;
	bool retVal = true;

	std::string label = pt.get_value<std::string>();
	if ( ! label.empty() )	{
		ReferenceConfig* cfg = new ReferenceConfig( "Reference", logPrefix().c_str(), "reference" );
		if ( cfg->parse( pt, nodeName ))
			m_dbConfig = cfg;
		else	{
			delete cfg;
			retVal = false;
		}
	}
	else	{
		for ( boost::property_tree::ptree::const_iterator L1it = pt.begin(); L1it != pt.end(); L1it++ )	{
			if ( m_dbConfig != NULL )	{
				LOG_ERROR << logPrefix() << "database already defined: '"
					    << L1it->first << "'";
				retVal = false;
			}
			else	{
				if ( boost::algorithm::iequals( L1it->first, "PostgreSQL" ))	{
					PostgreSQLconfig* cfg = new PostgreSQLconfig( "PostgreSQL server",
										      logPrefix().c_str(), "PostgreSQL" );
					if ( cfg->parse( L1it->second, L1it->first ))
						m_dbConfig = cfg;
					else	{
						delete cfg;
						retVal = false;
					}
				}
				else if ( boost::algorithm::iequals( L1it->first, "SQLite" ))	{
					SQLiteConfig* cfg = new SQLiteConfig( "SQLite database", logPrefix().c_str(), "SQLite" );
					if ( cfg->parse( L1it->second, L1it->first ))
						m_dbConfig = cfg;
					else	{
						delete cfg;
						retVal = false;
					}
				}
				else
					LOG_WARNING << logPrefix() << "unknown configuration option: '"
						    << L1it->first << "'";
			}
		}
	}

	if ( ! m_dbConfig )	{
		LOG_ERROR << logPrefix() << "database label without definition";
		retVal = false;
	}

	return retVal;
}

void SingleDBConfiguration::setCanonicalPathes( const std::string& refPath )
{
	m_dbConfig->setCanonicalPathes( refPath );
}

}} // namespace _Wolframe::db
