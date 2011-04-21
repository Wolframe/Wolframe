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
PostgreSQLconfig::PostgreSQLconfig() : DatabaseConfigBase( DBTYPE_POSTGRESQL )
{
	port = 0;
	connections = 0;
	acquireTimeout = 0;
}

void PostgreSQLconfig::print( std::ostream& os, size_t /* indent */ ) const
{
	os << "   PostgreSQL server:" << std::endl;
	if ( host.empty())
		os << "      Database host: local unix domain socket" << std::endl;
	else
		os << "      Database host: " << host << ":" << port << std::endl;
	os << "      Database name: " << (name.empty() ? "(not specified - server user default)" : name) << std::endl;
	os << "      Database user: " << (user.empty() ? "(not specified - same as server user)" : user)
	   << ", password: " << (password.empty() ? "(not specified - no password used)" : password) << std::endl;
	os << "      Database connections: " << connections << std::endl;
	os << "      Acquire database connection timeout: " << acquireTimeout << std::endl;
}

bool PostgreSQLconfig::check( const std::string& module ) const
{
	if ( connections == 0 )	{
		LOG_ERROR << module << "number of database connections cannot be 0";
		return false;
	}
	return true;
}

bool PostgreSQLconfig::parse( const std::string& module, const boost::property_tree::ptree& pt )
{
	using namespace _Wolframe::config;
	bool retVal = true;
	bool portDefined, connDefined, aTdefined;
	portDefined = connDefined = aTdefined = false;

	for ( boost::property_tree::ptree::const_iterator L1it = pt.begin(); L1it != pt.end(); L1it++ )	{
		if ( boost::algorithm::iequals( L1it->first, "host" ))	{
			bool isDefined = ( !host.empty());
			if ( !Parser::getValue( module.c_str(), *L1it, host, &isDefined ))
				retVal = false;
		}
		else if ( boost::algorithm::iequals( L1it->first, "port" ))	{
			if ( !Parser::getValue( module.c_str(), *L1it, port,
						Parser::RangeDomain<unsigned short>( 1 ), &portDefined ))
				retVal = false;
		}
		else if ( boost::algorithm::iequals( L1it->first, "name" ))	{
			bool isDefined = ( !name.empty());
			if ( !Parser::getValue( module.c_str(), *L1it, name, &isDefined ))
				retVal = false;
		}
		else if ( boost::algorithm::iequals( L1it->first, "user" ))	{
			bool isDefined = ( !user.empty());
			if ( !Parser::getValue( module.c_str(), *L1it, user, &isDefined ))
				retVal = false;
		}
		else if ( boost::algorithm::iequals( L1it->first, "password" ))	{
			bool isDefined = ( !password.empty());
			if ( !Parser::getValue( module.c_str(), *L1it, password, &isDefined ))
				retVal = false;
		}
		else if ( boost::algorithm::iequals( L1it->first, "connections" ))	{
			if ( !Parser::getValue( module.c_str(), *L1it, connections, &connDefined ))
				retVal = false;
		}
		else if ( boost::algorithm::iequals( L1it->first, "acquireTimeout" ))	{
			if ( !Parser::getValue( module.c_str(), *L1it, acquireTimeout, &aTdefined ))
				retVal = false;
		}
		else	{
			LOG_WARNING << module << ": unknown configuration option: '"
				    << L1it->first << "'";
		}
	}
	if ( connections == 0 )
		connections = DEFAULT_DB_CONNECTIONS;

	return retVal;
}

//***  SQLite functions  ************************************************
SQLiteConfig::SQLiteConfig() : DatabaseConfigBase( DBTYPE_SQLITE )
{
	flag = false;
}

void SQLiteConfig::print( std::ostream& os, size_t /* indent */ ) const
{
	os << "   SQLite database:" << std::endl;
	os << "      Filename: " << filename << std::endl;
	os << "      Flags: " << (flag ? "True Flag" : "False Flag");
}

bool SQLiteConfig::check( const std::string& module ) const
{
	if ( filename.empty() )	{
		LOG_ERROR << module << "SQLite database filename cannot be empty";
		return false;
	}
	return true;
}

