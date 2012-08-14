// modified example, following the MSXML documentation on MSDN

#include <tchar.h>
#include <AtlConv.h>
#include <strsafe.h>
#import <msxml6.dll>
#include <iostream>

namespace
{
	bool CreateAndInitDom( MSXML2::IXMLDOMDocument **xmlDom )
	{
		HRESULT h;
		
		h = CoCreateInstance( __uuidof( MSXML2::DOMDocument60 ), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS( xmlDom ) );
		if( FAILED( h ) ) {
			std::cerr << "Error creating and initializing DOM, error is " << h << std::endl;
			return false;
		}
	
		// these methods should not fail so don't inspect result
		(*xmlDom)->put_async( VARIANT_FALSE );  
		(*xmlDom)->put_validateOnParse( VARIANT_FALSE );
		(*xmlDom)->put_resolveExternals( VARIANT_FALSE );
		
		return true;
	}

	bool LoadXmlFile( MSXML2::IXMLDOMDocument *xmlDom, LPCSTR filename )
	{
		HRESULT h;
		
		USES_CONVERSION;

		// prepare parameters for load function
		VARIANT varFileName;
		VariantInit( &varFileName );
		BSTR bstr = SysAllocString( A2OLE( filename ) );
		if( bstr == NULL ) {
			std::cerr << "Error allocating variant string for filename, out of memory" << std::endl;
			return false;
		}
		V_VT( &varFileName ) = VT_BSTR;
		V_BSTR( &varFileName ) = bstr;
	    
		// load the file
		VARIANT_BOOL res = xmlDom->load( varFileName );
		if( res != VARIANT_TRUE ) {
			// Failed to load xml, get last parsing error
			MSXML2::IXMLDOMParseError *xmlErr = NULL;			
			BSTR err = NULL;
			xmlDom->get_parseError( &xmlErr );
			xmlErr->get_reason( &err );
			std::cerr << "Error opening XML file: " << OLE2A( err ) << std::endl;
			SysFreeString( err );
			VariantClear( &varFileName );
			return false;
		}
		
		return true;
	}

	bool example1Func( const _TCHAR *filename )
	{
		HRESULT h;
		MSXML2::IXMLDOMDocument *xmlDom = NULL;
    
		if( !CreateAndInitDom( &xmlDom ) ) {
			return false;
		}

		// load XML file
		if( !LoadXmlFile( xmlDom, filename ) ) {
			xmlDom->Release( );
			return false;
		}
				
		BSTR xml = NULL;
		xmlDom->get_xml( &xml );
		//puts( OLE2A( xml ) );
		
		// cleanup
		SysFreeString( xml );
		xmlDom->Release( );
		
		return true;
	}
}

int _tmain( int argc, _TCHAR* argv[] )
{
	if( argc != 2 ) {
		return 1;
	}

	HRESULT h = CoInitializeEx( NULL, COINIT_MULTITHREADED );
	if( FAILED( h ) ) {
		std::cerr << "Unable to initialize COM, error is: " << h << std::endl;
		return 1;
	}
		
	bool res = example1Func( argv[1] );

	if( res ) {
		std::cout << "msxml test OK" << std::endl;
	} else {
		std::cout << "msxml test ERROR" << std::endl;
	}

        CoUninitialize( );
	
	return res;
}
