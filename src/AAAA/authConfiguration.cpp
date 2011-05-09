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
// authentication configuration
//

#include "logger.hpp"
#include "config/valueParser.hpp"
#include "authenticator.hpp"
#include "database.hpp"

#include "boost/algorithm/string.hpp"
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
#include "miscUtils.hpp"

namespace _Wolframe {
namespace AAAA {

/// Text file authentication
bool TextFileAuthConfig::parse( const boost::property_tree::ptree& pt, const std::string& node )
{
	using namespace _Wolframe::config;
	bool retVal = true;

	if ( boost::algorithm::iequals( node, "file" ) || boost::algorithm::iequals( node, "filename" ))	{
		bool isDefined = ( ! m_file.empty() );
		if ( !Parser::getValue( logPrefix().c_str(), node.c_str(),
					pt.get_value<std::string>(), m_file, &isDefined ))
			retVal = false;
		else	{
			if ( ! boost::filesystem::path( m_file ).is_absolute() )
				LOG_WARNING << logPrefix() << "authentication file path is not absolute: "
					    << m_file;
		}
	}
	else	{
		LOG_WARNING << logPrefix() << "unknown configuration option: '" << node << "'";
	}
	return retVal;
}

bool TextFileAuthConfig::check() const
{
	if ( m_file.empty() )	{
		LOG_ERROR << logPrefix() << "Authentication filename cannot be empty";
		return false;
	}
	return true;
}

void TextFileAuthConfig::print( std::ostream& os, size_t indent ) const
{
	std::string indStr( indent, ' ' );
	os << indStr << sectionName() << ": " << m_file << std::endl;
}

void TextFileAuthConfig::setCanonicalPathes( const std::string& refPath )
{
	using namespace boost::filesystem;

	if ( ! m_file.empty() )	{
		if ( ! path( m_file ).is_absolute() )
			m_file = resolvePath( absolute( m_file,
							path( refPath ).branch_path()).string());
		else
			m_file = resolvePath( m_file );
	}
}


/// constructor
AuthenticationConfiguration::~AuthenticationConfiguration()
{
	for ( std::list<AuthenticationConfigBase*>::const_iterator it = m_config.begin();
								it != m_config.end(); it++ )
		delete *it;
}


/// methods
bool AuthenticationConfiguration::parse( const boost::property_tree::ptree& pt, const std::string& /*nodeName*/ )
{
	using namespace _Wolframe::config;
	bool retVal = true;

	for ( boost::property_tree::ptree::const_iterator L1it = pt.begin(); L1it != pt.end(); L1it++ )	{
		if ( boost::algorithm::iequals( L1it->first, "file" ))	{
			TextFileAuthConfig* cfg = new TextFileAuthConfig( "File", logPrefix().c_str(), "file" );
			if ( cfg->parse( L1it->second, L1it->first ))
				m_config.push_back( cfg );
			else	{
				delete cfg;
				retVal = false;
			}
		}
		else if ( boost::algorithm::iequals( L1it->first, "database" ))	{
			DatabaseAuthConfig* cfg = new DatabaseAuthConfig( "Database", logPrefix().c_str(), "database" );
			if ( cfg->parse( L1it->second, L1it->first ))
				m_config.push_back( cfg );
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


bool AuthenticationConfiguration::check() const
{
	bool correct = true;
	for ( std::list<AuthenticationConfigBase*>::const_iterator it = m_config.begin();
								it != m_config.end(); it++ )	{
		if ( !(*it)->check() )
			correct = false;
	}
	return correct;
}


void AuthenticationConfiguration::print( std::ostream& os, size_t indent ) const
{
	std::string indStr( indent, ' ' );

	os << indStr << sectionName() << ":" << std::endl;
	if ( m_config.size() > 0 )	{
		for ( std::list<AuthenticationConfigBase*>::const_iterator it = m_config.begin();
								it != m_config.end(); it++ )	{
			(*it)->print( os, indent + 3 );
		}
	}
	else
		os << "   None configured" << std::endl;
}


void AuthenticationConfiguration::setCanonicalPathes( const std::string& refPath )
{
	for ( std::list<AuthenticationConfigBase*>::const_iterator it = m_config.begin();
								it != m_config.end(); it++ )	{
		(*it)->setCanonicalPathes( refPath );
	}
}

}} // namespace _Wolframe::AAAA

