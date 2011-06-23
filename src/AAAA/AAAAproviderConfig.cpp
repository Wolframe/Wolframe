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
//
//

#include "logger.hpp"
#include "AAAAprovider.hpp"
#include "config/configurationParser.hpp"

#include "TextFileAuthentication.hpp"
#include "DBauthentication.hpp"
#include "FileAudit.hpp"
#include "DBaudit.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/algorithm/string.hpp>

namespace _Wolframe {
namespace config {

template<>
bool ConfigurationParser::parse( AAAA::AAAAconfiguration& cfg,
				 const boost::property_tree::ptree& pt, const std::string& /*node*/ )
{
	using namespace _Wolframe::config;
	bool retVal = true;

	for ( boost::property_tree::ptree::const_iterator L1it = pt.begin(); L1it != pt.end(); L1it++ )	{
		if ( boost::algorithm::iequals( L1it->first, "Authentication" ) ||
				boost::algorithm::iequals( L1it->first, "Auth" ))	{
			for ( boost::property_tree::ptree::const_iterator L2it = L1it->second.begin();
									L2it != L1it->second.end(); L2it++ )	{
				if ( boost::algorithm::iequals( L2it->first, "file" ))	{
					AAAA::TextFileAuthConfig* conf = new AAAA::TextFileAuthConfig( "File", cfg.logPrefix().c_str(), "file" );
					if ( ConfigurationParser::parse( *conf, L2it->second, L2it->first ))
						cfg.m_authConfig.push_back( conf );
					else	{
						delete conf;
						retVal = false;
					}
				}
				else if ( boost::algorithm::iequals( L2it->first, "database" ))	{
					AAAA::DatabaseAuthConfig* conf = new AAAA::DatabaseAuthConfig( "Database", cfg.logPrefix().c_str(), "database" );
					if ( ConfigurationParser::parse( *conf, L2it->second, L2it->first ))
						cfg.m_authConfig.push_back( conf );
					else	{
						delete conf;
						retVal = false;
					}
				}
				else
					LOG_WARNING << cfg.logPrefix() << "unknown configuration option: '"
						    << L2it->first << "'";
			}
		}
		else if ( boost::algorithm::iequals( L1it->first, "Audit" ))	{
			for ( boost::property_tree::ptree::const_iterator L2it = L1it->second.begin();
									L2it != L1it->second.end(); L2it++ )	{
				if ( boost::algorithm::iequals( L2it->first, "file" ))	{
					AAAA::FileAuditConfig* conf = new AAAA::FileAuditConfig( "File", cfg.logPrefix().c_str(), "file" );
					if ( ConfigurationParser::parse( *conf, L2it->second, L2it->first ))
						cfg.m_auditConfig.push_back( conf );
					else	{
						delete conf;
						retVal = false;
					}
				}
				else if ( boost::algorithm::iequals( L2it->first, "database" ))	{
					AAAA::DBauditConfig* conf = new AAAA::DBauditConfig( "Database", cfg.logPrefix().c_str(), "database" );
					if ( ConfigurationParser::parse( *conf, L2it->second, L2it->first ))
						cfg.m_auditConfig.push_back( conf );
					else	{
						delete conf;
						retVal = false;
					}
				}
				else
					LOG_WARNING << cfg.logPrefix() << "unknown configuration option: '"
						    << L2it->first << "'";
			}
		}
		else
			LOG_WARNING << cfg.logPrefix() << ": unknown configuration option: '"
				    << L1it->first << "'";
	}
	return retVal;
}

}} // namespace _Wolframe::config
