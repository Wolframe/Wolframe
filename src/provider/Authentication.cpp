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
// Authentication factory implementation
//

#include <stdexcept>
#include <cassert>

#include "AAAAproviderImpl.hpp"
#include "logger-v1.hpp"
#include "boost/algorithm/string.hpp"
#include "module/moduleDirectory.hpp"

namespace _Wolframe {
namespace AAAA {

AuthenticationFactory::AuthenticationFactory( const std::list< config::NamedConfiguration* >& confs,
					      const module::ModulesDirectory* modules )
{
	for ( std::list<config::NamedConfiguration*>::const_iterator it = confs.begin();
							it != confs.end(); it++ )	{
		module::ConfiguredBuilder* builder = modules->getBuilder((*it)->className());
		if ( builder )	{
			ConfiguredObjectConstructor< AuthenticationUnit >* auth =
					dynamic_cast< ConfiguredObjectConstructor< AuthenticationUnit >* >( builder->constructor());
			if ( auth == NULL )	{
				LOG_ALERT << "AuthenticationFactory: '" << builder->objectClassName()
					  << "' is not an Authentication Unit builder";
				throw std::logic_error( "object is not an AuthenticationUnit builder" );
			}
			m_authUnits.push_back( auth->object( **it ));
			LOG_TRACE << "'" << auth->objectClassName() << "' authentication unit registered";
		}
		else	{
			LOG_ALERT << "AuthenticationFactory: unknown authentication type '" << (*it)->className() << "'";
			throw std::domain_error( "Unknown authentication mechanism type in AAAAprovider constructor. See log" );
		}
	}

	// Iterate through the list of authenticators (built at this point)
	// and build the vector of available mechs
	for ( std::list< AuthenticationUnit* >::const_iterator ait = m_authUnits.begin();
						ait != m_authUnits.end(); ait ++ )	{
		// add unit mechs to the list
		const char** p_mech = (*ait)->mechs();
		if ( *p_mech == NULL )	{
			LOG_WARNING << "'" << (*ait)->className() << "' has no authentication mechanisms";
		}
		while ( *p_mech )	{
			std::string mech( *p_mech ); boost::to_upper( mech );
			bool exists = false;
			for( std::vector< std::string >::const_iterator mit = m_mechs.begin();
						mit != m_mechs.end(); mit++ )	{
				if ( *mit == mech )
					exists = true;
			}
			if ( ! exists )	{
				m_mechs.push_back( mech );
				LOG_TRACE << "'" << mech << "' authentication mechanism registered";
			}
			p_mech++;
		}
	}
}

AuthenticationFactory::~AuthenticationFactory()
{
	for ( std::list< AuthenticationUnit* >::const_iterator it = m_authUnits.begin();
								it != m_authUnits.end(); it++ )
		delete *it;
}

bool AuthenticationFactory::resolveDB( const db::DatabaseProvider& db )
{
	for ( std::list< AuthenticationUnit* >::const_iterator it = m_authUnits.begin();
								it != m_authUnits.end(); it++ )
		if ( ! (*it)->resolveDB( db ) )
			return false;
	return true;
}


Authenticator* AuthenticationFactory::authenticator( const net::RemoteEndpoint& client )
{
	return new StandardAuthenticator( m_mechs, m_authUnits, client );
}


//*********************************************************************************
// Standard authenticator
StandardAuthenticator::StandardAuthenticator( const std::vector<std::string>& mechs_,
					      const std::list<AuthenticationUnit *> &units_,
					      const net::RemoteEndpoint &client_ )
	: m_mechs( mechs_ ), m_authUnits( units_ ), m_client( client_ ),
	  m_status( INITIALIZED ), m_currentSlice( -1 ), m_user( NULL )
{
}

StandardAuthenticator::~StandardAuthenticator()
{
	for ( std::vector< AuthenticatorSlice* >::iterator it = m_slices.begin();
							it != m_slices.end(); it++ )
		(*it)->dispose();
	m_slices.clear();
	if ( m_user )
		delete m_user, m_user = NULL;
}

void StandardAuthenticator::dispose()
{
	delete this;
}

// Get the list of available mechs
const std::vector< std::string >& StandardAuthenticator::mechs() const
{
	return m_mechs;
}

// Set the authentication mech
bool StandardAuthenticator::setMech( const std::string& mech )
{
	// Reset all data
	if ( m_user )
		delete m_user, m_user = NULL;
	for ( std::vector< AuthenticatorSlice* >::iterator it = m_slices.begin();
							it != m_slices.end(); it++ )
		(*it)->dispose();
	m_slices.clear();
	m_currentSlice = -1;

	bool	mechAvailable = false;

	LOG_TRACE << "StandardAuthenticator: set the authentication mechanism to '"
		  << mech << "'";
	if ( mech.empty() )	{
		LOG_WARNING << "StandardAuthenticator: request for an empty authentication mechanism";
		m_status = MECH_UNAVAILABLE;
		return false;
	}
	// First check if the requested mech is in the list
	for ( std::vector< std::string >::const_iterator it = m_mechs.begin();
						it != m_mechs.end(); it ++ )	{
		if ( boost::iequals( mech, *it ) )	{
			LOG_TRACE << "StandardAuthenticator: authentication mechanism '"
				  << mech << "' found";
			mechAvailable = true;
			break;
		}
	}
	if ( ! mechAvailable )	{
		m_status = MECH_UNAVAILABLE;
		LOG_DEBUG << "StandardAuthenticator: authentication mechanism '"
			  << mech << "' not found";
		return false;
	}

	for ( std::list< AuthenticationUnit* >::const_iterator it = m_authUnits.begin();
							it != m_authUnits.end(); it++ )	{
		AuthenticatorSlice* slice = (*it)->slice( mech, m_client );
		if ( slice != NULL )	{
			LOG_TRACE << "StandardAuthenticator: authentication mechanism '"
				  << mech << "' provided by '" << (*it)->className();
			m_slices.push_back( slice );
		}
	}
	// Note the the following logic is badly flawed. A lot more knowledge is needed
	// in order to do this reasonably correct
	if ( ! m_slices.empty() )	{
		m_currentSlice = 0;
		LOG_TRACE << "StandardAuthenticator: authentication slice set to '"
			  << m_slices[ m_currentSlice ]->identifier() << "'";
		switch ( m_slices[ m_currentSlice ]->status() )	{
			case AuthenticatorSlice::MESSAGE_AVAILABLE:
				m_status = MESSAGE_AVAILABLE;
				LOG_TRACE << "StandardAuthenticator: status is MESSAGE_AVAILABLE";
				break;
			case AuthenticatorSlice::AWAITING_MESSAGE:
				m_status = AWAITING_MESSAGE;
				LOG_TRACE << "StandardAuthenticator: status is AWAITING_MESSAGE";
				break;
			case AuthenticatorSlice::AUTHENTICATED:
				LOG_ERROR << "StandardAuthenticator: authentication slice '"
					  << m_slices[ m_currentSlice ]->identifier() << "' status is AUTHENTICATED at initialization";
				m_status = SYSTEM_FAILURE;
				throw std::logic_error( "StandardAuthenticator (setMech): authentication slice status is AUTHENTICATED" );
				break;
			case AuthenticatorSlice::INVALID_CREDENTIALS:
				LOG_ERROR << "StandardAuthenticator: authentication slice '"
					  << m_slices[ m_currentSlice ]->identifier() << "' status is INVALID_CREDENTIALS at initialization";
				m_status = SYSTEM_FAILURE;
				throw std::logic_error( "StandardAuthenticator (setMech): authentication slice status is INVALID_CREDENTIALS" );
				break;
			case AuthenticatorSlice::SYSTEM_FAILURE:
				LOG_WARNING << "StandardAuthenticator: authentication slice '"
					    << m_slices[ m_currentSlice ]->identifier() << "' status is SYSTEM_FAILURE";
				m_status = SYSTEM_FAILURE;
				break;
		}
		return true;
	}
	else	{
		m_status = MECH_UNAVAILABLE;
		LOG_DEBUG << "StandardAuthenticator: authentication mechanism '"
			  << mech << "' is not available for this client";
		return false;
	}
}

// The input message
void StandardAuthenticator::messageIn( const std::string& message )
{
	// Sill missing a lot here ....

	if ( m_status != AWAITING_MESSAGE )
		throw std::logic_error( "StandardAuthenticator: unexpected message received" );
	if ( m_currentSlice < 0 )
		throw std::logic_error( "StandardAuthenticator: message received but no authentication slice selected" );

	assert( (std::size_t)m_currentSlice < m_slices.size() );
	assert( m_slices[ m_currentSlice ]->status() == AuthenticatorSlice::AWAITING_MESSAGE );

	m_slices[ m_currentSlice ]->messageIn( message );

	switch( m_slices[ m_currentSlice ]->status() )	{
		case AuthenticatorSlice::MESSAGE_AVAILABLE:
			m_status = MESSAGE_AVAILABLE;
			LOG_TRACE << "StandardAuthenticator: status is MESSAGE_AVAILABLE";
			break;
		case AuthenticatorSlice::AWAITING_MESSAGE:
			m_status = AWAITING_MESSAGE;
			LOG_TRACE << "StandardAuthenticator: status is AWAITING_MESSAGE";
			break;
		case AuthenticatorSlice::AUTHENTICATED:
			m_status = AUTHENTICATED;
			LOG_TRACE << "StandardAuthenticator: status is AUTHENTICATED";
			break;
		case AuthenticatorSlice::INVALID_CREDENTIALS:
			m_status = SYSTEM_FAILURE;
			LOG_TRACE << "StandardAuthenticator: status is INVALID_CREDENTIALS";
			break;
		case AuthenticatorSlice::SYSTEM_FAILURE:
			m_status = SYSTEM_FAILURE;
			LOG_WARNING << "StandardAuthenticator: status is SYSTEM_FAILURE";
			break;
	}
}

// The output message
std::string StandardAuthenticator::messageOut()
{
	// Still missing a lot here ....

	if ( m_status != MESSAGE_AVAILABLE )
		throw std::logic_error( "StandardAuthenticator: unexpected request for output message" );
	if ( m_currentSlice < 0 )
		throw std::logic_error( "StandardAuthenticator: message requested but no authentication slice selected" );

	assert( (std::size_t)m_currentSlice < m_slices.size() );
	assert( m_slices[ m_currentSlice ]->status() == AuthenticatorSlice::MESSAGE_AVAILABLE );

	std::string msgOut = m_slices[ m_currentSlice ]->messageOut();

	switch( m_slices[ m_currentSlice ]->status() )	{
		case AuthenticatorSlice::MESSAGE_AVAILABLE:
			m_status = MESSAGE_AVAILABLE;
			LOG_TRACE << "StandardAuthenticator: status is MESSAGE_AVAILABLE";
			break;
		case AuthenticatorSlice::AWAITING_MESSAGE:
			m_status = AWAITING_MESSAGE;
			LOG_TRACE << "StandardAuthenticator: status is AWAITING_MESSAGE";
			break;
		case AuthenticatorSlice::AUTHENTICATED:
			m_status = AUTHENTICATED;
			LOG_TRACE << "StandardAuthenticator: status is AUTHENTICATED";
			break;
		case AuthenticatorSlice::INVALID_CREDENTIALS:
			m_status = SYSTEM_FAILURE;
			LOG_TRACE << "StandardAuthenticator: status is INVALID_CREDENTIALS";
			break;
		case AuthenticatorSlice::SYSTEM_FAILURE:
			m_status = SYSTEM_FAILURE;
			LOG_WARNING << "StandardAuthenticator: status is SYSTEM_FAILURE";
			break;
	}

	return msgOut;
}

// The authenticated user or NULL if not authenticated
User* StandardAuthenticator::user()
{
	AAAA::User* retUser = m_user;
	if ( m_user )
		m_user = NULL;
	return retUser;
}

}} // namespace _Wolframe::AAAA
