/************************************************************************

 Copyright (C) 2011 - 2013 Project Wolframe.
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

#include "logger/logger-v1.hpp"
#include "moduleDirectory.hpp"
#include "AAAAproviderImpl.hpp"
#include "config/ConfigurationTree.hpp"
#include "config/valueParser.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/algorithm/string.hpp>

namespace _Wolframe {
namespace AAAA {

/// constructor
AAAAconfiguration::AAAAconfiguration()
	: config::ConfigurationBase( "AAAA", NULL, "AAAA configuration"  ),
	  m_authzDefault( false ), m_mandatoryAudit( true )
{}

/// destructor
AAAAconfiguration::~AAAAconfiguration()
{
	for ( std::list< config::NamedConfiguration* >::const_iterator it = m_authConfig.begin();
								it != m_authConfig.end(); it++ )
		delete *it;

	for ( std::list< config::NamedConfiguration* >::const_iterator it = m_authzConfig.begin();
								it != m_authzConfig.end(); it++ )
		delete *it;

	for ( std::list< config::NamedConfiguration* >::const_iterator it =m_auditConfig.begin();
								it != m_auditConfig.end(); it++ )
		delete *it;
}

/// methods
bool AAAAconfiguration::parse( const config::ConfigurationTree& pt, const std::string& /*node*/,
			       const module::ModulesDirectory* modules )
{
	using namespace _Wolframe::config;
	bool retVal = true;
	bool authzDfltDefined = false;
	bool mandatoryDefined = false;

	for ( boost::property_tree::ptree::const_iterator L1it = pt.begin(); L1it != pt.end(); L1it++ )	{
		if ( boost::algorithm::iequals( L1it->first, "Authentication" ) ||
				boost::algorithm::iequals( L1it->first, "Auth" ))	{
			std::string logStr = logPrefix() + "authentication: ";
			for ( boost::property_tree::ptree::const_iterator L2it = L1it->second.begin();
									L2it != L1it->second.end(); L2it++ )	{
				if ( boost::algorithm::iequals( "randomDevice", L2it->first ))	{
					bool isDefined = ( !m_randomDevice.empty() );
					if ( ! Parser::getValue( logStr.c_str(), *L2it, m_randomDevice, &isDefined ))
						retVal = false;
				}
				else if ( modules )	{
					module::ConfiguredBuilder* builder = modules->getBuilder( "Authentication", L2it->first );
					if ( builder )	{
						config::NamedConfiguration* conf = builder->configuration( logStr.c_str());
						if ( conf->parse( L2it->second, L2it->first, modules ))
							m_authConfig.push_back( conf );
						else	{
							delete conf;
							retVal = false;
						}
					}
					else
						LOG_WARNING << logStr << "authentication: unknown configuration option: '"
							    << L2it->first << "'";
				}
				else
					LOG_WARNING << logStr << "authentication: unknown configuration option: '"
						    << L2it->first << "'";
			}
		}
		else if ( boost::algorithm::iequals( L1it->first, "Authorization" ))	{
			std::string logStr = logPrefix() + "authorization: ";
			for ( boost::property_tree::ptree::const_iterator L2it = L1it->second.begin();
									L2it != L1it->second.end(); L2it++ )	{
				if ( boost::algorithm::iequals( "default", L2it->first ))	{
					if ( ! Parser::getValue( logStr.c_str(), *L2it, m_authzDefault,
								 Parser::BoolDomain(), &authzDfltDefined ))
						retVal = false;
				}
				else if ( modules )	{
					module::ConfiguredBuilder* builder = modules->getBuilder( "Authorization", L2it->first );
					if ( builder )	{
						config::NamedConfiguration* conf = builder->configuration( logStr.c_str());
						if ( conf->parse( L2it->second, L2it->first, modules ))
							m_authzConfig.push_back( conf );
						else	{
							delete conf;
							retVal = false;
						}
					}
					else
						LOG_WARNING << logStr << "unknown configuration option: '"
							    << L2it->first << "'";
				}
				else
					LOG_WARNING << logStr << "unknown configuration option: '"
						    << L2it->first << "'";
			}
		}
		else if ( boost::algorithm::iequals( L1it->first, "Audit" ))	{
			std::string logStr = logPrefix() + "audit: ";
			for ( boost::property_tree::ptree::const_iterator L2it = L1it->second.begin();
									L2it != L1it->second.end(); L2it++ )	{
				if ( boost::algorithm::iequals( "mandatory", L2it->first ))	{
					if ( ! Parser::getValue( logStr.c_str(), *L2it, m_mandatoryAudit,
								 Parser::BoolDomain(), &mandatoryDefined ))
						retVal = false;
				}
				else if ( modules )	{
					module::ConfiguredBuilder* builder = modules->getBuilder( "Audit", L2it->first );
					if ( builder )	{
						config::NamedConfiguration* conf = builder->configuration( logStr.c_str());
						if ( conf->parse( L2it->second, L2it->first, modules ))
							m_auditConfig.push_back( conf );
						else	{
							delete conf;
							retVal = false;
						}
					}
					else
						LOG_WARNING << logStr << "unknown configuration option: '"
							    << L2it->first << "'";
				}
				else
					LOG_WARNING << logStr << "unknown configuration option: '"
						    << L2it->first << "'";
			}
		}
		else
			LOG_WARNING << logPrefix() << "unknown configuration option: '"
				    << L1it->first << "'";
	}

	return retVal;
}


