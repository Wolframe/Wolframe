// modified example, original from the Windows SDK Code Samples

#include <tchar.h>
#include <atlbase.h>
#include <AtlConv.h>
#include "xmllite.h"
#include <strsafe.h>
#include <iostream>

namespace
{
	bool example1Func( const _TCHAR *filename )
	{
		HRESULT h;
		CComPtr<IStream> fileStream;
		CComPtr<IXmlReader> reader;
		XmlNodeType nodeType;
		const WCHAR *value;
		
		USES_CONVERSION;
		
		h = SHCreateStreamOnFileEx( A2CW( filename ), STGM_READ, OPEN_EXISTING, false, NULL, &fileStream );
		if( FAILED( h ) ) {
			std::cerr << "Error creating file reader, error is " << h << std::endl;
			return false;
		}

		h = CreateXmlReader( __uuidof( IXmlReader ), (void **)&reader, NULL );
		if( FAILED( h ) ) {
			std::cerr << "Error creating xml reader, error is " << h << std::endl;
			return false;
		}
		
		h = reader->SetInput( fileStream );
		if( FAILED( h ) ) {
			std::cerr << "Error setting input for reader, error is " << h << std::endl;
			return false;
		}
		
		h = reader->Read( &nodeType );
		while( h == S_OK ) {
			switch( nodeType ) {
				case XmlNodeType_Element:
					h = reader->GetLocalName( &value, NULL );
					if( FAILED( h ) ) {
						std::cerr << "Error getting name of element, error is " << h << std::endl;
						return false;
					}
//					std::cout << "tag: " << value << std::endl;
					break;
					
				case XmlNodeType_CDATA:
					h = reader->GetValue( &value, NULL );
					if( FAILED( h ) ) {
						std::cerr << "Error getting value of CDATA, error is " << h << std::endl;
						return false;
					}
//					std::cout << "data: " << value << std::endl;
					break;
			}
			
			h = reader->Read( &nodeType );
		}
		
		return true;
	}
}

int _tmain( int argc, _TCHAR* argv[] )
{
	if( argc != 2 ) {
		return 1;
	}
	
	bool res = example1Func( argv[1] );

	if( res ) {
		std::cout << "xmllite test OK" << std::endl;
	} else {
		std::cout << "xmllite test ERROR" << std::endl;
	}
	
	return res;
}
