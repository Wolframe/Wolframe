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

#include "AAAAproviderImpl.hpp"
#include "logger-v1.hpp"
#include "boost/algorithm/string.hpp"

/****  Impersonating the module loader  ***************************************************************/
#include "FileAudit.hpp"
#include "DBaudit.hpp"
#include "TextFileAuthentication.hpp"
#include "DBauthentication.hpp"

using namespace _Wolframe;

static const size_t noAuthModules = 2;
static module::ContainerDescription< ObjectContainer< AAAA::AuthenticationUnit > >
authModules[ noAuthModules ] = { module::ContainerDescription< ObjectContainer< AAAA::AuthenticationUnit > >( "DatabaseAuth", &AAAA::DBauthContainer::create ),
				 module::ContainerDescription< ObjectContainer< AAAA::AuthenticationUnit > >( "TextFileAuth", &AAAA::TxtFileAuthContainer::create ) };

static const size_t noAuditModules = 2;
static module::ContainerDescription< ObjectContainer< AAAA::AuditUnit > >
auditModules[ noAuditModules ] = { module::ContainerDescription< ObjectContainer< AAAA::AuditUnit > >( "DatabaseAudit", &AAAA::DBauditContainer::create ),
				   module::ContainerDescription< ObjectContainer< AAAA::AuditUnit > >( "FileAudit", &AAAA::FileAuditContainer::create ) };
/****  End impersonating the module loader  ***********************************************************/

