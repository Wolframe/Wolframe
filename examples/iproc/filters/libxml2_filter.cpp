#ifndef WITH_LIBXML2
#error Compiling a libxml2 module without libxml2 support enabled
#endif
#include "libxml2_filter.hpp"
#include "bufferingFilterBase.hpp"
#include "textwolf.hpp"
#include "protocol/streamIterator.hpp"
#include <cstddef>
#include <cstring>
#include <vector>
#include <string>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

using namespace _Wolframe;
using namespace filter;

namespace {

struct LibXml2Init
{
	LibXml2Init()
	{
		LIBXML_TEST_VERSION;
	}

	~LibXml2Init()
	{
		xmlCleanupParser();
	}
};
static LibXml2Init libXml2Init;

class Content
{
public:
	Content( const CountedReference<std::string>& e, bool withEmpty=true) :m_doc(0),m_node(0),m_value(0),m_prop(0),m_propvalues(0),m_encoding(e),m_withEmpty(withEmpty){}

	bool end() const
	{
		return (m_doc == 0);
	}

	///\brief Get the last error, if the filter got into an error state
	///\return the last error as string or 0
	const char* getLastError() const
	{
		return m_error.size()?m_error.c_str():0;
	}

	bool open( const void* content, std::size_t size)
	{
		if (m_doc) xmlFreeDoc( m_doc);
		m_nodestk.clear();

		int options = XML_PARSE_NOENT | XML_PARSE_DTDLOAD;
		m_doc = xmlReadMemory( (const char*)content, size, "noname.xml", NULL, options);
		if (!m_doc)
		{
			xmlError* err = xmlGetLastError();
			if (err) m_error.append( err->message);
			return false;
		}
		m_node = xmlDocGetRootElement( m_doc);
		std::string enc;
		const xmlChar* ec = m_doc->encoding;
		for (int ii=0; ec[ii]!=0; ii++)
		{
			enc.push_back((unsigned char)ec[ii]);
		}
		m_encoding.reset( new std::string(enc));
		return true;
	}

	~Content()
	{
		if (m_doc) xmlFreeDoc( m_doc);
	}

