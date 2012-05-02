#include <iostream>
#include <iomanip>

#include "gtest/gtest.h"

#ifndef _WIN32
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#else
#define WIN32_MEAN_AND_LEAN
#include <windows.h>
#endif

TEST( Random, Randomness )
{
	unsigned char buf[39];
	
#ifndef _WIN32
	ssize_t bytes;
	
	int f = open( "/dev/urandom", O_RDONLY );
	if( f < 0 ) {
		FAIL( ) << "open /dev/urandom";
	}

	if( ( bytes = read( f, (void *)buf, 39 ) ) < 0 ) {
		(void)close( f );
		FAIL( ) << "read error";
	}
	
	if( bytes != 39 ) {
		FAIL( ) << "short read, got only " << bytes << " bytes";
	}

	(void)close( f );
#else
	HCRYPTPROV provider = 0;

	if( !CryptAcquireContext( &provider, 0, MS_DEF_PROV, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_SILENT ) ) {
		FAIL( ) << "CryptGenRandom";
	}

	if( !CryptGenRandom( provider, 39, static_cast<BYTE *>( buf ) ) ) {
		CryptReleaseContext( provider, 0 );
		FAIL( ) << "CryptGenRandom";
	}

	if( !CryptReleaseContext( provider, 0 ) ) {
		FAIL( ) << "CryptReleaseContext";
	}
#endif

	for( int i = 0; i < 39; i++ ) { 
		std::cout 	<< std::hex << std::setfill( '0' ) << std::setw( 2 )
				<< static_cast<unsigned int>( buf[i] );
	}
	std::cout << std::endl;

	SUCCEED( );
}

int main( int argc, char **argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS( );
}
