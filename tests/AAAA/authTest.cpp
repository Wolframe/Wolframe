#include <iostream>
#include <string>
#include <algorithm>

#include "AAAA/authentication_textfile.hpp"
#include "AAAA/authentication_pam.hpp"
#include "AAAA/authentication_db_sqlite3.hpp"
#include "AAAA/authentication_sasl.hpp"

#include "getPassword.hpp"

using namespace std;
using namespace _Wolframe::AAAA;

static void print_usage( ) {
	cerr << "Available authentication methods: DBSQLITE3 PAM SASL TEXTFILE" << endl;
}

static Authenticator* getAuthenticator( const std::string s )
{
	if( s == "TEXTFILE" ) {
		return new TextFileAuthenticator( "passwd" );
#ifdef WITH_PAM
	} else if( s == "PAM" ) {
		return new PAMAuthenticator( "wolframe" );
#endif
#ifdef WITH_SQLITE3
	} else if( s == "DBSQLITE3" ) {
		return new DbSqlite3Authenticator( "passwd.db" );
#endif
#ifdef WITH_SASL
	} else if( s == "SASL" ) {
		return new SaslAuthenticator( "authTest", "wolframe", "." );
#endif
	} else {
		return 0;
	}
}

int main( int argc, const char *argv[] )
{
// check parameters
	if( argc != 2 ) {
		cerr << "usage: testAuth <authentication method>" << endl;
		print_usage( );
		return 1;
	}

// get a specific authenticator			
	Authenticator *a = getAuthenticator( argv[1] );
	if( !a ) {
		print_usage( );
		return 1;
	}
	
// go in a loop where we do what the authenticator tells us, in
// the simplest case it asks us for a login and a password
	Step::AuthStep step = a->nextStep( );
	while( step != Step::_Wolframe_AUTH_STEP_SUCCESS &&step != Step::_Wolframe_AUTH_STEP_FAIL ) {

// the authenticator needs to send some data to the client side		
		if( step == Step::_Wolframe_AUTH_STEP_SEND_DATA ) {
			string token = a->token( );
			string data = a->sendData( );

#ifdef WITH_SASL
// SASL mechanisms offered by the server
			if( token == "SASL_mechs" ) {
				cout << "Available SASL mechs: " << data << endl;
			} else if( token == "SASL_data" ) {
				cout << "Got SASL data: " << data << endl;
			} else {
#else
			if( true ) {
#endif // WITH_SASL
				cerr << "authenticator sends us unknown token '" << token << "'" << endl;
			}

// the authenticate needs some specific input from the client
		} else if( step == Step::_Wolframe_AUTH_STEP_RECV_DATA ) {
			string token = a->token( );
// safe password input required
			if( token == "password" ) {
				string pass = getPassword( );
				a->receiveData( pass );
				
// login name required
			} else if( token == "login" ) {
				char login[256];
				cout << "login: "; cout.flush( );
				cin.getline( login, 256 );
				a->receiveData( login );
#ifdef WITH_SASL
// SASL mechanism to use
			} else if( token == "SASL_mech" ) {
				char mech[256];
				cout << "Choose a SASL mech: "; cout.flush( );
				cin.getline( mech, 256 );
				a->receiveData( mech );
			} else if( token == "SASL_data" ) {
				char data[1024];
				unsigned int pos = 0;
				cout << "SASL data: "; cout.flush( );
				do {
					data[pos] = cin.get( );
					pos++;
				} while( data[pos-1] != '\n' );
				string s( data, pos-1 );
				a->receiveData( s );
#endif // WITH_SASL
			} else {
				cerr << "authenticator requests unknown token '" << token << "'" << endl;
				return 1;
			}

// an error occurred, get error message and print it
		} else if( step == Step::_Wolframe_AUTH_STEP_GET_ERROR ) {
			cerr << "ERROR: " << a->getError( ) << endl;
		} else if( step == Step::_Wolframe_AUTH_STEP_NEXT ) {
			// call next step only, nothing special to do
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
