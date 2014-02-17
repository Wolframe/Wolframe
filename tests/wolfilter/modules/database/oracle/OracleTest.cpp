/************************************************************************

 Copyright (C) 2011 - 2014 Project Wolframe.
 All rights reserved.

 This file is part of Project Wolframe.

 Commercial Usage
    Licensees holding valid Project Wolframe Commercial licenses may
    use this file in accordance with the Project Wolframe
    Commercial License Agreement provided with the Software or,
    alternatively, in accordance with the terms contained
    in a written agreement between the licensee and Project Wolframe.

 GNU General Public License Usage
    Alternatively, you can redistribute this file and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Wolframe is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
//
// Wolframe Oracle test client implementation
//
#include "logger-v1.hpp"
#include "OracleTest.hpp"
#include "utils/fileUtils.hpp"
#include "utils/stringUtils.hpp"
#include <boost/system/error_code.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <fstream>

using namespace _Wolframe;
using namespace _Wolframe::db;

static std::string buildConnStr( const std::string& host, unsigned short port,
				 const std::string& /* user */, const std::string& /* password */,
				 const std::string& dbName )
{
	std::stringstream ss;
	
	// everything empty, assume default host (whatever that is)
	if( host.empty( ) && dbName.empty( ) ) {
		return "";
	}

	// only a dbName, assume this is a dblink in tnsnames.ora
	if( host.empty( ) && !dbName.empty( ) ) {
		return dbName;
	}
	
	// assume default Oracle listener port, if port is undefined
	if( port == 0 ) port = 1521;

	// otherwise compose a connection string
	// TODO: needs improvement!
	ss << "(DESCRIPTION=(ADDRESS=(PROTOCOL=TCP)"
		<< "(HOST=" << host << ")"
		<< "(PORT=" << port << "))"
		<< "(CONNECT_DATA=(SID=" << dbName << ")"
		<< "))";

	return ss.str( );
}

static std::string getErrorMsg( sword status, OCIError *errhp )
{
	sb4 errcode = 0;
	std::ostringstream os;
	text errbuf[512];
	
	switch( status ) {
		case OCI_SUCCESS:
			os << "OCI_SUCCESS";
			break;

		case OCI_SUCCESS_WITH_INFO:
			os << "OCI_SUCCESS_WITH_INFO";
			break;
		
		case OCI_NEED_DATA:
			os << "OCI_NEED_DATA";
			break;
		
		case OCI_NO_DATA:
			os << "OCI_NO_DATA";
			break;
		
		case OCI_INVALID_HANDLE:
			os << "OCI_INVALID_HANDLE";
			break;
		
		case OCI_STILL_EXECUTING:
			os << "OCI_STILL_EXECUTING";
			break;
		
		case OCI_CONTINUE:
			os << "OCI_CONTINUE";
			break;
			
		case OCI_ERROR:
			(void)OCIErrorGet( (dvoid *)errhp, (ub4)1, (text *)NULL,
				&errcode, errbuf, (ub4)sizeof( errbuf ), OCI_HTYPE_ERROR );
			os << errbuf;
			break;
	}
	return os.str( );
}

