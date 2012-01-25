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

namespace _Wolframe {
namespace AAAA {

/// AAAAprovider PIMPL
AAAAprovider::AAAAprovider( const AAAAconfiguration* conf,
			    const module::ModulesDirectory* modules )
	: m_impl( new AAAAprovider_Impl( conf, modules ))	{}

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

Authorizer* AAAAprovider::authorizer() const
{
	return m_impl->authorizer();
}

Auditor* AAAAprovider::auditor()
{
	return m_impl->auditor();
}


/// AAAAprovider PIMPL implementation
AAAAprovider::AAAAprovider_Impl::AAAAprovider_Impl( const AAAAconfiguration* conf,
						    const module::ModulesDirectory* modules )
	: m_authenticator( conf->m_authConfig, modules ),
	  m_authorizer( conf->m_authzConfig, modules ),
	  m_auditor( conf->m_auditConfig, modules )	{}

bool AAAAprovider::AAAAprovider_Impl::resolveDB( const db::DatabaseProvider& db )
{
	LOG_DATA << "Resolving authentication databases";
	if ( !m_authenticator.resolveDB( db ))
		return false;
	LOG_DATA << "Resolving authorization databases";
	if ( !m_authorizer.resolveDB( db ))
		return false;
	LOG_DATA << "Resolving audit databases";
	if ( !m_auditor.resolveDB( db ))
		return false;
	LOG_DEBUG << "AAAA database references resolved";
	return true;
}


/***********************************************************************************/
AuthenticationFactory::AuthenticationFactory( const std::list< config::ObjectConfiguration* >& confs,
					  const module::ModulesDirectory* modules )
{
	for ( std::list<config::ObjectConfiguration*>::const_iterator it = confs.begin();
							it != confs.end(); it++ )	{
		module::ContainerBuilder* builder = modules->getContainer((*it)->objectName());
		if ( builder )	{
			ObjectContainer< AuthenticationUnit >* auth =
					dynamic_cast< ObjectContainer< AuthenticationUnit >* >( builder->container( **it ));
			m_authenticators.push_back( auth->object() );
			LOG_TRACE << "'" << auth->objectName() << "' authentication unit registered";
			auth->dispose();
		}
		else	{
			LOG_ALERT << "AuthenticationFactory: unknown authentication type '" << (*it)->objectName() << "'";
			throw std::domain_error( "Unknown authentication mechanism type in AAAAprovider constructor. See log" );
		}
	}
}

AuthenticationFactory::~AuthenticationFactory()
{
	for ( std::list< AuthenticationUnit* >::const_iterator it = m_authenticators.begin();
								it != m_authenticators.end(); it++ )
		delete *it;
}

bool AuthenticationFactory::resolveDB( const db::DatabaseProvider& db )
{
	for ( std::list< AuthenticationUnit* >::const_iterator it = m_authenticators.begin();
								it != m_authenticators.end(); it++ )
		if ( ! (*it)->resolveDB( db ) )
			return false;
	return true;
}


/***********************************************************************************/
AuditProvider::AuditProvider( const std::list< config::ObjectConfiguration* >& confs,
			const module::ModulesDirectory* modules )
{
	for ( std::list<config::ObjectConfiguration*>::const_iterator it = confs.begin();
								it != confs.end(); it++ )	{
		module::ContainerBuilder* builder = modules->getContainer((*it)->objectName());
		if ( builder )	{
			ObjectContainer< AuditUnit >* audit =
					dynamic_cast< ObjectContainer< AuditUnit >* >( builder->container( **it ));
			m_auditors.push_back( audit->object() );
			LOG_TRACE << "'" << audit->objectName() << "' audit unit registered";
			audit->dispose();
		}
		else	{
			LOG_ALERT << "AuditProvider: unknown audit type '" << (*it)->objectName() << "'";
			throw std::domain_error( "Unknown auditing mechanism type in AAAAprovider constructor. See log" );
		}
	}
}

AuditProvider::~AuditProvider()
{
	for ( std::list< AuditUnit* >::iterator it = m_auditors.begin();
						it != m_auditors.end(); it++ )
		delete *it;
}

bool AuditProvider::resolveDB( const db::DatabaseProvider& db )
{
	for ( std::list< AuditUnit* >::iterator it = m_auditors.begin();
						it != m_auditors.end(); it++ )
		if ( ! (*it)->resolveDB( db ) )
			return false;
	return true;
}

}} // namespace _Wolframe::AAAA
