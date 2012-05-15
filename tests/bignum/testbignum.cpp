#include <iostream>
#include "types/bignum.hpp"

using namespace std;
using namespace _Wolframe::types;

int main( int argc, char *argv[] )
{
	if( argc != 3 ) {
		cerr << "Expecting two big numbers" << endl;
		return 1;
	}

	Bignum a( argv[1] ), b( argv[2] );
	Bignum c = a + b;
	
	cout << a << " + " << b << " => " << c << endl;
	
	return 0;
}
