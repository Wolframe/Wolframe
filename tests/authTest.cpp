#include <iostream>
#include <string>
#include <algorithm>

#include "AAAA/authentication.hpp"
#include "getPassword.hpp"

using namespace std;
using namespace _Wolframe::AAAA;

static void print_usage( ) {
// get list of available authentication methods
	vector<string> mechs = AuthenticatorFactory::instance( ).getAvailableMechs( );
	cerr << "Available authentication methods: ";	
	for( vector<string>::const_iterator it = mechs.begin( ); it != mechs.end( ); it++ )
		cerr << *it << " ";
	cerr << endl;
}

int main( int argc, const char *argv[] )
{
// register some authentication methods
	AuthenticatorFactory::properties props;
	props.push_back( AuthenticatorFactory::property( "filename", std::string( "passwd" ) ) );
	AuthenticatorFactory::instance( ).registerAuthenticator( "TEXT_FILE", CreateTextFileAuthenticator, props );

#ifdef WITH_PAM
	AuthenticatorFactory::properties props2;
	props2.push_back( AuthenticatorFactory::property( "service", std::string( "wolframe" ) ) );
	AuthenticatorFactory::instance( ).registerAuthenticator( "PAM", CreatePAMAuthenticator, props2 );
#endif

#ifdef WITH_SQLITE3
	AuthenticatorFactory::properties props3;
	props3.push_back( AuthenticatorFactory::property( "filename", std::string( "passwd.db" ) ) );
	AuthenticatorFactory::instance( ).registerAuthenticator( "DBSQLITE3", CreateDbSqlite3Authenticator, props3 );
#endif

#ifdef WITH_SASL
	AuthenticatorFactory::properties props4;
	props4.push_back( AuthenticatorFactory::property( "appname", std::string( "authTest" ) ) );
	props4.push_back( AuthenticatorFactory::property( "service", std::string( "wolframe" ) ) );
	AuthenticatorFactory::instance( ).registerAuthenticator( "SASL", CreateSaslAuthenticator, props4 );
#endif

// check parameters
	if( argc != 2 ) {
		cerr << "usage: testAuth <authentication method>" << endl;
		print_usage( );
		return 1;
	}

// check if authentication method exists
	vector<string> mechs = AuthenticatorFactory::instance( ).getAvailableMechs( );
	vector<string>::const_iterator it = find( mechs.begin( ), mechs.end( ), argv[1] );
	if( it == mechs.end( ) ) {
		print_usage( );
		return 1;
	}

// get a specific authenticator			
	Authenticator *a = AuthenticatorFactory::instance( ).getAuthenticator( argv[1] );
	
// go in a loop where we do what the authenticator tells us, in
// the simplest case it asks us for a login and a password
	Step::AuthStep step = a->nextStep( );
	while( step != Step::_Wolframe_AUTH_STEP_SUCCESS &&step != Step::_Wolframe_AUTH_STEP_FAIL ) {

// the authenticator needs to send some data to the client side		
		if( step == Step::_Wolframe_AUTH_STEP_SEND_DATA ) {
			string token = a->token( );
			string data = a->sendData( );

// the authenticate needs some specific input from the client
		} else if( step == Step::_Wolframe_AUTH_STEP_RECV_DATA ) {
			string token = a->token( );
// safe password input required
			if( token == "password" ) {
				string pass = getPassword( );
				a->receiveData( pass );
// login name required
			} else if( token == "login" ) {
				string login = getLogin( );
				cout << "login is: " << login << endl;
				a->receiveData( login );
			} else {
				cerr << "authenticator requests unknown token '" << token << "'" << endl;
				return 1;
			}

// an error occurred, get error message and print it
		} else if( step == Step::_Wolframe_AUTH_STEP_GET_ERROR ) {
			cerr << "ERROR: " << a->getError( ) << endl;
		}
// next step
		step = a->nextStep( );
	}

// no we are either authenticated or not
	if( step == Step::_Wolframe_AUTH_STEP_SUCCESS ) {
		cout << "Authentication succeeded!" << endl;
	} else if( step == Step::_Wolframe_AUTH_STEP_FAIL ) {
		cout << "Authentication failed!" << endl;
	}

	return ( step != Step::_Wolframe_AUTH_STEP_SUCCESS );
}
