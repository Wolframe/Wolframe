#include <libxml/parser.h>
#include <libxml/tree.h>   

namespace
{
	void example1Func( const char *filename )
	{
		xmlDocPtr doc;

		doc = xmlReadFile( filename, NULL, 0 );
		xmlFreeDoc( doc );
	}
}

int main( int argc, char** argv )
{
	LIBXML_TEST_VERSION

	if( argc != 2 ) {
		return 1;
	}

	example1Func( argv[1] );

	xmlCleanupParser( );

	return 0;
}
