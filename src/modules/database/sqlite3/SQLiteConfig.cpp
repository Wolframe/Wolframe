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
// SQLite configuration parser
//

#include "SQLite.hpp"
#include "config/valueParser.hpp"
#include "config/ConfigurationTree.hpp"
#include "utils/fileUtils.hpp"

#include <boost/algorithm/string.hpp>
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>

namespace _Wolframe {
namespace db {

static const unsigned short DEFAULT_SQLITE_CONNECTIONS = 3;

bool SQLiteConfig::parse( const config::ConfigurationTree& pt, const std::string& /*node*/,
			  const module::ModulesDirectory* /*modules*/ )
{
	using namespace _Wolframe::config;
	bool fkDefined = false;
	bool profilingDefined = false;
	bool connDefined = false;
	bool retVal = true;

	for ( boost::property_tree::ptree::const_iterator L1it = pt.begin(); L1it != pt.end(); L1it++ )	{
		if ( boost::algorithm::iequals( L1it->first, "identifier" ))	{
			bool isDefined = ( !m_ID.empty() );
			std::string id;
			if ( !Parser::getValue( logPrefix().c_str(), *L1it, id, &isDefined ))
				retVal = false;
			else
				m_ID = id;
		}
		else if ( boost::algorithm::iequals( L1it->first, "file" ) ||
			  boost::algorithm::iequals( L1it->first, "filename" ))	{
			bool isDefined = ( !m_filename.empty() );
			if ( !Parser::getValue( logPrefix().c_str(), *L1it, m_filename, &isDefined ))
				retVal = false;
		}
		else if ( boost::algorithm::iequals( L1it->first, "foreignKeys" ))	{
			if ( !Parser::getValue( logPrefix().c_str(), *L1it, m_foreignKeys,
						Parser::BoolDomain(), &fkDefined ))
				retVal = false;
		}
		else if ( boost::algorithm::iequals( L1it->first, "profiling" ))	{
			if ( !Parser::getValue( logPrefix().c_str(), *L1it, m_profiling,
						Parser::BoolDomain(), &profilingDefined ))
				retVal = false;
		}
		else if ( boost::algorithm::iequals( L1it->first, "connections" ))	{
			if ( !Parser::getValue( logPrefix().c_str(), *L1it, m_connections,
						Parser::RangeDomain<unsigned short>( 0 ), &connDefined ))
				retVal = false;
		}
		else if ( boost::algorithm::iequals( L1it->first, "program" ))	{
			std::string programFile;
			if ( !Parser::getValue( logPrefix().c_str(), *L1it, programFile ))
				retVal = false;
			else	{
				m_programFiles.push_back( programFile );
			}
		}
		else if ( boost::algorithm::iequals( L1it->first, "extension" ))	{
			std::string extensionFile;
			if ( !Parser::getValue( logPrefix().c_str(), *L1it, extensionFile ))
				retVal = false;
			else	{
				m_extensionFiles.push_back( extensionFile );
			}
		}
		else	{
			MOD_LOG_WARNING << logPrefix() << "unknown configuration option: '"
					<< L1it->first << "'";
		}
	}
	if ( ! connDefined )
		m_connections = DEFAULT_SQLITE_CONNECTIONS;
	if ( ! fkDefined )
		m_foreignKeys = true;
	if ( ! profilingDefined )
		m_profiling = false;
	return retVal;
}

SQLiteConfig::SQLiteConfig( const char* name, const char* logParent, const char* logName )
	: config::NamedConfiguration( name, logParent, logName )
{}

void SQLiteConfig::print( std::ostream& os, size_t indent ) const
{
	std::string indStr( indent, ' ' );

	os << indStr << sectionName() << ":" << std::endl;
	if ( ! m_ID.empty() )
		os << indStr << "   ID: " << m_ID << std::endl;
	os << indStr << "   Filename: " << m_filename << std::endl;
	os << indStr << "      Referential integrity: " << (m_foreignKeys ? "enabled" : "disabled") << std::endl;
	os << indStr << "      Profiling: " << (m_profiling ? "enabled" : "disabled") << std::endl;
	if ( m_programFiles.empty() )
		os << indStr << "   Program file: none" << std::endl;
	else if ( m_programFiles.size() == 1 )
		os << indStr << "   Program file: " << m_programFiles.front() << std::endl;
	else	{
		std::list< std::string >::const_iterator it = m_programFiles.begin();
		os << indStr << "   Program files: " << *it++ << std::endl;
		while ( it != m_programFiles.end() )
			os << indStr << "                  " << *it++ << std::endl;
	}
}

bool SQLiteConfig::check() const
{
	if ( m_filename.empty() )	{
		MOD_LOG_ERROR << logPrefix() << "SQLite database filename cannot be empty";
		return false;
	}
	return true;
}

void SQLiteConfig::setCanonicalPathes( const std::string& refPath )
{
	if ( ! m_filename.empty() )	{
		std::string oldPath = m_filename;
		m_filename = utils::getCanonicalPath( m_filename, refPath);
		if ( oldPath != m_filename )	{
/*MBa ?!?*/		MOD_LOG_NOTICE << logPrefix() << "Using absolute database filename '" << m_filename
				       << "' instead of '" << oldPath << "'";
		}
	}
	for ( std::list< std::string >::iterator it = m_programFiles.begin();
						it != m_programFiles.end(); it++ )	{
		std::string oldPath = *it;
		*it = utils::getCanonicalPath( *it, refPath );
		if ( oldPath != *it )	{
/*MBa ?!?*/		MOD_LOG_NOTICE << logPrefix() << "Using absolute program filename '" << *it
				       << "' instead of '" << oldPath << "'";
		}
	}
}

}} // namespace _Wolframe::db
