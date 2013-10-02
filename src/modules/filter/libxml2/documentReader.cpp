#include "documentReader.hpp"
#include <stdexcept>

using namespace _Wolframe;
using namespace _Wolframe::langbind;

DocumentReader::DocumentReader( const char* content, std::size_t contentsize)
{
	int options = XML_PARSE_NOENT | XML_PARSE_COMPACT | XML_PARSE_NONET | XML_PARSE_NODICT;
	xmlDocPtr pp = xmlReadMemory( content, contentsize, "noname.xml", NULL, options);
	if (pp)
	{
		m_ptr = boost::shared_ptr<xmlDoc>( pp, xmlFreeDoc);
	}
}

DocumentReader::DocumentReader( xmlDocPtr doc)
	:m_ptr(doc){}


std::string DocumentReader::getContent() const
{
	xmlChar* mem;
	int memsize;
	xmlDocDumpMemory( m_ptr.get(), &mem, &memsize);
	if (!mem)
	{
		xmlError* err = xmlGetLastError();
		throw std::runtime_error( "failed to dump XML document content");
	}
	boost::shared_ptr<xmlChar> contentref( mem, xmlFree);
	return std::string( (char*)mem, memsize);
}

