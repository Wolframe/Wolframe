//
// authentication.hpp
//

#ifndef _AUTHENTICATION_HPP_INCLUDED
#define _AUTHENTICATION_HPP_INCLUDED

#include <string>

namespace _SMERP {
	namespace Authentication {

class Mech {
public:
	enum AuthMech {
#ifdef _WIN32
		_SMERP_AUTH_MECH_WINAD,			// Windows AD
		_SMERP_AUTH_MECH_WINKERB,		// integrated Windows authentication
							// (Kerberos)
#endif
#ifdef WITH_OPENLDAP
		_SMERP_AUTH_MECH_LDAP,			// LDAP (Unix and Windows)
#endif
#ifdef WITH_PAM
		_SMERP_AUTH_MECH_PAM,			/// Suns PAM
#endif
#ifdef WITH_SASL
		_SMERP_AUTH_MECH_SASL,			/// Cyrus SASL
#endif
#ifdef WITH_GSSAPI
		_SMERP_AUTH_MECH_GSSAPI,		/// GSSAPI
#endif
		_SMERP_AUTH_MECH_TEXT_FILE,		/// simple textfile
		_SMERP_AUTH_MECH_UNDEFINED
	};

	static AuthMech str2AuthMech( const std::string s );
};

// map enum values to strings
template< typename CharT, typename TraitsT >
inline std::basic_ostream< CharT, TraitsT > &operator<< ( std::basic_ostream< CharT, TraitsT >& s, Mech::AuthMech f )
{
	static const CharT *const str[] = {
		"WINAD", "WINKERB", "LDAP", "PAM", "SASL", "GSSAPI", "TEXT_FILE" };
	if( static_cast< size_t >( f ) < ( sizeof( str ) / sizeof( *str ) ) ) {
		s << str[f];
	} else {
		s << "Unknown enum used '" << static_cast< int >( f ) << "'";
	}
	return s;
}

} // namespace Authentication
} // namespace _SMERP

#endif // _AUTHENTICATION_HPP_INCLUDED
