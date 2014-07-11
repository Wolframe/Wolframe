/************************************************************************

 Copyright (C) 2011 - 2014 Project Wolframe.
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
#include "system/globalRngGen.hpp"

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

Authenticator* AAAAprovider::authenticator( const _Wolframe::net::RemoteEndpoint &client ) const
{
	return m_impl->authenticator( client );
}

PasswordChanger* AAAAprovider::passwordChanger( const User& user, const net::RemoteEndpoint& client) const
{
	return m_impl->passwordChanger( user, client );
}

Authorizer* AAAAprovider::authorizer() const
{
	return m_impl->authorizer();
}

Auditor* AAAAprovider::auditor() const
{
	return m_impl->auditor();
}


/// AAAAprovider PIMPL implementation
AAAAprovider::AAAAprovider_Impl::AAAAprovider_Impl( const AAAAconfiguration* conf,
						    const module::ModulesDirectory* modules )
	: m_authenticator( conf->m_authConfig, modules ),
	  m_authorizer( conf->m_authzConfig, conf->m_authzDefault, modules ),
	  m_auditor( conf->m_auditConfig, modules )
{
	// initialize the global rnd generator
	GlobalRandomGenerator::instance( conf->m_randomDevice );
}

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
	LOG_TRACE << "AAAA database references resolved";
	return true;
}

}} // namespace _Wolframe::AAAA
