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
// audit configuration implementation
//

#include "logger.hpp"
#include "config/valueParser.hpp"
#include "config/configurationBase.hpp"
#include "config/configurationParser.hpp"
#include "AAAA/auditor.hpp"
#include "AAAA/DBaudit.hpp"
#include "AAAA/FileAudit.hpp"
#include "database/database.hpp"

#include "boost/algorithm/string.hpp"
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
#include "miscUtils.hpp"

namespace _Wolframe {
namespace config {

template<>
bool ConfigurationParser::parse( AAAA::AuditConfiguration& cfg,
				 const boost::property_tree::ptree& pt, const std::string& /*node*/ )
{
	using namespace _Wolframe::config;
	bool retVal = true;

	for ( boost::property_tree::ptree::const_iterator L1it = pt.begin(); L1it != pt.end(); L1it++ )	{
		if ( boost::algorithm::iequals( L1it->first, "file" ))	{
			AAAA::FileAuditConfig* conf = new AAAA::FileAuditConfig( "File", cfg.logPrefix().c_str(), "file" );
			if ( ConfigurationParser::parse( *conf, L1it->second, L1it->first ))
				cfg.m_config.push_back( conf );
			else	{
				delete conf;
				retVal = false;
			}
		}
		else if ( boost::algorithm::iequals( L1it->first, "database" ))	{
			AAAA::DatabaseAuditConfig* conf = new AAAA::DatabaseAuditConfig( "Database", cfg.logPrefix().c_str(), "database" );
			if ( ConfigurationParser::parse( *conf, L1it->second, L1it->first ))
				cfg.m_config.push_back( conf );
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

} // namespace config
//*************************************************************************************************
namespace AAAA {

/// constructor
AuditConfiguration::~AuditConfiguration()
{
	for ( std::list<AuditConfigurationBase*>::const_iterator it = m_config.begin();
								it != m_config.end(); it++ )
		delete *it;
}


/// methods
bool AuditConfiguration::check() const
{
	bool correct = true;
	for ( std::list<AuditConfigurationBase*>::const_iterator it = m_config.begin();
								it != m_config.end(); it++ )	{
		if ( !(*it)->check() )
			correct = false;
	}
	return correct;
}


void AuditConfiguration::print( std::ostream& os, size_t indent ) const
{
	std::string indStr( indent, ' ' );

	os << indStr << sectionName() << ":" << std::endl;
	if ( m_config.size() > 0 )	{
		for ( std::list<AuditConfigurationBase*>::const_iterator it = m_config.begin();
								it != m_config.end(); it++ )	{
			(*it)->print( os, indent + 3 );
		}
	}
	else
		os << "   None configured" << std::endl;
}


void AuditConfiguration::setCanonicalPathes( const std::string& referencePath )
{
	for ( std::list<AuditConfigurationBase*>::const_iterator it = m_config.begin();
								it != m_config.end(); it++ )	{
		(*it)->setCanonicalPathes( referencePath );
	}
}

}} // namespace _Wolframe::AAAA

