#include "outputfilterImpl.hpp"

using namespace _Wolframe;
using namespace _Wolframe::langbind;

bool OutputFilterImpl::flushBuffer()
{
	bool rt = true;
	// if we have the whole document, then we start to print it and return an error, as long as we still have data:
	if (m_elemitr < m_elembuf.size())
	{
		m_elemitr += write( m_elembuf.c_str() + m_elemitr, m_elembuf.size() - m_elemitr);
		if (m_elemitr == m_elembuf.size())
		{
			setState( OutputFilter::Open);
			rt = true;
		}
		else
		{
			setState( OutputFilter::EndOfBuffer);
			rt = false;
		}
	}
	else
	{
		setState( OutputFilter::Open);
	}
	return rt;
}

bool OutputFilterImpl::print( ElementType type, const void* element, std::size_t elementsize)
{
	bool rt = true;
	xmlTextWriterPtr xmlout = m_doc.get();

	if (!xmlout)
	{
		const char* ec = m_encoding.get()?m_encoding.get()->c_str():"UTF-8";
		if (m_doctype_root.size())
		{
			m_doc = DocumentWriter( ec, m_doctype_root.c_str(), m_doctype_public.size()?m_doctype_public.c_str():0, m_doctype_system.size()?m_doctype_system.c_str():0);
		}
		else
		{
			m_doc = DocumentWriter( ec);
		}
		xmlout = m_doc.get();
		if (!xmlout)
		{
			setState( Error, "libxml2 filter: writer creation failed");
			return false;
		}
	}
	if (m_taglevel == 0 && m_nofroot == 1)
	{
		return flushBuffer();
	}
	switch (type)
	{
		case OutputFilter::OpenTag:
			m_attribname.clear();
			if (m_taglevel == 0)
			{
				if (m_nofroot > 0)
				{
					setState( Error, "libxml2 filter: multi root element error");
					rt = false;
				}
				else
				{
					m_nofroot += 1;
				}
			}
			if (0>xmlTextWriterStartElement( xmlout, getElement( element, elementsize)))
			{
				setState( Error, "libxml2 filter: write start element error");
				rt = false;
			}
			m_taglevel += 1;
			break;

		case OutputFilter::Attribute:
			if (m_attribname.size())
			{
				setState( Error, "libxml2 filter: illegal operation");
				rt = false;
			}
			m_attribname.clear();
			m_attribname.append( (const char*)element, elementsize);
			break;

		case OutputFilter::Value:
			if (m_attribname.empty())
			{
				if (0>xmlTextWriterWriteString( xmlout, getElement( element, elementsize)))
				{
					setState( Error, "libxml2 filter: write value error");
					rt = false;
				}
			}
			else if (0>xmlTextWriterWriteAttribute( xmlout, getXmlString(m_attribname), getElement( element, elementsize)))
			{
				setState( Error, "libxml2 filter: write attribute error");
				rt = false;
			}
			else
			{
				m_attribname.clear();
			}
			break;

		case OutputFilter::CloseTag:
			if (0>xmlTextWriterEndElement( xmlout))
			{
				setState( Error, "libxml2 filter: write close tag error");
				rt = false;
			}
			else if (m_taglevel == 1)
			{
				if (0>xmlTextWriterEndDocument( xmlout))
				{
					setState( Error, "libxml2 filter: write end document error");
					rt = false;
				}
				else
				{
					if (m_xsltMapper.defined())
					{
						m_elembuf = m_xsltMapper.apply( m_doc.getContent());
					}
					else
					{
						m_elembuf = m_doc.getContent();
					}
					m_elemitr = 0;
					m_taglevel = 0;
					return flushBuffer();
				}
			}
			m_taglevel -= 1;
			m_attribname.clear();
			break;

		default:
			setState( Error, "libxml2 filter: illegal state");
			rt = false;
	}
	return rt;
}

void OutputFilterImpl::setDocType( const std::string& value)
{
	types::DocType doctype( value);
	if (doctype.rootid)
	{
		m_doctype_root = doctype.rootid;
		if (doctype.publicid) m_doctype_public = doctype.publicid;
		if (doctype.systemid) m_doctype_system = doctype.systemid;
	}
}

