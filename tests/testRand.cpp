#include <iostream>
#ifdef _WIN32
#include <windows.h>
#endif

int main()
{
#ifdef _WIN32
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
		std::cout << std::hex << static_cast<unsigned int>( buf[i] );
	}
	std::cout << std::endl;

	if( !CryptReleaseContext( provider, 0 ) ) {
		return 1;
	}
#endif

	return 0;
}