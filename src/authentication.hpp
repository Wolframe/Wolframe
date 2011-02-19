//
// authentication.hpp
//

#ifndef _AUTHENTICATION_HPP_INCLUDED
#define _AUTHENTICATION_HPP_INCLUDED

#include <string>
#include <vector>
#include <map>

#include "singleton.hpp"

namespace _SMERP {
	namespace Authentication {

// the list of available authentication methods
// (TODO: make this dynamic and in modules)
class Mech {
public:
	enum AuthMech {
#ifdef _WIN32
		_SMERP_AUTH_MECH_WINAD,			/// Windows AD
		_SMERP_AUTH_MECH_WINKERB,		/// integrated Windows authentication
							/// (Kerberos)
#endif
#ifdef WITH_OPENLDAP
		_SMERP_AUTH_MECH_LDAP,			/// LDAP (Unix and Windows)
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

// the basic steps the authenticator can be in
class Step {
public:
	enum AuthStep {
		_SMERP_AUTH_STEP_SUCCESS,		/// successful authentication
		_SMERP_AUTH_STEP_FAIL,			/// authentication failed
		_SMERP_AUTH_STEP_SEND_DATA,		/// we need to send some data
		_SMERP_AUTH_STEP_RECV_DATA,		/// we require some data
		_SMERP_AUTH_STEP_GET_ERROR		/// error occurred
	};
};

// virtual base for all authentication methods
class Authenticator {		
	public:
		virtual ~Authenticator( ) { }
		
		// get next step in authentication
		virtual Step::AuthStep nextStep( ) = 0;

		// used when sending or receiving to indicate
		// the kind of data the Authenticator expects
		// (e.g. "login", "password", "md5")
		virtual std::string token( ) = 0;
		
		// the authenticator wants us to send out data
		// (for instance a challenge). The message has
		// to be send to the client
		//
		// token() gives you the kind of data (which
		// depends on the authentication method)
		virtual std::string sendData( ) = 0;
		
		// the authenticator can't continue without getting
		// some data (for instance we need a password)
		//
		// token() indicates the kind of data the authenticator
		// expects (depends on the authentication method)
		virtual void receiveData( const std::string data ) = 0;

		// we got an error (which usually should be logged only,
		// not sent to the client)
		virtual std::string getError( ) = 0;
};

// a factory returning us an authenticator for a given authentication
// method (indicated by a speaking string like 'PAM')
class AuthenticatorFactory : public Singleton< AuthenticatorFactory> {
	private:
		std::map<std::string, Authenticator *> m_authenticators;

	public:
		AuthenticatorFactory( );
		virtual ~AuthenticatorFactory( );
		
		// get a specific authenticator identified by method
		Authenticator* getAuthenticator( const std::string method );
		
		// get the list of all currently available authentication methods
		std::vector<std::string> getAvailableMechs( );
};

// map authentication method enum values to strings
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
