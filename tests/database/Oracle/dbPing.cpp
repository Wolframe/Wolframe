#include <iostream>
#include <sstream>
#include <cstring>

#include "logger-v1.hpp"

// to make linking in this directory possible
_Wolframe::log::LogBackend*	logBackendPtr;

using namespace std;
using namespace _Wolframe::log;

#include <oci.h>

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
	
	int rt = 1;
	
	char *host = argv[1];
	char *user = argv[2];
	char *password = argv[3];
	char *database = argv[4];

	ostringstream ss;
	ss << "(DESCRIPTION=(ADDRESS=(PROTOCOL=TCP)"
		<< "(HOST=" << host << ")"
		<< "(PORT=" << 1521 << "))"
		<< "(CONNECT_DATA=(SID=" << database << ")"
		<< "))";
	string connStr = ss.str( );

	OCIEnv *envhp = 0;
	OCIError *errhp = 0;
	OCIServer *srvhp = 0;
	OCISvcCtx *svchp = 0;
	OCISession *authp = 0;

	sword status;
	
	// create an Oracle OCI environment (global per process), what
	// options do we really need (charset, mutex, threading, pooling)?
	status = OCIEnvCreate( &envhp, OCI_DEFAULT, (dvoid *)0,
		0, 0, 0, 0, (dvoid **)0 );
	if( status != OCI_SUCCESS ) goto cleanup;
	
	status = OCIHandleAlloc( envhp, (dvoid **)&srvhp, OCI_HTYPE_SERVER, (size_t)0, (dvoid **)0 );
	if( status != OCI_SUCCESS ) goto cleanup;

	status = OCIHandleAlloc( envhp, (dvoid **)&errhp, OCI_HTYPE_ERROR, (size_t)0, (dvoid **)0 );
	if( status != OCI_SUCCESS ) goto cleanup;

	status = OCIHandleAlloc( envhp, (dvoid **)&svchp, OCI_HTYPE_SVCCTX, (size_t)0, (dvoid **)0 );
	if( status != OCI_SUCCESS ) goto cleanup;

	status = OCIServerAttach( srvhp, errhp,
		connStr.empty( ) ? NULL : (CONST text *)( connStr.c_str( ) ),
		connStr.empty( ) ? (sb4)0 : (sb4)( connStr.length( ) ),
		OCI_DEFAULT );
	if( status != OCI_SUCCESS ) goto cleanup;
		
	status = OCIAttrSet( svchp, OCI_HTYPE_SVCCTX,
		srvhp, (ub4)0, OCI_ATTR_SERVER,
		(OCIError *)errhp );		
	if( status != OCI_SUCCESS ) goto cleanup;
	
	status = OCIHandleAlloc( envhp, (dvoid **)&authp,
		OCI_HTYPE_SESSION, (size_t)0, (dvoid **)0 );
	if( status != OCI_SUCCESS ) goto cleanup;
	
	status = OCIAttrSet( authp, OCI_HTYPE_SESSION,
		(dvoid *)user, (ub4)strlen( user ),
		OCI_ATTR_USERNAME, errhp );
	if( status != OCI_SUCCESS ) goto cleanup;

	status = OCIAttrSet( authp, OCI_HTYPE_SESSION,
		(dvoid *)password, (ub4)strlen( password ),
		OCI_ATTR_PASSWORD, errhp );
	if( status != OCI_SUCCESS ) goto cleanup;

	status = OCISessionBegin( svchp, errhp, authp,
		OCI_CRED_RDBMS, OCI_DEFAULT );
	if( status != OCI_SUCCESS ) goto cleanup;

	status = OCIAttrSet( svchp, OCI_HTYPE_SVCCTX,
		authp, (ub4)0, OCI_ATTR_SESSION, errhp );
	if( status != OCI_SUCCESS ) goto cleanup;

	// success
	rt = 0;
	
cleanup:

	if( srvhp && errhp && authp ) (void)OCISessionEnd( svchp, errhp, authp, OCI_DEFAULT );
	if( srvhp && errhp ) (void)OCIServerDetach( srvhp, errhp, OCI_DEFAULT );
	if( authp ) (void)OCIHandleFree( authp, OCI_HTYPE_SESSION );
	if( svchp ) (void)OCIHandleFree( svchp, OCI_HTYPE_SVCCTX );
	if( errhp ) (void)OCIHandleFree( errhp, OCI_HTYPE_ERROR );
	if( srvhp ) (void)OCIHandleFree( srvhp, OCI_HTYPE_SERVER );
	if( envhp ) (void)OCIHandleFree( envhp, OCI_HTYPE_ENV );
	
	return rt;
}
