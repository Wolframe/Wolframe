//
// authentication.hpp
//

#ifndef _AUTHENTICATION_HPP_INCLUDED
#define _AUTHENTICATION_HPP_INCLUDED

#include <string>
#include <vector>
#include <map>
#include <list>
#include <stdexcept>
#include <boost/any.hpp>

#include "singleton.hpp"

namespace _SMERP {
	namespace Authentication {

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
	public:
		struct property {
			property( );
			property( const std::string &_name, const boost::any &_value )
				: name( _name ), value( _value ) { }
			std::string name;
			boost::any value;
			
			bool operator==( const std::string &_name ) const
			{
				return name == _name;
			}
		};
		typedef std::list<property> properties;
		
		typedef Authenticator* (*CreateAuthenticatorFunc)( properties props );
		
	private:
		std::map<std::string, Authenticator *> m_authenticators;

	public:
		AuthenticatorFactory( );
		virtual ~AuthenticatorFactory( );
		
		void registerAuthenticator(	std::string _method,
						CreateAuthenticatorFunc _createf,
						properties _props );
		
		void unregisterAuthenticator( std::string _method );
		
		// get a specific authenticator identified by method
		Authenticator* getAuthenticator( const std::string method );
		
		// get the list of all currently available authentication methods
		std::vector<std::string> getAvailableMechs( );
};

template<class T> T findprop( AuthenticatorFactory::properties _props, const std::string _name )
{
	std::list<AuthenticatorFactory::property>::const_iterator it;
	it = std::find( _props.begin( ), _props.end( ), _name );
	if( it != _props.end( ) ) {
		return boost::any_cast<T>( it->value );
	}
	throw std::logic_error( "bad argument in property set" );
}

} // namespace Authentication
} // namespace _SMERP

#endif // _AUTHENTICATION_HPP_INCLUDED
