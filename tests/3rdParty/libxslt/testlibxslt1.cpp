#include <iostream>

#include <libxslt/xslt.h>
#include <libxslt/xsltInternals.h>
#include <libxslt/transform.h>
#include <libxslt/xsltutils.h>

int main( int argc, char *argv[] )
{
	if( argc != 3 ) {
		std::cerr << "usage: testlibxslt1 <XSLT file> <XML file>" << std::endl;
		return 1;
	}

	LIBXML_TEST_VERSION
	
	xsltStylesheetPtr script = xsltParseStylesheetFile( ( const xmlChar *)argv[1] );
	xmlDocPtr doc = xmlParseFile( argv[2] );
	const char *params[1] = { NULL };
	xmlDocPtr res = xsltApplyStylesheet( script, doc, params );

	xmlChar *resTxt;
	int resLen;
	xsltSaveResultToString( &resTxt, &resLen, res, script );
	std::cout << resTxt;

	xmlFreeDoc( res );
	xmlFreeDoc( doc );
	xsltFreeStylesheet( script );

	xsltCleanupGlobals( );
	xmlCleanupParser( );

	return 0;
}