static void createTestDatabase_( const std::string& host, unsigned short port,
				 const std::string& user, const std::string& password,
				 const std::string& dbName, const std::string& inputfile )
{
	std::string connStr = buildConnStr( host, port, user, password, dbName );
	OCIEnv *envhp = 0;
	OCIError *errhp = 0;
	OCIServer *srvhp = 0;
	OCISvcCtx *svchp = 0;
	OCISession *authp = 0;
	OCIStmt *stmthp = 0;
	OCIDefine *defhp = 0;
	
	sword status;

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
		(dvoid *)const_cast<char *>( user.c_str( ) ), (ub4)user.length( ),
		OCI_ATTR_USERNAME, errhp );
	if( status != OCI_SUCCESS ) goto cleanup;

	status = OCIAttrSet( authp, OCI_HTYPE_SESSION,
		(dvoid *)const_cast<char *>( password.c_str( ) ), (ub4)password.length( ),
		OCI_ATTR_PASSWORD, errhp );
	if( status != OCI_SUCCESS ) goto cleanup;

	status = OCISessionBegin( svchp, errhp, authp,
		OCI_CRED_RDBMS, OCI_DEFAULT );
	if( status != OCI_SUCCESS ) goto cleanup;

	status = OCIAttrSet( svchp, OCI_HTYPE_SVCCTX,
		authp, (ub4)0, OCI_ATTR_SESSION, errhp );
	if( status != OCI_SUCCESS ) goto cleanup;
	
	{
		// Get a list of tables (in the public schema)
		std::vector<std::string> tables;
		std::string dbcmd = std::string( "select table_name from user_tables" );	
		char tableName[60];
		
		status = OCIHandleAlloc( envhp, (dvoid **)&stmthp,
			OCI_HTYPE_STMT, (size_t)0, (dvoid **)0 );
		if( status != OCI_SUCCESS ) goto cleanup;
		
		status = OCIStmtPrepare( stmthp, errhp, 
			(text *)const_cast<char *>( dbcmd.c_str( ) ),
			(ub4)dbcmd.length( ), (ub4)OCI_NTV_SYNTAX, (ub4)OCI_DEFAULT );
		if( status != OCI_SUCCESS ) goto cleanup;

		status = OCIDefineByPos( stmthp, &defhp, errhp, 1, (dvoid *)&tableName,
			(sword)60, SQLT_STR, (dvoid *)0, (ub2 *)0, (ub2 *)0, OCI_DEFAULT );
		if( status != OCI_SUCCESS ) goto cleanup;

		status = OCIStmtExecute( svchp, stmthp, errhp, (ub4)1, (ub4)0,
			NULL, NULL, OCI_DEFAULT );
		if( status != OCI_SUCCESS && status != OCI_NO_DATA ) goto cleanup;
		
		while( status != OCI_NO_DATA ) {
			tables.push_back( tableName );
			status = OCIStmtFetch( stmthp, errhp, 1, 0, 0 );
		}
  
		if( stmthp ) (void)OCIHandleFree( stmthp, OCI_HTYPE_STMT );
		
		// Drop the tables
		for ( std::vector< std::string >::const_iterator it = tables.begin();
								it != tables.end(); it++ )	{
			std::string query = "DROP TABLE " + *it;
			
			MOD_LOG_TRACE << "Deleting test table " << query;

			status = OCIHandleAlloc( envhp, (dvoid **)&stmthp,
				OCI_HTYPE_STMT, (size_t)0, (dvoid **)0 );
			if( status != OCI_SUCCESS ) goto cleanup;
			
			status = OCIStmtPrepare( stmthp, errhp, 
				(text *)const_cast<char *>( query.c_str( ) ),
				(ub4)query.length( ), (ub4)OCI_NTV_SYNTAX, (ub4)OCI_DEFAULT );
			if( status != OCI_SUCCESS ) goto cleanup;

			status = OCIStmtExecute( svchp, stmthp, errhp, (ub4)1, (ub4)0,
				NULL, NULL, OCI_DEFAULT );
			if( status != OCI_SUCCESS && status != OCI_SUCCESS_WITH_INFO ) goto cleanup;

			if( stmthp ) (void)OCIHandleFree( stmthp, OCI_HTYPE_STMT );
		}
	}

	{
		// Get a list of sequences (in the public schema)
		std::vector<std::string> tables;
		std::string dbcmd = std::string( "select sequence_name from user_sequences" );	
		char tableName[60];
		
		status = OCIHandleAlloc( envhp, (dvoid **)&stmthp,
			OCI_HTYPE_STMT, (size_t)0, (dvoid **)0 );
		if( status != OCI_SUCCESS ) goto cleanup;
		
		status = OCIStmtPrepare( stmthp, errhp, 
			(text *)const_cast<char *>( dbcmd.c_str( ) ),
			(ub4)dbcmd.length( ), (ub4)OCI_NTV_SYNTAX, (ub4)OCI_DEFAULT );
		if( status != OCI_SUCCESS ) goto cleanup;

		status = OCIDefineByPos( stmthp, &defhp, errhp, 1, (dvoid *)&tableName,
			(sword)60, SQLT_STR, (dvoid *)0, (ub2 *)0, (ub2 *)0, OCI_DEFAULT );
		if( status != OCI_SUCCESS ) goto cleanup;

		status = OCIStmtExecute( svchp, stmthp, errhp, (ub4)1, (ub4)0,
			NULL, NULL, OCI_DEFAULT );
		if( status != OCI_SUCCESS && status != OCI_NO_DATA ) goto cleanup;
		
		while( status != OCI_NO_DATA ) {
			tables.push_back( tableName );
			status = OCIStmtFetch( stmthp, errhp, 1, 0, 0 );
		}
  
		if( stmthp ) (void)OCIHandleFree( stmthp, OCI_HTYPE_STMT );
		
		// Drop sequences
		for ( std::vector< std::string >::const_iterator it = tables.begin();
								it != tables.end(); it++ )	{
			std::string query = "DROP SEQUENCE " + *it;
			
			MOD_LOG_TRACE << "Deleting test sequence " << query;

			status = OCIHandleAlloc( envhp, (dvoid **)&stmthp,
				OCI_HTYPE_STMT, (size_t)0, (dvoid **)0 );
			if( status != OCI_SUCCESS ) goto cleanup;
			
			status = OCIStmtPrepare( stmthp, errhp, 
				(text *)const_cast<char *>( query.c_str( ) ),
				(ub4)query.length( ), (ub4)OCI_NTV_SYNTAX, (ub4)OCI_DEFAULT );
			if( status != OCI_SUCCESS ) goto cleanup;

			status = OCIStmtExecute( svchp, stmthp, errhp, (ub4)1, (ub4)0,
				NULL, NULL, OCI_DEFAULT );
			if( status != OCI_SUCCESS && status != OCI_SUCCESS_WITH_INFO ) goto cleanup;

			if( stmthp ) (void)OCIHandleFree( stmthp, OCI_HTYPE_STMT );
		}
	}

	{
		// Now create the test database
		std::string dbsource;
		std::string line;
		std::ifstream sinputfile( inputfile.c_str( ) );
		while( std::getline( sinputfile, line ) ) {
			if( boost::starts_with( line, "--" ) ) continue;
			dbsource.append( line );
			dbsource.append( "\n" );
		}
		std::vector<std::string> dbcmds;
		utils::splitString( dbcmds, dbsource, ";" );
		std::vector<std::string>::const_iterator end = dbcmds.end( );
		bool collect_trigger_code = false;
		std::string dbcmd;
		std::string nextCmd;
		std::vector<std::string>::const_iterator it = dbcmds.begin( );
		while( it != end ) {

			if( !nextCmd.empty( ) ) {
				dbcmd = nextCmd;
				nextCmd.clear( );
			} else {
				std::string part = boost::algorithm::trim_copy( *it );
				it++;

				size_t pos = part.find( "\n" );
				while( pos != std::string::npos ) {
					part.replace( pos, 1, " " );
					pos = part.find( "\n", pos+1 );
				}

				if( boost::starts_with( part, "CREATE TRIGGER"  ) ) {
					collect_trigger_code = true;
				}
				if( collect_trigger_code ) {
					if( boost::starts_with( part, "/" ) ) {
						collect_trigger_code = false;
						if( part.length( ) > 2 ) {
							nextCmd = part.substr( 2 );
						}
					} else {
						dbcmd.append( part );
						dbcmd.append( "; " );
						continue;
					}
				} else {
					dbcmd = part;
				}				
			}
			
			if( dbcmd.empty( ) ) continue;
			
			MOD_LOG_TRACE << "Creating database SQL statement: " << dbcmd;
			
			status = OCIHandleAlloc( envhp, (dvoid **)&stmthp,
				OCI_HTYPE_STMT, (size_t)0, (dvoid **)0 );
			if( status != OCI_SUCCESS ) goto cleanup;
			
			status = OCIStmtPrepare( stmthp, errhp, 
				(text *)const_cast<char *>( dbcmd.c_str( ) ),
				(ub4)dbcmd.length( ), (ub4)OCI_NTV_SYNTAX, (ub4)OCI_DEFAULT );
			if( status != OCI_SUCCESS ) goto cleanup;

			status = OCIStmtExecute( svchp, stmthp, errhp, (ub4)1, (ub4)0,
				NULL, NULL, OCI_DEFAULT );
			if( status != OCI_SUCCESS && status != OCI_SUCCESS_WITH_INFO ) goto cleanup;

			if( stmthp ) (void)OCIHandleFree( stmthp, OCI_HTYPE_STMT );
			
			dbcmd.clear( );
		}
	}
	
