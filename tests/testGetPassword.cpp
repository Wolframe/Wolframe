#include <iostream>
#include <string>

#include "getPassword.hpp"

using namespace std;
using namespace _SMERP::Authentication;

int main( void )
{
	string pass = getPassword( );
	cout << "Your password is: " << pass << endl;

	return 0;
}
