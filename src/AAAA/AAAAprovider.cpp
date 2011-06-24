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

#include <stdexcept>

#include "AAAAprovider.hpp"
#include "logger.hpp"
#include "boost/algorithm/string.hpp"

#include "FileAudit.hpp"
#include "DBaudit.hpp"
#include "TextFileAuthentication.hpp"
#include "DBauthentication.hpp"

namespace _Wolframe {
namespace AAAA {

/****  Impersonating the module loader  ***************************************************************/
static const size_t noAuthModules = 2;
static AuthModuleDescription
authModules[ noAuthModules ] = { AuthModuleDescription( "DatabaseAuth", &DBauthContainer::create ),
			     AuthModuleDescription( "TextFileAuth", &TxtFileAuthContainer::create ) };

static const size_t noAuditModules = 2;
static AuditModuleDescription
auditModules[ noAuditModules ] = { AuditModuleDescription( "DatabaseAudit", &DBauditContainer::create ),
				   AuditModuleDescription( "FileAudit", &FileAuditContainer::create ) };
/****  End impersonating the module loader  ***********************************************************/


AAAAprovider::AAAAprovider( const AAAAconfiguration& conf )
	: m_authenticator( conf.m_authConfig ),
	  m_auditor( conf.m_auditConfig )	{}

bool AAAAprovider::resolveDB( db::DatabaseProvider& db )
{
	if ( !m_authenticator.resolveDB( db ))
		return false;
	if ( !m_auditor.resolveDB( db ))
		return false;
	return true;
}


/***********************************************************************************/
AuthenticationGroup::AuthenticationGroup( const std::list< AuthenticationConfiguration* >& confs )
{
	for ( std::list<AuthenticationConfiguration*>::const_iterator it = confs.begin();
							it != confs.end(); it++ )	{
		const char* authType = (*it)->typeName();
		size_t i;
		for ( i = 0; i < noAuthModules; i++ )	{
			if ( boost::algorithm::iequals( authModules[i].name, authType ))	{
				AuthenticationContainer* container = authModules[i].createFunc( **it );
				m_authenticators.push_back( container );
				break;
			}
		}
		if ( i >= noAuthModules )	{
			LOG_ALERT << "AuthenticationGroup: unknown authentication type '" << authType << "'";
			throw std::domain_error( "Unknown authentication mechanism type in AAAAprovider constructor. See log" );
		}
	}
}

AuthenticationGroup::~AuthenticationGroup()
{
	for ( std::list<AuthenticationContainer*>::const_iterator it = m_authenticators.begin();
								it != m_authenticators.end(); it++ )
		delete *it;
}

bool AuthenticationGroup::resolveDB( db::DatabaseProvider& db )
{
	for ( std::list<AuthenticationContainer*>::const_iterator it = m_authenticators.begin();
								it != m_authenticators.end(); it++ )
		if ( ! (*it)->resolveDB( db ) )
			return false;
	return true;
}


/***********************************************************************************/
AuditGroup::AuditGroup( const std::list< AuditConfiguration* >& confs )

{
	for ( std::list<AuditConfiguration*>::const_iterator it = confs.begin();
							it != confs.end(); it++ )	{
		const char* auditType = (*it)->typeName();
		size_t i;
		for ( i = 0; i < noAuditModules; i++ )	{
			if ( boost::algorithm::iequals( auditModules[i].name, auditType ))	{
				AuditContainer* container = auditModules[i].createFunc( **it );
				m_auditors.push_back( container );
				break;
			}
		}
		if ( i >= noAuditModules )	{
			LOG_ALERT << "AuditGroup: unknown audit type '" << auditType << "'";
			throw std::domain_error( "Unknown auditing mechanism type in AAAAprovider constructor. See log" );
		}
	}
}

AuditGroup::~AuditGroup()
{
	for ( std::list<AuditContainer*>::const_iterator it = m_auditors.begin();
								it != m_auditors.end(); it++ )
		delete *it;
}

bool AuditGroup::resolveDB( db::DatabaseProvider& db )
{
	for ( std::list<AuditContainer*>::const_iterator it = m_auditors.begin();
								it != m_auditors.end(); it++ )
		if ( ! (*it)->resolveDB( db ) )
			return false;
	return true;
}


/***********************************************************************************/
AAAAconfiguration::AAAAconfiguration()
	: config::ConfigurationBase( "AAAA", NULL, "AAAA configuration"  )
{}

/// destructor
AAAAconfiguration::~AAAAconfiguration()
{
	for ( std::list<AuthenticationConfiguration*>::const_iterator it = m_authConfig.begin();
								it != m_authConfig.end(); it++ )
		delete *it;

	for ( std::list<AuditConfiguration*>::const_iterator it =m_auditConfig.begin();
								it != m_auditConfig.end(); it++ )
		delete *it;
}


/// methods
void AAAAconfiguration::print( std::ostream& os, size_t /* indent */ ) const
{
	os << sectionName() << std::endl;
	os << "   Authentication" << std::endl;
	for ( std::list<AuthenticationConfiguration*>::const_iterator it = m_authConfig.begin();
								it != m_authConfig.end(); it++ )
		(*it)->print( os, 6 );

	os << "   Audit" << std::endl;
	for ( std::list<AuditConfiguration*>::const_iterator it = m_auditConfig.begin();
								it != m_auditConfig.end(); it++ )
		(*it)->print( os, 6 );

}

/// Check if the database configuration makes sense
bool AAAAconfiguration::check() const
{
	bool correct = true;

	for ( std::list<AuthenticationConfiguration*>::const_iterator it = m_authConfig.begin();
								it != m_authConfig.end(); it++ )	{
		if ( !(*it)->check() )
			correct = false;
	}

	for ( std::list<AuditConfiguration*>::const_iterator it = m_auditConfig.begin();
								it != m_auditConfig.end(); it++ )	{
		if ( !(*it)->check() )
			correct = false;
	}

	return correct;
}

void AAAAconfiguration::setCanonicalPathes( const std::string& refPath )
{
	for ( std::list<AuthenticationConfiguration*>::const_iterator it = m_authConfig.begin();
								it != m_authConfig.end(); it++ )
		(*it)->setCanonicalPathes( refPath );

	for ( std::list<AuditConfiguration*>::const_iterator it = m_auditConfig.begin();
								it != m_auditConfig.end(); it++ )
		(*it)->setCanonicalPathes( refPath );
}

}} // namespace _Wolframe::AAAA

