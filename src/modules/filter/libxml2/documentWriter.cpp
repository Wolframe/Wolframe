#include "documentWriter.hpp"

using namespace _Wolframe;
using namespace _Wolframe::langbind;

bool DocumentWriter::init( const char* encoding, bool standalone)
{
	xmlBufferPtr bb = xmlBufferCreate();
	if (!bb) return false;

	m_writerbuf = boost::shared_ptr<xmlBuffer>( bb, xmlBufferFree);
	xmlTextWriterPtr ww = xmlNewTextWriterMemory( bb, 0);
	if (!ww) return false;

	m_writer = boost::shared_ptr<xmlTextWriter>( ww, xmlFreeTextWriter);
	if (0>xmlTextWriterStartDocument( ww, "1.0", encoding, (standalone?"yes":"no")))
	{
		m_writer.reset();
		m_writerbuf.reset();
		return false;
	}
	return true;
}

DocumentWriter::DocumentWriter( const char* encoding, const char* doctype, const char* publicid, const char* systemid)
{
	if (init( encoding, false))
	{
		xmlTextWriterPtr ww = m_writer.get();
		if (0>xmlTextWriterStartDTD( ww, (const xmlChar*)doctype, (const xmlChar*)publicid, (const xmlChar*)systemid)
		||  0>xmlTextWriterEndDTD(ww))
		{
			m_writer.reset();
			m_writerbuf.reset();
		}
	}
}