cleanup:
	std::string errmsg;
	if( status != OCI_SUCCESS ) {
		std::ostringstream os;
		os << "Setting up Oracle test database failed: " << getErrorMsg( status, errhp );
		errmsg = os.str( );
	}
	
	if( srvhp && errhp && authp ) (void)OCISessionEnd( svchp, errhp, authp, OCI_DEFAULT );
	if( srvhp && errhp ) (void)OCIServerDetach( srvhp, errhp, OCI_DEFAULT );
	if( stmthp ) (void)OCIHandleFree( stmthp, OCI_HTYPE_STMT );
	if( authp ) (void)OCIHandleFree( authp, OCI_HTYPE_SESSION );
	if( svchp ) (void)OCIHandleFree( svchp, OCI_HTYPE_SVCCTX );
	if( srvhp ) (void)OCIHandleFree( srvhp, OCI_HTYPE_SERVER );
	if( envhp ) (void)OCIHandleFree( envhp, OCI_HTYPE_ENV );
	if( errhp ) (void)OCIHandleFree( errhp, OCI_HTYPE_ERROR );

	if( status != OCI_SUCCESS ) {
		throw std::runtime_error( errmsg );
	}
}

static void dumpDatabase_( const std::string& host, unsigned short port,
			  const std::string& user, const std::string& password,
			  const std::string& dbName, const std::string& outputfile )
{
	FILE *fh = fopen( outputfile.c_str(), "w" );
	if ( fh == NULL )
		throw std::runtime_error( std::string( "failed to open file for database dump (" )
					  + boost::lexical_cast<std::string>( errno ) + "), file '"
					  + outputfile + "'");
	boost::shared_ptr<FILE> file_closer( fh, fclose);

	std::string connStr = buildConnStr( host, port, user, password, dbName );
	
	OCIEnv *envhp = 0;
	OCIError *errhp = 0;
	OCIServer *srvhp = 0;
	OCISvcCtx *svchp = 0;
	OCISession *authp = 0;
	OCIStmt *stmthp = 0;
	OCIDefine *defhp = 0;
	
	sword status;

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
		(dvoid *)const_cast<char *>( user.c_str( ) ), (ub4)user.length( ),
		OCI_ATTR_USERNAME, errhp );
	if( status != OCI_SUCCESS ) goto cleanup;

	status = OCIAttrSet( authp, OCI_HTYPE_SESSION,
		(dvoid *)const_cast<char *>( password.c_str( ) ), (ub4)password.length( ),
		OCI_ATTR_PASSWORD, errhp );
	if( status != OCI_SUCCESS ) goto cleanup;

	status = OCISessionBegin( svchp, errhp, authp,
		OCI_CRED_RDBMS, OCI_DEFAULT );
	if( status != OCI_SUCCESS ) goto cleanup;

	status = OCIAttrSet( svchp, OCI_HTYPE_SVCCTX,
		authp, (ub4)0, OCI_ATTR_SESSION, errhp );
	if( status != OCI_SUCCESS ) goto cleanup;

	{
		// Get a list of tables (in the public schema)
		std::vector<std::string> tables;
		std::string dbcmd = std::string( "select table_name from user_tables" );	
		char tableName[60];
		
		status = OCIHandleAlloc( envhp, (dvoid **)&stmthp,
			OCI_HTYPE_STMT, (size_t)0, (dvoid **)0 );
		if( status != OCI_SUCCESS ) goto cleanup;
		
		status = OCIStmtPrepare( stmthp, errhp, 
			(text *)const_cast<char *>( dbcmd.c_str( ) ),
			(ub4)dbcmd.length( ), (ub4)OCI_NTV_SYNTAX, (ub4)OCI_DEFAULT );
		if( status != OCI_SUCCESS ) goto cleanup;

		status = OCIDefineByPos( stmthp, &defhp, errhp, 1, (dvoid *)&tableName,
			(sword)60, SQLT_STR, (dvoid *)0, (ub2 *)0, (ub2 *)0, OCI_DEFAULT );
		if( status != OCI_SUCCESS ) goto cleanup;

		status = OCIStmtExecute( svchp, stmthp, errhp, (ub4)1, (ub4)0,
			NULL, NULL, OCI_DEFAULT );
		if( status != OCI_SUCCESS && status != OCI_NO_DATA ) goto cleanup;
		
		while( status != OCI_NO_DATA ) {
			tables.push_back( tableName );
			status = OCIStmtFetch( stmthp, errhp, 1, 0, 0 );
		}
  
		if( stmthp ) (void)OCIHandleFree( stmthp, OCI_HTYPE_STMT );
		
		// dump the tables
		std::sort( tables.begin(), tables.end());
		for ( std::vector< std::string >::const_iterator it = tables.begin();
								it != tables.end(); it++ )	{
			// dump the name of the table
			fprintf( fh, "%s:\n", it->c_str() );

			// dump metadata (currently column name) of the table

			OCIDescribe *descrhp;

			status = OCIHandleAlloc( envhp, (dvoid **)&descrhp,
				OCI_HTYPE_DESCRIBE, (size_t)0, (dvoid **)0 );
			if( status != OCI_SUCCESS ) goto cleanup;

			status = OCIDescribeAny( svchp, errhp, (dvoid *)const_cast<char *>( it->c_str( ) ),
				(ub4)it->length( ), (ub1)OCI_OTYPE_NAME,
				(ub1)OCI_DEFAULT, (ub1)OCI_PTYPE_TABLE,
				descrhp );
			if( status != OCI_SUCCESS ) goto cleanup;

			OCIParam *parmh = 0;
			status = OCIAttrGet( (dvoid *)descrhp, (ub4)OCI_HTYPE_DESCRIBE,
				(dvoid *)&parmh, (ub4 *)0, (ub4)OCI_ATTR_PARAM, errhp );
			if( status != OCI_SUCCESS ) goto cleanup;

			ub2 numCols = 0;
			status = OCIAttrGet( (dvoid *)parmh, (ub4)OCI_DTYPE_PARAM,
				(dvoid *)&numCols, (ub4 *)0, (ub4)OCI_ATTR_NUM_COLS, errhp );
			if( status != OCI_SUCCESS ) goto cleanup;

			OCIParam *collsthd = 0;
			status = OCIAttrGet( (dvoid *)parmh, (ub4)OCI_DTYPE_PARAM,
				(dvoid *)&collsthd, (ub4 *)0, (ub4)OCI_ATTR_LIST_COLUMNS, errhp );
			if( status != OCI_SUCCESS ) goto cleanup;

			std::string orderby;

			for( ub4 i = 1; i <= numCols; i++ ) {
				OCIParam *colhd;
				status = OCIParamGet( (dvoid *)collsthd, (ub4)OCI_DTYPE_PARAM,
					errhp, (dvoid **)&colhd, i );
				if( status != OCI_SUCCESS ) goto cleanup;

				ub4 colNameLen;
				char *colName;
				status = OCIAttrGet( (dvoid *)colhd, (ub4)OCI_DTYPE_PARAM,
					(dvoid *)&colName, (ub4 *)&colNameLen, (ub4)OCI_ATTR_NAME, errhp );
				if( status != OCI_SUCCESS ) goto cleanup;

				orderby.append( ( i > 1 ) ? ", ":" ORDER BY "); 
				orderby.append( "\"");
				orderby.append( colName, colNameLen);
				orderby.append( "\"");
				orderby.append( " ASC");

				std::string realColName = std::string( colName, colNameLen );
				fprintf( fh, ( i > 1 ) ? ", %s" : "%s", realColName.c_str( ) );
			} // foreach column

			fprintf( fh, "\n" );

			if( collsthd ) (void)OCIHandleFree( collsthd, OCI_HTYPE_DESCRIBE );

			if( descrhp ) (void)OCIHandleFree( descrhp, OCI_HTYPE_DESCRIBE );

			// the data
			dbcmd = "SELECT * from " + *it + orderby;

			status = OCIHandleAlloc( envhp, (dvoid **)&stmthp,
				OCI_HTYPE_STMT, (size_t)0, (dvoid **)0 );
			if( status != OCI_SUCCESS ) goto cleanup;
			
			status = OCIStmtPrepare( stmthp, errhp, 
				(text *)const_cast<char *>( dbcmd.c_str( ) ),
				(ub4)dbcmd.length( ), (ub4)OCI_NTV_SYNTAX, (ub4)OCI_DEFAULT );
			if( status != OCI_SUCCESS ) goto cleanup;

			char **data = (char **)calloc( sizeof( char *), numCols );
			ub2 *errcode = (ub2 *)calloc( sizeof( ub2 ), numCols );
			ub2 *ind = (ub2 *)calloc( sizeof( ub2 ), numCols );
			for( ub4 i = 1; i <= numCols; i++ ) {
				data[i-1] = (char *)calloc( sizeof( char ), 255 );
			}
			for( ub4 i = 1; i <= numCols; i++ ) {
				status = OCIDefineByPos( stmthp, &defhp, errhp, i, (dvoid *)data[i-1],
					(sword)255, SQLT_STR, (dvoid *)&ind[i-1], (ub2 *)0, (ub2 *)&errcode[i-1], OCI_DEFAULT );
				if( status != OCI_SUCCESS ) goto cleanup;
			}

			status = OCIStmtExecute( svchp, stmthp, errhp, (ub4)1, (ub4)0,
				NULL, NULL, OCI_DEFAULT );
			if( status != OCI_SUCCESS && status != OCI_NO_DATA ) goto cleanup;
			
			while( status != OCI_NO_DATA ) {
				for( ub4 i = 1; i <= numCols; i++ ) {
					if( errcode[i-1] == 1405 ) {
						fprintf( fh, ( i > 1 ) ? ", NULL" : "NULL" );
					} else {
						fprintf( fh, ( i > 1 ) ? ", '%s'" : "'%s'", data[i-1] );
					}
				}			
				fprintf( fh, "\n" );

				status = OCIStmtFetch( stmthp, errhp, 1, 0, 0 );
			}

			for( ub4 i = 1; i < numCols; i++ ) {
				(void)free( data[i-1] );
			}
			(void)free( data );
			(void)free( errcode );

			if( stmthp ) (void)OCIHandleFree( stmthp, OCI_HTYPE_STMT );

		} // foreach table
	} // end of scope
	
