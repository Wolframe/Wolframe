// modified example, following the MSXML documentation on MSDN

#include <tchar.h>
#include <AtlConv.h>
#include <strsafe.h>
#import <msxml6.dll>
#include <iostream>

namespace
{
	bool example1Func( const _TCHAR *filename )
	{
		HRESULT h;
		MSXML2::IXMLDOMDocument *xmlDom = NULL;
    
		h = CoCreateInstance( __uuidof( MSXML2::DOMDocument60 ), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS( &xmlDom ) );
		if( FAILED( h ) ) {
			std::cerr << "Error creating and initializing DOM, error is " << h << std::endl;
			return false;
		}
		
		// these methods should not fail so don't inspect result
		xmlDom->put_async( VARIANT_FALSE );  
		xmlDom->put_validateOnParse( VARIANT_FALSE );
		xmlDom->put_resolveExternals( VARIANT_FALSE );
		
		// prepare parameters for load function
		VARIANT varFileName;

		USES_CONVERSION;

		VariantInit( &varFileName );
		BSTR bstr = SysAllocString( A2OLE( filename ) );
		if( FAILED( bstr ) ) {
			xmlDom->Release( );
			std::cerr << "Error allocating variant string for filename, error is " << h << std::endl;
			return false;
		}
		V_VT( &varFileName ) = VT_BSTR;
		V_BSTR( &varFileName ) = bstr;
		    
		// load XML file
		VARIANT_BOOL res = xmlDom->load( varFileName );
		if( res != VARIANT_TRUE ) {
			// Failed to load xml, get last parsing error
			//xmlDom->get_parseError(&pXMLErr));
			//CHK_HR(pXMLErr->get_reason(&bstrErr));
			//printf("Failed to load DOM from stocks.xml. %S\n", bstrErr);		}
			std::cerr << "Error opening XML file" << std::endl;
			//SAFE_RELEASE(pXMLErr);
			VariantClear( &varFileName );
			xmlDom->Release( );
			return false;
		}
		
		BSTR xml = NULL;
		xmlDom->get_xml( &xml );
		//puts( OLE2A( xml ) );
		
		// cleanup
		SysFreeString( xml );
		VariantClear( &varFileName );
		xmlDom->Release( );
		
		return true;
	}
}

int main( int argc, _TCHAR* argv[] )
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
