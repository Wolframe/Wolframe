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
#include "AAAAproviderImpl.hpp"
#include "config/configurationParser.hpp"
#include "config/valueParser.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/algorithm/string.hpp>

/****  Impersonating the module loader  ******************************************************/
#include "TextFileAuthentication.hpp"
#include "DBauthentication.hpp"
#include "FileAudit.hpp"
#include "DBaudit.hpp"

using namespace _Wolframe;

static const size_t noAuthConfigs = 2;
static module::ModuleConfigurationDescription
authConfig[ noAuthConfigs ] = { module::ModuleConfigurationDescription( "file", "Authentication file", "file",
				&AAAA::TextFileAuthConfig::create,
				&config::ConfigurationParser::parseBase<AAAA::TextFileAuthConfig> ),
				module::ModuleConfigurationDescription( "database", "Authentication database", "database",
				&AAAA::DatabaseAuthConfig::create,
				&config::ConfigurationParser::parseBase<AAAA::DatabaseAuthConfig> ) };

static const size_t noAuditConfigs = 2;
static module::ModuleConfigurationDescription
auditConfig[ noAuditConfigs ] = { module::ModuleConfigurationDescription( "file", "Audit file", "file",
				  &AAAA::FileAuditConfig::create,
				  &config::ConfigurationParser::parseBase<AAAA::FileAuditConfig> ),
				  module::ModuleConfigurationDescription( "database", "Audit database", "database",
				  &AAAA::DBauditConfig::create,
				  &config::ConfigurationParser::parseBase<AAAA::DBauditConfig> ) };
/****  End impersonating the module loader  **************************************************/

namespace _Wolframe {
namespace config {

template<>
bool ConfigurationParser::parse( AAAA::AAAAconfiguration& cfg,
				 const boost::property_tree::ptree& pt, const std::string& /*node*/ )
{
	using namespace _Wolframe::config;
	bool retVal = true;
	bool allowAnonDefined = false;

	for ( boost::property_tree::ptree::const_iterator L1it = pt.begin(); L1it != pt.end(); L1it++ )	{
		if ( boost::algorithm::iequals( L1it->first, "Authentication" ) ||
				boost::algorithm::iequals( L1it->first, "Auth" ))	{
			for ( boost::property_tree::ptree::const_iterator L2it = L1it->second.begin();
									L2it != L1it->second.end(); L2it++ )	{
				if ( boost::algorithm::iequals( "allowAnonymous", L2it->first ))	{
					if ( ! Parser::getValue( cfg.logPrefix().c_str(), *L2it, cfg.m_allowAnonymous,
								 Parser::BoolDomain(), &allowAnonDefined ))
						retVal = false;
				}
				else {
					size_t i;
					for ( i = 0; i < noAuthConfigs; i++ )	{
						if ( boost::algorithm::iequals( authConfig[i].typeName, L2it->first ))	{
							config::TypedConfiguration* conf = authConfig[i].createFunc( authConfig[i].sectionTitle,
															 cfg.logPrefix().c_str(),
															 authConfig[i].sectionName );
							if ( authConfig[i].parseFunc( *conf, L2it->second, L2it->first ))
								cfg.m_authConfig.push_back( conf );
							else	{
								delete conf;
								retVal = false;
							}
							break;
						}
					}
					if ( i >= noAuthConfigs )
						LOG_WARNING << cfg.logPrefix() << "unknown configuration option: '"
							    << L2it->first << "'";
				}
			}
		}
		else if ( boost::algorithm::iequals( L1it->first, "Audit" ))	{
			for ( boost::property_tree::ptree::const_iterator L2it = L1it->second.begin();
									L2it != L1it->second.end(); L2it++ )	{
				size_t i;
				for ( i = 0; i < noAuditConfigs; i++ )	{
					if ( boost::algorithm::iequals( auditConfig[i].typeName, L2it->first ))	{
						config::TypedConfiguration* conf = auditConfig[i].createFunc( auditConfig[i].sectionTitle,
														  cfg.logPrefix().c_str(),
														  auditConfig[i].sectionName );
						if ( auditConfig[i].parseFunc( *conf, L2it->second, L2it->first ))
							cfg.m_auditConfig.push_back( conf );
						else	{
							delete conf;
							retVal = false;
						}
						break;
					}
				}
				if ( i >= noAuditConfigs )
					LOG_WARNING << cfg.logPrefix() << "unknown configuration option: '"
						    << L2it->first << "'";
			}
		}
		else
			LOG_WARNING << cfg.logPrefix() << "unknown configuration option: '"
				    << L1it->first << "'";
	}
	return retVal;
}

}} // namespace _Wolframe::config