cleanup:
	std::string errmsg;
	if( status != OCI_SUCCESS && status != OCI_NO_DATA ) {
		std::ostringstream os;
		os << "Dumping Oracle test database failed: " << getErrorMsg( status, errhp );
		errmsg = os.str( );
	}

	if( srvhp && errhp && authp ) (void)OCISessionEnd( svchp, errhp, authp, OCI_DEFAULT );
	if( srvhp && errhp ) (void)OCIServerDetach( srvhp, errhp, OCI_DEFAULT );
	if( stmthp ) (void)OCIHandleFree( stmthp, OCI_HTYPE_STMT );
	if( authp ) (void)OCIHandleFree( authp, OCI_HTYPE_SESSION );
	if( svchp ) (void)OCIHandleFree( svchp, OCI_HTYPE_SVCCTX );
	if( srvhp ) (void)OCIHandleFree( srvhp, OCI_HTYPE_SERVER );
	if( envhp ) (void)OCIHandleFree( envhp, OCI_HTYPE_ENV );
	if( errhp ) (void)OCIHandleFree( errhp, OCI_HTYPE_ERROR );

	if( status != OCI_SUCCESS && status != OCI_NO_DATA ) {
		throw std::runtime_error( errmsg );
	}
}

void OracleTestConstructor::createTestDatabase( const OracleTestConfig& cfg )
{
	createTestDatabase_( cfg.host(), cfg.port(), cfg.user(), cfg.password(), cfg.dbName(), cfg.input_filename());
}

config::ConfigurationTree OracleTestConfig::extractMyNodes( const config::ConfigurationTree& pt )
{
	boost::property_tree::ptree rt;
	boost::property_tree::ptree::const_iterator pi = pt.begin(), pe = pt.end();

	for ( ; pi != pe; ++pi )	{
		if ( boost::algorithm::iequals( pi->first, "inputfile" ))
			m_input_filename = pi->second.data();
		else if ( boost::algorithm::iequals( pi->first, "dumpfile" ))
			m_dump_filename = pi->second.data();
		else
			rt.add_child( pi->first, pi->second );
	}
	return rt;
}

void OracleTestConfig::setMyCanonicalPathes( const std::string& referencePath )
{
	if ( !m_input_filename.empty() )
		m_input_filename = utils::getCanonicalPath( m_input_filename, referencePath );
	if (!m_dump_filename.empty())
		m_dump_filename = utils::getCanonicalPath( m_dump_filename, referencePath );
}

void OracleTestConfig::dump_database()
{
	dumpDatabase_( host(), port(), user(), password(), dbName(), m_dump_filename);
}