void AAAAconfiguration::print( std::ostream& os, size_t /* indent */ ) const
{
	os << sectionName() << std::endl;
	os << "   Authentication" << std::endl;
	os << "      Random number device: " << m_randomDevice << std::endl;
	for ( std::list< config::NamedConfiguration* >::const_iterator it = m_authConfig.begin();
								it != m_authConfig.end(); it++ )
		(*it)->print( os, 6 );

	os << "   Authorization" << std::endl;
	os << "      Default: " << (m_authzDefault ? "allow" : "deny") << std::endl;
	for ( std::list< config::NamedConfiguration* >::const_iterator it = m_authzConfig.begin();
								it != m_authzConfig.end(); it++ )
		(*it)->print( os, 6 );

	os << "   Audit" << std::endl;
	os << "      Audit is mandatory: " << (m_mandatoryAudit ? "yes" : "no") << std::endl;
	for ( std::list< config::NamedConfiguration* >::const_iterator it = m_auditConfig.begin();
								it != m_auditConfig.end(); it++ )
		(*it)->print( os, 6 );

}

/// Check if the AAAA configuration makes sense
bool AAAAconfiguration::check() const
{
	bool correct = true;

	for ( std::list< config::NamedConfiguration* >::const_iterator it = m_authConfig.begin();
								it != m_authConfig.end(); it++ )	{
		if ( !(*it)->check() )
			correct = false;
	}

	for ( std::list< config::NamedConfiguration* >::const_iterator it = m_authzConfig.begin();
								it != m_authzConfig.end(); it++ )	{
		if ( !(*it)->check() )
			correct = false;
	}

	for ( std::list< config::NamedConfiguration* >::const_iterator it = m_auditConfig.begin();
								it != m_auditConfig.end(); it++ )	{
		if ( !(*it)->check() )
			correct = false;
	}

	return correct;
}

void AAAAconfiguration::setCanonicalPathes( const std::string& refPath )
{
	for ( std::list< config::NamedConfiguration* >::const_iterator it = m_authConfig.begin();
								it != m_authConfig.end(); it++ )
		(*it)->setCanonicalPathes( refPath );

	for ( std::list< config::NamedConfiguration* >::const_iterator it = m_authzConfig.begin();
								it != m_authzConfig.end(); it++ )
		(*it)->setCanonicalPathes( refPath );


	for ( std::list< config::NamedConfiguration* >::const_iterator it = m_auditConfig.begin();
								it != m_auditConfig.end(); it++ )
		(*it)->setCanonicalPathes( refPath );
}

}} // namespace _Wolframe::AAAA