bool SQLiteConfig::parse( const std::string& module, const boost::property_tree::ptree& pt )
{
	using namespace _Wolframe::config;
	bool retVal = true;

	for ( boost::property_tree::ptree::const_iterator L1it = pt.begin(); L1it != pt.end(); L1it++ )	{
		if ( boost::algorithm::iequals( L1it->first, "filename" ))	{
			bool isDefined = ( !filename.empty());
			if ( !Parser::getValue( module.c_str(), *L1it, filename, &isDefined ))
				retVal = false;
			else	{
				if ( ! boost::filesystem::path( filename ).is_absolute() )
					LOG_WARNING << module << ": database file path is not absolute: "
						    << filename;
			}
		}
		else if ( boost::algorithm::iequals( L1it->first, "flag" ))	{
			if ( !Parser::getValue( module.c_str(), *L1it, flag, Parser::BoolDomain() ))
				retVal = false;
		}
		else	{
			LOG_WARNING << module << ": unknown configuration option: '"
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


//***  Generic database functions  **************************************
void Configuration::print( std::ostream& os, size_t /* indent */ ) const
{
	os << sectionName() << std::endl;
	if ( dbConfig_.size() > 1 )
		os << "   Strategy: " << Database::strategyToStr( strategy ) << std::endl;
	for ( std::list<DatabaseConfigBase*>::const_iterator it = dbConfig_.begin();
								it != dbConfig_.end(); it++ )	{
		(*it)->print( os, 0 );
	}
}


/// Check if the database configuration makes sense
bool Configuration::check() const
{
	bool correct = true;
	for ( std::list<DatabaseConfigBase*>::const_iterator it = dbConfig_.begin();
								it != dbConfig_.end(); it++ )	{
		if ( !(*it)->check( sectionName() ))
			correct = false;
	}
	return correct;
}


bool Configuration::parse( const boost::property_tree::ptree& pt, const std::string& /* nodeName */ )
{
	using namespace _Wolframe::config;
	bool retVal = true;
	DatabaseType type = DBTYPE_UNKNOWN;

	enum { NofDBtypes = 2 };
	static const char* DBtypesEnum[ NofDBtypes ] = { "PostgreSQL", "SQLite" };
	Parser::EnumDomain DBtypesDomain( NofDBtypes, DBtypesEnum );

	enum { NofDBstrategies = 2 };
	static const char* DBstrategies[ NofDBstrategies ] = { "round-robin", "failover" };
	Parser::EnumDomain DBstrategyDomain( NofDBstrategies, DBstrategies );


	for ( boost::property_tree::ptree::const_iterator L1it = pt.begin(); L1it != pt.end(); L1it++ )	{
		if ( boost::algorithm::iequals( L1it->first, "type" ))	{
			if ( !Parser::getValue( sectionName().c_str(), *L1it, type, DBtypesDomain ))
				retVal = false;
		}
		else if ( boost::algorithm::iequals( L1it->first, "strategy" ))	{
			if ( !Parser::getValue( sectionName().c_str(), *L1it, strategy, DBstrategyDomain ))
				retVal = false;
		}
		else if ( boost::algorithm::iequals( L1it->first, "server" ))	{
			switch ( type )	{
			case DBTYPE_POSTGRESQL:	{
				PostgreSQLconfig* cfg = new PostgreSQLconfig();
				if ( cfg->parse( sectionName(), L1it->second ))
					dbConfig_.push_back( cfg );
				else	{
					delete cfg;
					retVal = false;
				}
				break;
			}
			case DBTYPE_SQLITE:	{
				SQLiteConfig* cfg = new SQLiteConfig();
				if ( cfg->parse( sectionName(), L1it->second ))
					dbConfig_.push_back( cfg );
				else	{
					retVal = false;
					delete cfg;
				}
				break;
			}
			case DBTYPE_UNKNOWN:
				LOG_ERROR << sectionName() << "database type must be defined first";
				retVal = false;
				break;
			}
		}
		else
			LOG_WARNING << sectionName() << ": unknown configuration option: '"
				    << L1it->first << "'";
	}
	return retVal;
}

void Configuration::setCanonicalPathes( const std::string& refPath )
{
	for ( std::list<DatabaseConfigBase*>::const_iterator it = dbConfig_.begin();
								it != dbConfig_.end(); it++ )	{
		(*it)->setCanonicalPathes( refPath );
	}
}

}} // namespace _Wolframe::db
