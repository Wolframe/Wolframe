#include "common.h"

LPCWSTR vcsSetPropertyFromXmlQuery = L"SELECT `SetPropertyFromXml`, `Name`, `File`, `Path`, `Value` FROM `SetPropertyFromXml`";
enum eSetPropertyFromXmlQuery { rfqId = 1, rfqName, rfqFile, rfqPath, rfqValue };

static HRESULT CheckXmlfile( __in_z LPCWSTR wzFile );
static HRESULT CheckXPath( __in_z LPCWSTR wzPath );

extern "C" UINT WINAPI SetPropertyFromXml( __in MSIHANDLE hInstall )
{
	HRESULT hr = S_OK;
	PMSIHANDLE hView;
	PMSIHANDLE hRec;
	LPWSTR sczId = NULL;
	LPWSTR sczName = NULL;
	LPWSTR sczFile = NULL;
	LPWSTR sczPath = NULL;
	LPWSTR sczValue = NULL;

	hr = WcaInitialize( hInstall, "SetPropertyFromXml" );
	ExitOnFailure( hr, "Failed to initialize SetPropertyFromXml." );
	
	WcaLog( LOGMSG_STANDARD, "Initializing SetPropertyFromXml schedule." );

	// anything to do?
	if( WcaTableExists( L"SetPropertyFromXml") != S_OK ) {
		WcaLog( LOGMSG_STANDARD, "SetPropertyFromXml table doesn't exist, so there are no properties to be set from XML." );
		ExitFunction( );
	}

	// query and loop through all SetPropertyFromXml entries
	hr = WcaOpenExecuteView( vcsSetPropertyFromXmlQuery, &hView );
	ExitOnFailure( hr, "Failed to open view on SetPropertyFromXml table" );

	while( ( hr = WcaFetchRecord( hView, &hRec ) ) == S_OK ) {
		hr = WcaGetRecordString( hRec, rfqId, &sczId );
		ExitOnFailure( hr, "Failed to get SetXMLProperty identity." );

		hr = WcaGetRecordString( hRec, rfqName, &sczName );
		ExitOnFailure( hr, "Failed to get property name for SetXMLProperty." );

		hr = WcaGetRecordString( hRec, rfqFile, &sczFile );
		ExitOnFailure( hr, "Failed to get file for SetXMLProperty." );

		hr = WcaGetRecordString( hRec, rfqPath, &sczPath );
		ExitOnFailure( hr, "Failed to get path for SetXMLProperty." );

		hr = WcaGetRecordString( hRec, rfqValue, &sczValue );
		ExitOnFailure( hr, "Failed to get value for SetXMLProperty." );

		// check if XML file exists
		hr = CheckXmlfile( sczFile );
 		ExitOnFailure2( hr, "Failed reading from XML file: %S for row: %S", sczFile, sczId );

		// check if the XPath expression is correct
		hr = CheckXPath( sczPath );
 		ExitOnFailure2( hr, "Error in XPath expression: %S for row: %S", sczPath, sczId );
	}

	// transform no more item "error" into a success
	if( hr == E_NOMOREITEMS ) {
		hr = S_OK;
	}
	
	ExitOnFailure( hr, "Failure occured while processing SetXMLProperty table" );

LExit:
	ReleaseStr( sczValue );
	ReleaseStr( sczPath );
	ReleaseStr( sczFile );
	ReleaseStr( sczName );
	ReleaseStr( sczId );
	
	DWORD er = SUCCEEDED( hr ) ? ERROR_SUCCESS : ERROR_INSTALL_FAILURE;	
	return WcaFinalize( er );
}

static HRESULT CheckXmlfile( __in_z LPCWSTR wzFile )
{
	HRESULT hr = S_OK;
	return hr;
}

static HRESULT CheckXPath( __in_z LPCWSTR wzPath )
{
	HRESULT hr = S_OK;
	return hr;
}
