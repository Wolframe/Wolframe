// note: use C+11 <random> later

#include <fstream>
#include <string>

#ifndef _WIN32
#include <time.h>
#include <stdlib.h>
#endif

#include "logger-v1.hpp"

// to make linking in this directory possible
_Wolframe::log::LogBackend* logBackendPtr;

using namespace std;

static int intRand( int min, int max )
{
	return ( (double)rand( ) / ( (double)RAND_MAX + 1.0 ) ) * ( max - min + 1 ) + min;
}

int main( int argc, char *argv[] )
{
	if( argc != 3 ) {
		return 1;
	}
	
	fstream f( argv[2], ios::out | ios::binary );
	
	int N = atoi( argv[1] );
	char *buf = new char[N];
	 
	srand( time( 0 ) );
	for( int i = 0; i < N; i++ ) {
		buf[i] = (unsigned char )intRand( 0, 255 );
	}
		
	f.write( buf, N );
	
	f.close( );

	delete[] buf;

	return 0;
}
