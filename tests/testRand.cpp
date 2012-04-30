#include <iostream>
#include <iomanip>

#ifndef _WIN32
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#else
#define WIN32_MEAN_AND_LEAN
#include <windows.h>
#endif

int main()
{
	unsigned char buf[39];

	int f = open( "/dev/urandom", O_RDONLY );

	if( read( f, (void *)buf, 39 ) < 0 ) {
		(void)close( f );
		return 1;
	}

	for( int i = 0; i < 39; i++ ) { 
		std::cout 	<< std::hex << std::setfill( '0' ) << std::setw( 2 )
				<< static_cast<unsigned int>( buf[i] );
	}
	std::cout << std::endl;

	if( f < 0 ) {
		return 1;
	}

	(void)close( f );

#ifndef _WIN32
#else
	HCRYPTPROV provider = 0;

	if( !CryptAcquireContextW( &provider, 0, 0, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_SILENT ) ) {
		return 1;
	}

	BYTE buf[39];

	if( !CryptGenRandom( provider, 39, buf ) ) {
		CryptReleaseContext( provider, 0 );
		return 1;
	}

	for( int i = 0; i < 39; i++ ) { 
    std::cout 	<< std::hex << std::setfill( '0' ) << std::setw( 2 )
				<< static_cast<unsigned int>( buf[i] );
	}
	std::cout << std::endl;

	if( !CryptReleaseContext( provider, 0 ) ) {
		return 1;
	}
#endif

	return 0;
}