	bool fetch( protocol::InputFilter::ElementType* type, void* buffer, std::size_t buffersize, std::size_t* bufferpos)
	{
		bool rt = true;
		if (!m_doc)
		{
			rt = false;
		}
		else if (m_value)
		{
			*type = protocol::InputFilter::Value;
			rt = getElement( buffer, buffersize, bufferpos, m_value);
			if (rt) m_value = 0;
		}
		else if (m_prop && m_propvalues)
		{
			*type = protocol::InputFilter::Attribute;
			rt = getElement( buffer, buffersize, bufferpos, m_prop->name);
			m_value = m_propvalues->content;
			m_propvalues = m_propvalues->next;
			if (!m_propvalues)
			{
				m_prop = m_prop->next;
				if (m_prop) m_propvalues = m_prop->children;
			}
		}
		else if (!m_node)
		{
			if (m_nodestk.empty())
			{
				xmlFreeDoc( m_doc);
				m_doc = 0;
				rt = false;
			}
			else
			{
				m_node = m_nodestk.back();
				m_nodestk.pop_back();
				*type = protocol::InputFilter::CloseTag;
				rt = true;
			}
		}
		else switch (m_node->type)
		{
			case XML_HTML_DOCUMENT_NODE:
			case XML_DOCB_DOCUMENT_NODE:
			case XML_DOCUMENT_NODE:
			case XML_ELEMENT_NODE:
				*type = protocol::InputFilter::OpenTag;
				m_prop = m_node->properties;
				if (m_prop) m_propvalues = m_prop->children;
				m_nodestk.push_back( m_node->next);
				rt = getElement( buffer, buffersize, bufferpos, m_node->name);
				m_node = m_node->children;
				break;

			case XML_ATTRIBUTE_NODE:
				*type = protocol::InputFilter::Attribute;
				rt = getElement( buffer, buffersize, bufferpos, m_node->name);
				m_value = m_node->content;
				m_node = m_node->next;
				break;

			case XML_TEXT_NODE:
				*type = protocol::InputFilter::Value;
				if (!m_withEmpty)
				{
					std::size_t ii=0;
					while (m_node->content[ii] != 0 && m_node->content[ii]<=' ') ++ii;
					if (m_node->content[ii] == 0)
					{
						m_node = m_node->next;
						return fetch( type, buffer, buffersize, bufferpos);
					}
				}
				rt = getElement( buffer, buffersize, bufferpos, m_node->content);
				m_node = m_node->next;
				break;

			case XML_CDATA_SECTION_NODE:
			case XML_ENTITY_REF_NODE:
			case XML_ENTITY_NODE:
			case XML_PI_NODE:
			case XML_COMMENT_NODE:
			case XML_DOCUMENT_TYPE_NODE:
			case XML_DOCUMENT_FRAG_NODE:
			case XML_NOTATION_NODE:
			case XML_DTD_NODE:
			case XML_ELEMENT_DECL:
			case XML_ATTRIBUTE_DECL:
			case XML_ENTITY_DECL:
			case XML_NAMESPACE_DECL:
			case XML_XINCLUDE_START:
			case XML_XINCLUDE_END:
			default:
				return fetch( type, buffer, buffersize, bufferpos);
		}
		return rt;
	}

private:
	bool getElement( void* buffer, std::size_t buffersize, std::size_t* bufferpos, const xmlChar* str)
	{
		if (!str) return true;

		std::size_t elemenlen = xmlStrlen( str) * sizeof(*str);
		if (buffersize - *bufferpos < elemenlen)
		{
			return false;
		}
		std::memcpy( (char*)buffer + *bufferpos, str, elemenlen);
		*bufferpos += elemenlen;
		return true;
	}

private:
	friend class InputFilterImpl;
	xmlDocPtr m_doc;
	xmlNode* m_node;
	xmlChar* m_value;
	xmlAttr* m_prop;
	xmlNode* m_propvalues;
	std::vector<xmlNode*> m_nodestk;
	CountedReference<std::string> m_encoding;
	std::string m_error;
	bool m_withEmpty;
};

struct InputFilterImpl :public BufferingInputFilter<Content>
{
	typedef BufferingInputFilter<Content> Parent;

	InputFilterImpl( const CountedReference<std::string>& e, std::size_t bufsize)
		:BufferingInputFilter<Content>(new Content( e), bufsize){}

	InputFilterImpl( const InputFilterImpl& o) :BufferingInputFilter<Content>(o){}

	///\brief Get a member value of the filter
	///\param [in] name case sensitive name of the variable
	///\param [in] valbuf buffer for the value returned
	///\param [in] valbufsize size of the valbuf buffer in bytes
	///\return true on success, false, if the variable does not exist or the operation failed
	virtual bool getValue( const char* name, char* valbuf, std::size_t valbufsize)
	{
		Content* dc = content();
		if (std::strcmp( name, "empty") == 0)
		{
			if (valbufsize > 6) return false;
			std::strncpy( valbuf, dc->m_withEmpty?"true":"false", valbufsize);
			return true;
		}
		return Parent::getValue( name, valbuf, valbufsize);
	}

	///\brief Set a member value of the filter
	///\param [in] name case sensitive name of the variable
	///\param [in] value new value of the variable to set
	///\return true on success, false, if the variable does not exist or the operation failed
	virtual bool setValue( const char* name, const char* value)
	{
		Content* dc = content();
		if (std::strcmp( name, "empty") == 0)
		{
			if (std::strcmp( value, "true") == 0)
			{
				dc->m_withEmpty = true;
			}
			else if (std::strcmp( value, "false") == 0)
			{
				dc->m_withEmpty = false;
			}
			else
			{
				return false;
			}
		}
		return Parent::setValue( name, value);
	}
};


class FormatOutputImpl :public protocol::FormatOutput
{
public:
	enum Error
	{
		Ok,
		ErrIllegalOperation,
		ErrIllegalState,
		ErrOutOfMem,
		ErrCreateWriter,
		ErrCreateDocument,
		ErrLibXMLEndElement,
		ErrLibXMLAttribute,
		ErrLibXMLStartElement,
		ErrLibXMLMultiRootElement,
		ErrLibXMLText
	};

