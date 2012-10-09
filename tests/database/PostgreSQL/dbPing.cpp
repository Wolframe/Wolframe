#include <iostream>
#include <sstream>

// we need this program in the Windows makefile, we can't assume
// Postgresql to be installed with psql.exe (nmake and msdev build
// only build libpq.dll)

#include "logger-v1.hpp"

// to make linking in this directory possible
_Wolframe::log::LogBackend*	logBackendPtr;

using namespace std;
using namespace _Wolframe::log;

#include <libpq-fe.h>

int main( int argc, char *argv[] )
{
	// for modules to log (HACK)
	LogBackend &logBack = LogBackend::instance( );
	logBackendPtr = &logBack;
	
	logBack.setConsoleLevel( LogLevel::LOGLEVEL_DEBUG );

	if( argc != 5 ) {
		cerr << "Usage: dbPing <host> <user> <password> <database>" << endl;
		return 1;
	}
	
	char *host = argv[1];
	char *user = argv[2];
	char *password = argv[3];
	char *database = argv[4];

	ostringstream ss;
	ss 	<< "host = '" << host << "' "
		<< "user = '" << user << "' "
		<< "password = '" << password << "' "
		<< "dbname = '" << database << "'";

	PGconn* conn = PQconnectdb( ss.str( ).c_str( ) );
	if( conn == NULL ) {
		return 1;
	}
	
	ConnStatusType stat = PQstatus( conn );
	if( stat != CONNECTION_OK ) {
		PQfinish( conn );
		return 1;
	}
	
	PQfinish( conn );
	return 0;
}
