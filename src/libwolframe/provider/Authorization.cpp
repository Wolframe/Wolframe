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
// Authorization provider and standard authorizer
//

#include <stdexcept>

#include "AAAAproviderImpl.hpp"
#include "moduleDirectory.hpp"
#include "logger-v1.hpp"
#include "boost/algorithm/string.hpp"

namespace _Wolframe {
namespace AAAA {

AuthorizationProvider::AuthorizationProvider( const std::list< config::NamedConfiguration* >& confs,
					      bool authzDefault,
					      const module::ModulesDirectory* modules )
{
	for ( std::list<config::NamedConfiguration*>::const_iterator it = confs.begin();
								it != confs.end(); it++ )	{
		module::ConfiguredBuilder* builder = modules->getBuilder((*it)->className());
		if ( builder )	{
			ConfiguredObjectConstructor< AuthorizationUnit >* authz =
					dynamic_cast< ConfiguredObjectConstructor< AuthorizationUnit >* >( builder->constructor());
			if ( authz == NULL )	{
				LOG_ALERT << "AuthorizationProvider: '" << builder->objectClassName()
					  << "' is not an Authorization Unit builder";
				throw std::logic_error( "object is not an AuthorizationUnit builder" );
			}
			m_authorizeUnits.push_back( authz->object( **it ) );
			LOG_TRACE << "'" << authz->objectClassName() << "' authorization unit registered";
		}
		else	{
			LOG_ALERT << "AuthorizationProvider: unknown authorization type '" << (*it)->className() << "'";
			throw std::domain_error( "Unknown authorization mechanism type in AAAAprovider constructor. See log" );
		}
	}
	m_authorizer = new StandardAuthorizer( m_authorizeUnits, authzDefault );
}

AuthorizationProvider::~AuthorizationProvider()
{
	if ( m_authorizer )
		delete m_authorizer;
	for ( std::list< AuthorizationUnit* >::iterator it = m_authorizeUnits.begin();
								it != m_authorizeUnits.end(); it++ )
		delete *it;
}

bool AuthorizationProvider::resolveDB( const db::DatabaseProvider& db )
{
	for ( std::list< AuthorizationUnit* >::iterator it = m_authorizeUnits.begin();
								it != m_authorizeUnits.end(); it++ )
		if ( ! (*it)->resolveDB( db ) )
			return false;
	return true;
}


//********************************************************************************
StandardAuthorizer::StandardAuthorizer( const std::list< AuthorizationUnit* >& units,
					bool dflt )
	: m_authorizeUnits( units ), m_default( dflt )
{
}

StandardAuthorizer::~StandardAuthorizer()
{
}

void StandardAuthorizer::close()
{
}

// authorization requests
bool StandardAuthorizer::allowed( const Information& authzObject )
{
	bool retVal = true;
	bool ignored = true;

	for ( std::list< AuthorizationUnit* >::const_iterator au = m_authorizeUnits.begin();
							au != m_authorizeUnits.end(); au++ )	{
		AuthorizationUnit::Result res = (*au)->allowed( authzObject );
		switch( res )	{
			case AuthorizationUnit::ALLOWED:
				ignored = false;
				break;
			case AuthorizationUnit::DENIED:
				ignored = false;
				retVal = false;
				break;
			case AuthorizationUnit::IGNORED:
				break;
			case AuthorizationUnit::ERROR:
				return false;
		}
	}
	if ( ignored )
		return m_default;
	else
		return retVal;
}

}} // namespace _Wolframe::AAAA
