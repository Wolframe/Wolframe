// modified example, original from http://xmlsoft.org/examples/parse1.c

#include <iostream>
#include <string>

#include <libxml/parser.h>
#include <libxml/tree.h>   

namespace {

std::string repeat( const std::string &s, int times )
{
	std::string res;
	
	res.reserve( times * s.length( ) );
	for( int i = 0; i < times; i++ ) {
		res += s;
	}
	
	return res;
}

} // anon namspace

void print_structure( xmlNode *node, const int depth )
{
	for( xmlNode *cur = node; cur != 0; cur = cur->next ) {
		if( cur->type == XML_ELEMENT_NODE ) {
			std::cout << repeat( " ", depth ) <<
				"<" << cur->name << ">" << std::endl;
		}
		
		if( cur->type == XML_TEXT_NODE ) {
			std::cout << repeat( " " , depth ) <<
				cur->content << std::endl;
		}
		
		print_structure( cur->children, depth+1 );
		
		if( cur->type == XML_ELEMENT_NODE ) {
			std::cout << repeat( " ", depth ) <<
				"</" << cur->name << ">" << std::endl;
		}
	}
}

int main( int argc, char** argv )
{
	LIBXML_TEST_VERSION
	
	if( argc != 2 ) {
		return 1;
	}

	xmlDocPtr doc;

	doc = xmlReadFile( argv[1], NULL, 0 );
	xmlNodePtr root;
	
	root = xmlDocGetRootElement( doc );
	if( root == NULL ) {
		std::cerr << "ERROR: empty document" << std::endl;
		return 1;
	}
	
	if( xmlStrcmp( root->name, (const xmlChar *)"list" ) ) {
		std::cerr << "ERROR: expected 'list' root node" << std::endl;
		return 1;
	}
	
	print_structure( root, 1 );
	
	xmlFreeDoc( doc );

	xmlCleanupParser( );

	return 0;
}
