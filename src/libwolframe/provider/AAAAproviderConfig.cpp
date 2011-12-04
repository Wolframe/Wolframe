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

#include "logger-v1.hpp"
#include "AAAAproviderImpl.hpp"
#include "config/ConfigurationTree.hpp"
#include "config/valueParser.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/algorithm/string.hpp>

namespace _Wolframe {
namespace AAAA {

bool AAAAconfiguration::parse( const config::ConfigurationTree& pt, const std::string& /*node*/,
			       const module::ModulesDirectory* modules )
{
	using namespace _Wolframe::config;
	bool retVal = true;
	bool allowDefined = false;
	bool mandatoryDefined = false;

	for ( boost::property_tree::ptree::const_iterator L1it = pt.begin(); L1it != pt.end(); L1it++ )	{
		if ( boost::algorithm::iequals( L1it->first, "Authentication" ) ||
				boost::algorithm::iequals( L1it->first, "Auth" ))	{
			for ( boost::property_tree::ptree::const_iterator L2it = L1it->second.begin();
									L2it != L1it->second.end(); L2it++ )	{
				if ( boost::algorithm::iequals( "allowAnonymous", L2it->first ))	{
					if ( ! Parser::getValue( logPrefix().c_str(), *L2it, m_allowAnonymous,
								 Parser::BoolDomain(), &allowDefined ))
						retVal = false;
				}
				else if ( modules )	{
					module::ModuleContainer* container = modules->getContainer( "Authentication", L2it->first );
					if ( container )	{
						config::ObjectConfiguration* conf = container->configuration( logPrefix().c_str());
						if ( conf->parse( L2it->second, L2it->first, modules ))
							m_authConfig.push_back( conf );
						else	{
							delete conf;
							retVal = false;
						}
					}
					else
						LOG_WARNING << logPrefix() << "unknown configuration option: '"
							    << L2it->first << "'";
				}
				else
					LOG_WARNING << logPrefix() << "unknown configuration option: '"
						    << L2it->first << "'";
			}
		}
		else if ( boost::algorithm::iequals( L1it->first, "Authorization" ))	{
			for ( boost::property_tree::ptree::const_iterator L2it = L1it->second.begin();
									L2it != L1it->second.end(); L2it++ )	{
				if ( modules )	{
					module::ModuleContainer* container = modules->getContainer( "Authorization", L2it->first );
					if ( container )	{
						config::ObjectConfiguration* conf = container->configuration( logPrefix().c_str());
						if ( conf->parse( L2it->second, L2it->first, modules ))
							m_authzConfig.push_back( conf );
						else	{
							delete conf;
							retVal = false;
						}
					}
					else
						LOG_WARNING << logPrefix() << "unknown configuration option: '"
							    << L2it->first << "'";
				}
				else
					LOG_WARNING << logPrefix() << "unknown configuration option: '"
						    << L2it->first << "'";
			}
		}
		else if ( boost::algorithm::iequals( L1it->first, "Audit" ))	{
			for ( boost::property_tree::ptree::const_iterator L2it = L1it->second.begin();
									L2it != L1it->second.end(); L2it++ )	{
				if ( boost::algorithm::iequals( "mandatory", L2it->first ))	{
					if ( ! Parser::getValue( logPrefix().c_str(), *L2it, m_mandatoryAudit,
								 Parser::BoolDomain(), &mandatoryDefined ))
						retVal = false;
				}
				else if ( modules )	{
					module::ModuleContainer* container = modules->getContainer( "Audit", L2it->first );
					if ( container )	{
						config::ObjectConfiguration* conf = container->configuration( logPrefix().c_str());
						if ( conf->parse( L2it->second, L2it->first, modules ))
							m_auditConfig.push_back( conf );
						else	{
							delete conf;
							retVal = false;
						}
					}
					else
						LOG_WARNING << logPrefix() << "unknown configuration option: '"
							    << L2it->first << "'";
				}
				else
					LOG_WARNING << logPrefix() << "unknown configuration option: '"
						    << L2it->first << "'";
			}
		}
		else
			LOG_WARNING << logPrefix() << "unknown configuration option: '"
				    << L1it->first << "'";
	}
	return retVal;
}

}} // namespace _Wolframe::config
