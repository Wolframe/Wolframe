#include <iostream>
#include <gmpxx.h>

int main( int argc, char *argv[] )
{
	mpz_class a, b, c;

	if( argc != 3 ) {
		std::cerr << "Expecting two integers" << std::endl;
		return 1;
	}

	a = argv[1];
	b = argv[2];
	c = a + b;

	std::cout << a << " + " << b << " => " << c << std::endl;

	return 0;
}