	static const char* errorName( Error e)
	{
		static const char* ar[] =
					{0
					,"illegal operation"
					,"illegal state"
					,"out of mem"
					,"create writer failed"
					,"create document failed",
					"XML end element error",
					"XML attribute error",
					"XML start element error",
					"XML multi root element",
					"XML text error"};
		return ar[ (int)e];
	}

	///\brief Get the last error, if the filter got into an error state
	///\return the last error as string or 0
	virtual const char* getLastError() const	{return errorName((Error)getError());}

	class Document
	{
	public:
		Document( const char* encoding)
			:m_nofroot(0)
			,m_taglevel(0)
			,m_error(Ok)
			,m_writerbuf(0)
			,m_writer(0)
			,m_valuebuf(0)
			,m_bufstate(protocol::EscapingBuffer<textwolf::StaticBuffer>::SRC)

		{
			m_writerbuf = xmlBufferCreate();
			if (!m_writerbuf)
			{
				m_error = ErrOutOfMem;
				return;
			}
			m_writer = xmlNewTextWriterMemory( m_writerbuf, 0);
			if (!m_writer)
			{
				m_error = ErrCreateWriter;
				return;
			}
			if (0>xmlTextWriterStartDocument( m_writer, "1.0", encoding, "yes"))
			{
				m_error = ErrCreateDocument;
				return;
			}
		}

		~Document()
		{
			if (m_valuebuf) xmlFree( m_valuebuf);
			if (m_writer) xmlFreeTextWriter( m_writer);
			if (m_writerbuf) xmlBufferFree( m_writerbuf);
		}

		bool print( ElementType type, const void* element, std::size_t elementsize)
		{
			bool rt = true;
			switch (type)
			{
				case protocol::FormatOutput::OpenTag:
					m_attribname.clear();
					if (m_taglevel == 0)
					{
						if (m_nofroot > 0)
						{
							m_error = ErrLibXMLMultiRootElement;
							rt = false;
						}
						else
						{
							m_nofroot += 1;
						}
					}
					if (0>xmlTextWriterStartElement( m_writer, getElement( element, elementsize)))
					{
						m_error = ErrLibXMLStartElement;
						rt = false;
					}
					m_taglevel += 1;
					break;

				case protocol::FormatOutput::Attribute:
					if (m_attribname.size())
					{
						m_error = ErrIllegalOperation;
						rt = false;
					}
					m_attribname.clear();
					m_attribname.append( (const char*)element, elementsize);
					break;

				case protocol::FormatOutput::Value:
					if (m_attribname.empty())
					{
						if (0>xmlTextWriterWriteString( m_writer, getElement( element, elementsize)))
						{
							m_error = ErrLibXMLText;
							rt = false;
						}
					}
					else if (0>xmlTextWriterWriteAttribute( m_writer, getXmlString(m_attribname), getElement( element, elementsize)))
					{
						m_error = ErrLibXMLAttribute;
						rt = false;
					}
					else
					{
						m_attribname.clear();
					}
					break;

				case protocol::FormatOutput::CloseTag:
					if (0>xmlTextWriterEndElement( m_writer))
					{
						m_error = ErrLibXMLEndElement;
						rt = false;
					}
					else if (m_taglevel == 1)
					{
						if (0>xmlTextWriterEndDocument( m_writer))
						{
							m_error = ErrLibXMLEndElement;
							rt = false;
						}
						else
						{
							std::size_t nn = xmlBufferLength( m_writerbuf);
							m_content.clear();
							m_content.append( (const char*)xmlBufferContent( m_writerbuf), nn);
							m_contentitr = m_content.begin();
							m_contentend = m_content.end();
						}
					}
					m_taglevel -= 1;
					m_attribname.clear();
					break;

				default:
					m_error = ErrIllegalState;
					rt = false;
			}
			return rt;
		}