namespace _Wolframe {
namespace AAAA {

/// AAAAprovider PIMPL
AAAAprovider::AAAAprovider( const AAAAconfiguration* conf ) :
	m_impl( new AAAAprovider_Impl( conf ))	{}

AAAAprovider::~AAAAprovider()
{
	delete m_impl;
}

bool AAAAprovider::resolveDB( const db::DatabaseProvider& db )
{
	return m_impl->resolveDB( db );
}

Authenticator* AAAAprovider::authenticator()
{
	return m_impl->authenticator();
}

Auditor* AAAAprovider::auditor()
{
	return m_impl->auditor();
}


/// AAAAprovider PIMPL implementation
AAAAprovider::AAAAprovider_Impl::AAAAprovider_Impl( const AAAAconfiguration* conf )
	: m_authenticator( conf->m_authConfig, authModules, noAuthModules ),
	  m_auditor( conf->m_auditConfig, auditModules, noAuditModules )	{}

bool AAAAprovider::AAAAprovider_Impl::resolveDB( const db::DatabaseProvider& db )
{
	if ( !m_authenticator.resolveDB( db ))
		return false;
	if ( !m_auditor.resolveDB( db ))
		return false;
	return true;
}


/***********************************************************************************/
AuthenticationGroup::AuthenticationGroup( const std::list< config::ObjectConfiguration* >& confs,
					  module::ContainerDescription< ObjectContainer< AuthenticationUnit > >* description,
					  size_t descrSize )
{
	for ( std::list<config::ObjectConfiguration*>::const_iterator it = confs.begin();
							it != confs.end(); it++ )	{
		const char* authType = (*it)->objectName();
		size_t i;
		for ( i = 0; i < descrSize; i++ )	{
			if ( boost::algorithm::iequals( description[i].name, authType ))	{
				ObjectContainer< AuthenticationUnit >* container = authModules[i].createFunc( **it );
				m_authenticators.push_back( container );
				break;
			}
		}
		if ( i >= descrSize )	{
			LOG_ALERT << "AuthenticationGroup: unknown authentication type '" << authType << "'";
			throw std::domain_error( "Unknown authentication mechanism type in AAAAprovider constructor. See log" );
		}
	}
}

AuthenticationGroup::~AuthenticationGroup()
{
	for ( std::list< ObjectContainer< AuthenticationUnit >* >::const_iterator it = m_authenticators.begin();
								it != m_authenticators.end(); it++ )
		delete *it;
}

bool AuthenticationGroup::resolveDB( const db::DatabaseProvider& db )
{
	for ( std::list< ObjectContainer< AuthenticationUnit >* >::const_iterator it = m_authenticators.begin();
								it != m_authenticators.end(); it++ )
		if ( ! (*it)->object().resolveDB( db ) )
			return false;
	return true;
}


/***********************************************************************************/
AuditGroup::AuditGroup( const std::list< config::ObjectConfiguration* >& confs,
			module::ContainerDescription< ObjectContainer< AAAA::AuditUnit > >* description,
			size_t descrSize )
{
	for ( std::list<config::ObjectConfiguration*>::const_iterator it = confs.begin();
							it != confs.end(); it++ )	{
		const char* auditType = (*it)->objectName();
		size_t i;
		for ( i = 0; i < descrSize; i++ )	{
			if ( boost::algorithm::iequals( description[i].name, auditType ))	{
				ObjectContainer< AAAA::AuditUnit >* container = auditModules[i].createFunc( **it );
				m_auditors.push_back( container );
				break;
			}
		}
		if ( i >= descrSize )	{
			LOG_ALERT << "AuditGroup: unknown audit type '" << auditType << "'";
			throw std::domain_error( "Unknown auditing mechanism type in AAAAprovider constructor. See log" );
		}
	}
}

AuditGroup::~AuditGroup()
{
	for ( std::list< ObjectContainer< AAAA::AuditUnit >* >::const_iterator it = m_auditors.begin();
								it != m_auditors.end(); it++ )
		delete *it;
}

bool AuditGroup::resolveDB( const db::DatabaseProvider& db )
{
	for ( std::list< ObjectContainer< AAAA::AuditUnit >* >::const_iterator it = m_auditors.begin();
								it != m_auditors.end(); it++ )
		if ( ! (*it)->object().resolveDB( db ) )
			return false;
	return true;
}


/***********************************************************************************/
AAAAconfiguration::AAAAconfiguration()
	: config::ConfigurationBase( "AAAA", NULL, "AAAA configuration"  ),
	  m_allowAnonymous( false )	{}

/// destructor
AAAAconfiguration::~AAAAconfiguration()
{
	for ( std::list< config::ObjectConfiguration* >::const_iterator it = m_authConfig.begin();
								it != m_authConfig.end(); it++ )
		delete *it;

	for ( std::list< config::ObjectConfiguration* >::const_iterator it =m_auditConfig.begin();
								it != m_auditConfig.end(); it++ )
		delete *it;
}


/// methods
void AAAAconfiguration::print( std::ostream& os, size_t /* indent */ ) const
{
	os << sectionName() << std::endl;
	os << "   Authentication" << std::endl;
	os << "      Allow anonymous login: " << (m_allowAnonymous ? "yes" : "no") << std::endl;
	for ( std::list< config::ObjectConfiguration* >::const_iterator it = m_authConfig.begin();
								it != m_authConfig.end(); it++ )
		(*it)->print( os, 6 );

	os << "   Audit" << std::endl;
	for ( std::list< config::ObjectConfiguration* >::const_iterator it = m_auditConfig.begin();
								it != m_auditConfig.end(); it++ )
		(*it)->print( os, 6 );

}

/// Check if the database configuration makes sense
bool AAAAconfiguration::check() const
{
	bool correct = true;

	for ( std::list< config::ObjectConfiguration* >::const_iterator it = m_authConfig.begin();
								it != m_authConfig.end(); it++ )	{
		if ( !(*it)->check() )
			correct = false;
	}

	for ( std::list< config::ObjectConfiguration* >::const_iterator it = m_auditConfig.begin();
								it != m_auditConfig.end(); it++ )	{
		if ( !(*it)->check() )
			correct = false;
	}

	return correct;
}

void AAAAconfiguration::setCanonicalPathes( const std::string& refPath )
{
	for ( std::list< config::ObjectConfiguration* >::const_iterator it = m_authConfig.begin();
								it != m_authConfig.end(); it++ )
		(*it)->setCanonicalPathes( refPath );

	for ( std::list< config::ObjectConfiguration* >::const_iterator it = m_auditConfig.begin();
								it != m_auditConfig.end(); it++ )
		(*it)->setCanonicalPathes( refPath );
}

}} // namespace _Wolframe::AAAA

