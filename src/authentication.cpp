//
// authentication.cpp
//

#include "authentication.hpp"

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

Authenticator* AuthenticatorFactory::getAuthenticator( const std::string method )
{
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