		static const xmlChar* getXmlString( const std::string& aa)
		{
			return (const xmlChar*)aa.c_str();
		}

		xmlChar* getElement( const void* element, std::size_t elementsize)
		{
			m_valuestrbuf.clear();
			m_valuestrbuf.append( (const char*)element, elementsize);
			return (xmlChar*)m_valuestrbuf.c_str();
		}

		std::size_t printNextChunk( void* out, std::size_t outsize)
		{
			protocol::EscapingBuffer<textwolf::StaticBuffer> buf( (char*)out, outsize, m_bufstate);
			while (m_contentitr != m_contentend)
			{
				std::size_t bufmark = buf.size();
				buf.push_back( *m_contentitr);
				if (buf.overflow())
				{
					buf.resize( bufmark);
					break;
				}
				++m_contentitr;
			}
			m_bufstate = buf.state();
			return buf.size();
		}

	public:
		int m_nofroot;
		int m_taglevel;
		Error m_error;
		xmlBufferPtr m_writerbuf;
		xmlTextWriterPtr m_writer;
		xmlChar* m_valuebuf;
		std::string m_attribname;
		std::string m_valuestrbuf;
		std::string m_content;
		std::string::const_iterator m_contentitr;
		std::string::const_iterator m_contentend;
		protocol::EscapingBuffer<textwolf::StaticBuffer>::State m_bufstate;
	};

	bool flushBuffer( Document* dc)
	{
		bool rt = true;
		// if we have the whole document, then we start to print it and return an error, as long as we still have data:
		if (dc->m_contentitr < dc->m_contentend)
		{
			std::size_t nn = dc->printNextChunk( rest(), restsize());
			incPos( nn);
			if (dc->m_contentitr == dc->m_contentend)
			{
				setState( protocol::FormatOutput::Open);
				rt = true;
			}
			else
			{
				setState( protocol::FormatOutput::EndOfBuffer);
				rt = false;
			}
		}
		else
		{
			setState( protocol::FormatOutput::Open);
		}
		return rt;
	}

public:
	virtual bool print( ElementType type, const void* element, std::size_t elementsize)
	{
		bool rt = true;
		Document* dc;

		if ((dc = m_document.get()) == 0)
		{
			const char* ec = m_encoding.get()?m_encoding.get()->c_str():"UTF-8";
			m_document.reset( dc = new Document( ec));
		}
		if (dc->m_taglevel == 0 && dc->m_nofroot == 1)
		{
			rt = flushBuffer( dc);
		}
		else if (dc->print( type, element, elementsize))
		{
			if (dc->m_taglevel == 0 && dc->m_nofroot == 1)
			{
				rt = flushBuffer( dc);
			}
			else
			{
				setState( protocol::FormatOutput::Open);
				rt = true;
			}
		}
		else
		{
			setState( protocol::FormatOutput::Error, dc->m_error);
			rt = false;
		}
		return rt;
	}
public:
	FormatOutputImpl( const CountedReference<std::string>& enc)
		:m_encoding(enc),m_document(0){}
	FormatOutputImpl( const FormatOutputImpl& o)
		:m_encoding(o.m_encoding),m_document(o.m_document){}

	virtual ~FormatOutputImpl(){}

	///\brief self copy
	///\return copy of this
	virtual FormatOutputImpl* copy() const
	{
		return new FormatOutputImpl( *this);
	}

private:
	CountedReference<std::string> m_encoding;
	CountedReference<Document> m_document;
};

}//end anonymous namespace


Libxml2Filter::Libxml2Filter( std::size_t bufsize)
{
	CountedReference<std::string> enc;
	m_inputFilter.reset( new InputFilterImpl( enc, bufsize));
	m_formatOutput.reset( new FormatOutputImpl( enc));
}

