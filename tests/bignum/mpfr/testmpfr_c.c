#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <gmp.h>
#include <mpfr.h>

static void reverse( char *s ) {
	int i, j;
	char c;   

	for( i = 0, j = (int)strlen( s )-1; i<j; i++, j-- ) {
		c = s[i];
		s[i] = s[j];
		s[j] = c;
	}
}

static char *itoa( int value, char *string, int radix ) {
        int sign;
        int i;
        int digit;

        /* remember sign and make value positive */
        sign = value;
        if( sign < 0 ) {
                value = -value;
        }

        i = 0;
        do {
                digit = value % radix;
                value = value / radix;
                if( digit < 10 ) {
                        string[i++] = (char)( digit + '0' );
                } else {
                        string[i++] = (char)( digit - 10 + 'A' );
                }
        } while( value > 0 );

        if( sign < 0 ) {
                string[i++] = '-';
        }
        string[i] = '\0';

        reverse( string );

        return string;
}

static char *mpfr2str( mpfr_t a, char *s, size_t len )
{
	mp_exp_t e;
	char *s1, *s2;
	char *p1;
	char *d;
	
	mpfr_out_str( stdout, 10, 0, a, GMP_RNDD );
	puts( "" );
	
	if( mpfr_nan_p( a ) ) {
		if( len < 4 ) return NULL;
		strcpy( s, "NaN" );
	}
	
	if( mpfr_inf_p( a ) ) {
		if( len < 5 ) return NULL;
		if( MPFR_SIGN( a ) )
			strcpy( s, "Inf" );
		else
			strcpy( s, "-Inf" );
	}

	if( mpfr_zero_p( a ) ) {
		if( len < 3 ) return NULL;
		if( MPFR_SIGN( a ) )
			strcpy( s, "0" );
		else
			strcpy( s, "-0" );
	}
	
	s1 = mpfr_get_str( NULL, &e, 10, 0, a, GMP_RNDD );
	s2 = mpfr_get_str( NULL, &e, 10, 10, a, GMP_RNDD );
	
	if( s1 == NULL || s2 == NULL ) return NULL;

	if( len < strlen( s1 ) + strlen( s2 ) + 4 ) return NULL;

	if( strlen( s1 ) < strlen( s2 ) ) {
		mpfr_free_str( s2 );
	} else {
		mpfr_free_str( s1 );
		s1 = s2;
	}
	
	d = s;
	p1 = s1;

	/* handle optional minus sign */
	if( *p1 == '-' )
		*d++ = *p1++;
    
	/* append leading digit of mantissa */
	*d++ = *p1++; e--;
	*d++ = '.';
	*d = '\0';
	
	/* rest of mantissa digits */
	strcat( s, p1 );
	d += strlen( p1 );
			
	/* exponent */
	if( e > 0 ) {
		*d++ = 'E';
		*d = '\0';
		(void)itoa( e, d, 10 );
	}
	
	mpfr_free_str( s1 );
	
	return s;
}	
    
int main( int argc, char *argv[] )
{
	mpfr_t a, b, c;
	char sa[100];
	char sb[100];
	char sc[100];

	if( argc != 3 ) {
		fprintf( stderr, "Expecting two floats\n" );
		return 1;
	}

	mpfr_init2( a, 200 );
	mpfr_set_str( a, argv[1], 10, GMP_RNDD );
	mpfr_init2( b, 200 );
	mpfr_set_str( b, argv[2], 10, GMP_RNDD );
	mpfr_init2( c, 200 );
	
	mpfr_add( c, a, b, GMP_RNDD );

	printf( "%s + %s => %s\n",
		mpfr2str( a, sa, 100 ),
		mpfr2str( b, sb, 100 ),
		mpfr2str( c, sc, 100 ) );

	mpfr_clear( a );
	mpfr_clear( b );
	mpfr_clear( c );

	return 0;
}
