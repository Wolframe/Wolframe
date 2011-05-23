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
// AAAA provider implementation
//

#include "AAAAprovider.hpp"
#include "config/configurationBase.hpp"
#include "logger.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/algorithm/string.hpp>

#include <ostream>

namespace _Wolframe {
namespace config {

template<>
bool ConfigurationParser::parse( AAAA::Configuration& cfg,
				 const boost::property_tree::ptree& pt, const std::string& /*node*/ )
{
	using namespace _Wolframe::config;
	bool retVal = true;

	for ( boost::property_tree::ptree::const_iterator L1it = pt.begin(); L1it != pt.end(); L1it++ )	{
		if ( boost::algorithm::iequals( L1it->first, "Authentication" ) ||
				boost::algorithm::iequals( L1it->first, "Auth" ))	{
			if ( ! cfg. auth.parse( L1it->second, L1it->first ))
				retVal = false;
		}
		else if ( boost::algorithm::iequals( L1it->first, "Audit" ))	{
			if ( ! cfg.audit.parse( L1it->second, L1it->first ))
				retVal = false;
		}
		else
			LOG_WARNING << cfg.logPrefix() << ": unknown configuration option: '"
				    << L1it->first << "'";
	}
	return retVal;
}

} // namespace config

namespace AAAA {

bool Configuration::parse( const boost::property_tree::ptree& pt, const std::string& node )
{
	return config::ConfigurationParser::parse( *this, pt, node );
}

AAAAprovider::AAAAprovider( const Configuration& config )
{
	for ( std::list<AuthenticationConfigBase*>::const_iterator it = config.auth.m_config.begin();
							it != config.auth.m_config.end(); it++ )	{
		switch( (*it)->type() )	{
		case AUTH_DATABASE:	{
			DatabaseAuth* auth = new DatabaseAuth( static_cast<DatabaseAuthConfig&>(**it) );
			m_authenticators.push_back( auth );
		}
			break;
		case AUTH_TEXTFILE:	{
			TextFileAuth* auth = new TextFileAuth( static_cast<TextFileAuthConfig&>(**it) );
			m_authenticators.push_back( auth );
		}
			break;
		case AUTH_PAM:
		case AUTH_SASL:
		case AUTH_LDAP:
			LOG_ERROR << "Auth method not implemented yet";
			break;
		case AUTH_UNKNOWN:
		default:
			throw std::domain_error( "Unknown auditing mechanism type in AAAAprovider constructor" );
		}
	}

	for ( std::list<AuditConfigBase*>::const_iterator it = config.audit.m_config.begin();
							it != config.audit.m_config.end(); it++ )	{
		switch( (*it)->type() )	{
		case AUDIT_FILE:	{
			FileAuditor* auditor = new FileAuditor( static_cast<FileAuditConfig&>(**it) );
			m_auditors.push_back( auditor );
		}
			break;
		case AUDIT_DATABASE:	{
			DatabaseAuditor* auditor = new DatabaseAuditor( static_cast<DatabaseAuditConfig&>(**it) );
			m_auditors.push_back( auditor );
		}
			break;
		default:
			throw std::domain_error( "Unknown auditing mechanism type in AAAAprovider constructor" );
		}
	}
}

AAAAprovider::~AAAAprovider()
{
	for ( std::list<GlobalAuthenticatorBase*>::const_iterator it = m_authenticators.begin();
								it != m_authenticators.end(); it++ )
		delete *it;
	for ( std::list<GlobalAuditorBase*>::const_iterator it = m_auditors.begin();
								it != m_auditors.end(); it++ )
		delete *it;
}

bool AAAAprovider::resolveDB( db::DBprovider& db )
{
	for ( std::list<GlobalAuthenticatorBase*>::const_iterator it = m_authenticators.begin();
								it != m_authenticators.end(); it++ )
		if ( ! (*it)->resolveDB( db ) )
			return false;

	for ( std::list<GlobalAuditorBase*>::const_iterator it = m_auditors.begin();
								it != m_auditors.end(); it++ )
		if ( ! (*it)->resolveDB( db ) )
			return false;
	return true;
}


Configuration::Configuration() : config::ConfigurationBase( "AAAA", NULL, "AAAA configuration"  ),
	auth( "Authentication", logPrefix().c_str(), "Authentication" ),
	audit( "Auditing", logPrefix().c_str(), "Auditing" )
{
}


/// methods
void Configuration::print( std::ostream& os, size_t /* indent */ ) const
{
	os << sectionName() << std::endl;
	auth.print( os, 3 );
	audit.print( os, 3 );
}


/// Check if the database configuration makes sense
bool Configuration::check() const
{
	bool correct = true;

	if ( !auth.check() )
		correct = false;
	if ( !audit.check() )
		correct = false;

	return correct;
}

void Configuration::setCanonicalPathes( const std::string& refPath )
{
	auth.setCanonicalPathes( refPath );
	audit.setCanonicalPathes( refPath );
}

}} // namespace _Wolframe::AAAA

