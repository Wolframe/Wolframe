#include <iostream>
#include <cstdlib>
#include <boost/asio/ssl.hpp> // Memory Leak

using namespace std;

int main(int argc, char *argv[])
{
	cout << "Hello, world !" << endl;
	ERR_remove_state( 0 );
	ENGINE_cleanup();
	CONF_modules_unload( 1 );
	ERR_free_strings();
	EVP_cleanup();
	CRYPTO_cleanup_all_ex_data(); 
	return 0;
}

