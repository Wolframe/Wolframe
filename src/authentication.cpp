//
// authentication.cpp
//

#include "authentication.hpp"

// badbad, see below
#include "authentication_textfile.hpp"
#ifdef WITH_PAM
#include "authentication_pam.hpp"
#endif

#include <utility>

namespace _SMERP {
	namespace Authentication {

Mech::AuthMech str2AuthMech( const std::string s )
{
	if( s == "TEXT_FILE" )		return Mech::_SMERP_AUTH_MECH_TEXT_FILE;
#ifdef _WIN32
	else if( s == "WINAD" )		return Mech::_SMERP_AUTH_MECH_WINAD;
	else if( s == "WINKERB" )	return Mech::_SMERP_AUTH_MECH_WINKERB;
#endif
#ifdef WITH_OPENLDAP
	else if( s == "LDAP" )		return Mech::_SMERP_AUTH_MECH_LDAP;
#endif
#ifdef WITH_PAM
	else if( s == "PAM" )		return Mech::_SMERP_AUTH_MECH_PAM;
#endif
#ifdef WITH_SASL
	else if( s == "SASL" )		return Mech::_SMERP_AUTH_MECH_SASL;
#endif
#ifdef WITH_GSSAPI
	else if( s == "GSSAPI" )	return Mech::_SMERP_AUTH_MECH_GSSAPI;
#endif
	else				return Mech::_SMERP_AUTH_MECH_UNDEFINED;
}

AuthenticatorFactory::AuthenticatorFactory( )
{
	// TODO: register over loadable module loader and a register function
	// with creation functors, but later :-)
	
	// TODO: where to get parameters for creation from outside?
	
	m_authenticators.insert( std::make_pair<std::string, Authenticator *>("TEXT_FILE", new TextFileAuthenticator( "creds.conf" ) ) );
#ifdef WITH_PAM
	m_authenticators.insert( std::make_pair<std::string, Authenticator *>( "PAM", new PAMAuthenticator( "smerp" ) ) );
#endif
}

AuthenticatorFactory::~AuthenticatorFactory( )
{
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
		
	mechs.push_back( "TEXT_FILE");
#ifdef _WIN32
	mechs.push_back( "WINAD" );
	mechs.push_back( "WINKERB" );
#endif
#ifdef WITH_OPENLDAP
	mechs.push_back( "LDAP" );
#endif
#ifdef WITH_PAM
	mechs.push_back( "PAM" );
#endif
#ifdef WITH_SASL
	mechs.push_back( "SASL" );
#endif
#ifdef WITH_GSSAPI
	mechs.push_back( "GSSAPI" );
#endif
	
	return mechs;
}

} // namespace Authentication
} // namespace _SMERP

