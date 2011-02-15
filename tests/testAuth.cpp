#include <iostream>
#include <string>

#include "authentication.hpp"
#include "getPassword.hpp"

using namespace std;
using namespace _SMERP::Authentication;

#ifndef _WIN32
#include <unistd.h>
#endif

int main( void )
{
// get list of available authentication methods
	vector<string> mechs = AuthenticatorFactory::instance( ).getAvailableMechs( );
	cout << "Available authentication methods: ";
	for( vector<string>::const_iterator it = mechs.begin( ); it != mechs.end( ); it++ )
		cout << *it << " ";
	cout << endl;

// get a specific authenticator			
	Authenticator *a = AuthenticatorFactory::instance( ).getAuthenticator( "TEXT_FILE" );

// go in a loop where we do what the authenticator tells us, in
// the simplest case it asks us for a login and a password
	Step::AuthStep step = a->nextStep( );
	while( step != Step::_SMERP_AUTH_STEP_SUCCESS &&step != Step::_SMERP_AUTH_STEP_FAIL ) {

// the authenticator needs to send some data to the client side		
		if( step == Step::_SMERP_AUTH_STEP_SEND_DATA ) {
			std::string data = a->sendData( );
			
// the authenticate needs some specific input from the client
		} else if( step == Step::_SMERP_AUTH_STEP_RECV_DATA ) {
			string token = a->token( );
// safe password input required
			if( token == "password" ) {
				string pass = getPassword( );
				a->receiveData( pass );
// login name required
			} else if( token == "login" ) {
#ifndef _WIN32
				string login = getlogin( );
#else
// TODO: get username on Windows
				string login = "dummy";
#endif
				a->receiveData( login );
			} else {
				cerr << "authenticator requests unknown token '" << token << "'" << endl;
				return 1;
			}
		}

// next step
		step = a->nextStep( );
	}

// no we are either authenticated or not
	if( step == Step::_SMERP_AUTH_STEP_SUCCESS ) {
		cout << "Authentication succeeded!" << endl;
	} else if( step == Step::_SMERP_AUTH_STEP_FAIL ) {
		cout << "Authentication failed!" << endl;
	}

	return ( step != Step::_SMERP_AUTH_STEP_SUCCESS );
}
