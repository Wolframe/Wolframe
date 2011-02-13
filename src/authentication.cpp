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

} // namespace Authentication
} // namespace _SMERP
