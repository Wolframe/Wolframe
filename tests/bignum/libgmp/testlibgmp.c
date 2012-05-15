#include <stdio.h>
#include <gmp.h>

int main( int argc, char *argv[] )
{
	mpz_t a, b, c;

	if( argc != 3 ) {
		fprintf( stderr, "Expecting two integers\n" );
		return 1;
	}

	mpz_init_set_str( a, argv[1], 10 );
	mpz_init_set_str( b, argv[2], 10 );
	mpz_init( c );
	mpz_add( c, a, b );

	printf( "%s + %s => %s\n",
		mpz_get_str( NULL, 10, a ),
		mpz_get_str( NULL, 10, b ),
		mpz_get_str( NULL, 10, c ) );

	mpz_clear( a );
	mpz_clear( b );
	mpz_clear( c );

	return 0;
}
