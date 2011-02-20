//
// authentication.cpp
//

#include "authentication.hpp"

namespace _SMERP {
	namespace Authentication {

AuthenticatorFactory::AuthenticatorFactory( )
{
}

AuthenticatorFactory::~AuthenticatorFactory( )
{
	std::map<std::string, Authenticator *>::iterator it;
	for( it = m_authenticators.begin( ); it != m_authenticators.end( ); it++ )
		delete it->second;
}

void AuthenticatorFactory::registerAuthenticator(	std::string _method,
							CreateAuthenticatorFunc _createf,
							properties _props )
{
	m_authenticators.insert( std::make_pair<std::string, Authenticator *>( _method, _createf( _props ) ) );
}
		
void AuthenticatorFactory::unregisterAuthenticator( std::string _method )
{
	m_authenticators.erase( _method );
}

Authenticator* AuthenticatorFactory::getAuthenticator( const std::string method )
{
	std::map<std::string, Authenticator *>::const_iterator it = m_authenticators.find( method );
	if( it != m_authenticators.end( ) )
		return it->second;

	return 0;
}

std::vector<std::string> AuthenticatorFactory::getAvailableMechs( )
{
	std::vector<std::string> mechs;
	std::map<std::string, Authenticator *>::const_iterator it;
	for( it = m_authenticators.begin( ); it != m_authenticators.end( ); it++ )
		mechs.push_back( it->first );
	return mechs;
}

} // namespace Authentication
} // namespace _SMERP

