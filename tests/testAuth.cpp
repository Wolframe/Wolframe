#include <iostream>
#include <string>

#include "authentication.hpp"
#include "getPassword.hpp"

using namespace std;
using namespace _SMERP::Authentication;

int main( void )
{
	vector<string> mechs = AuthenticatorFactory::instance( ).getAvailableMechs( );
	cout << "Available authentication methods: ";
	for( vector<string>::const_iterator it = mechs.begin( ); it != mechs.end( ); it++ )
		cout << *it << " ";
	cout << endl;
			
	Authenticator *a = AuthenticatorFactory::instance( ).getAuthenticator( "PAM" );

	string pass = getPassword( );
	cout << "Your password is: " << pass << endl;

	return 0;
}
