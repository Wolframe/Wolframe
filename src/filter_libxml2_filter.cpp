#ifndef WITH_LIBXML2
#error Compiling a libxml2 module without libxml2 support enabled
#endif
#include "filter/libxml2_filter.hpp"
#include "filter/bufferingfilter.hpp"
#include "filter/doctype.hpp"
#include <cstddef>
#include <cstring>
#include <vector>
#include <string>
#include <stdexcept>
#include "libxml/parser.h"
#include "libxml/tree.h"
#include "libxml/encoding.h"
#include "libxml/xmlwriter.h"
#include "libxml/xmlsave.h"
#include <boost/shared_ptr.hpp>

using namespace _Wolframe;
using namespace langbind;

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


class DocumentReader
{
public:
	DocumentReader(){}

	DocumentReader( const DocumentReader& o)
		:m_ptr(o.m_ptr){}

	DocumentReader( const char* content, std::size_t contentsize)
	{
		int options = XML_PARSE_NOENT | XML_PARSE_COMPACT | XML_PARSE_NONET | XML_PARSE_NODICT;
		xmlDocPtr pp = xmlReadMemory( content, contentsize, "noname.xml", NULL, options);
		if (pp)
		{
			m_ptr = boost::shared_ptr<xmlDoc>( pp, xmlFreeDoc);
		}
	}
	xmlDocPtr get() const
	{
		return m_ptr.get();
	}
private:
	boost::shared_ptr<xmlDoc> m_ptr;
};

class DocumentWriter
{
public:
	DocumentWriter(){}

	DocumentWriter( const DocumentWriter& o)
		:m_writerbuf(o.m_writerbuf)
		,m_writer(o.m_writer){}

	bool init( const char* encoding, bool standalone)
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

	DocumentWriter( const char* encoding, const char* doctype, const char* publicid, const char* systemid)
	{
		if (init( encoding, false))
		{
			xmlTextWriterPtr ww = m_writer.get();
			if (0>xmlTextWriterStartDTD( ww, (const xmlChar*)doctype, (const xmlChar*)publicid, (const xmlChar*)systemid))
			{
				xmlTextWriterEndDTD(ww);
			}
		}
	}

	DocumentWriter( const char* encoding)
	{
		init( encoding, true);
	}

	xmlTextWriterPtr get() const
	{
		return m_writer.get();
	}

	const std::string getElement()
	{
		return std::string(
			(const char*)xmlBufferContent( m_writerbuf.get()),
			xmlBufferLength( m_writerbuf.get()));

	}
private:
	boost::shared_ptr<xmlBuffer> m_writerbuf;
	boost::shared_ptr<xmlTextWriter> m_writer;
};



struct InputFilterImpl :public InputFilter
{
	InputFilterImpl( const CountedReference<std::string>& e)
		:m_node(0)
		,m_value(0)
		,m_prop(0)
		,m_propvalues(0)
		,m_taglevel(0)
		,m_withEmpty(false)
		,m_encoding(e){}

	InputFilterImpl( const InputFilterImpl& o)
		:InputFilter(o)
		,m_doc(o.m_doc)
		,m_node(o.m_node)
		,m_value(o.m_value)
		,m_prop(o.m_prop)
		,m_propvalues(o.m_propvalues)
		,m_taglevel(o.m_taglevel)
		,m_withEmpty(o.m_withEmpty)
		,m_elembuf(o.m_elembuf)
		,m_encoding(o.m_encoding)
		,m_doctype_root(o.m_doctype_root)
		,m_doctype_public(o.m_doctype_public)
		,m_doctype_system(o.m_doctype_system)
		{}

	///\brief Implements InputFilter::copy()
	virtual InputFilter* copy() const
	{
		return new InputFilterImpl(*this);
	}

	///\brief Implements FilterBase::getValue(const char*,std::string&)
	virtual bool getValue( const char* name, std::string& val)
	{
		if (std::strcmp( name, "empty") == 0)
		{
			val = m_withEmpty?"true":"false";
			return true;
		}
		return InputFilter::getValue( name, val);
	}

	///\brief Implements FilterBase::getDocType(std::string&)
	virtual bool getDocType( std::string& val)
	{
		DocType doctype;
		if (getDocType( doctype))
		{
			val = doctype.tostring();
			return true;
		}
		else
		{
			return false;
		}
	}

	///\brief Implements FilterBase::setValue(const char*,const std::string&)
	virtual bool setValue( const char* name, const std::string& value)
	{
		if (std::strcmp( name, "empty") == 0)
		{
			if (std::strcmp( value.c_str(), "true") == 0)
			{
				m_withEmpty = true;
			}
			else if (std::strcmp( value.c_str(), "false") == 0)
			{
				m_withEmpty = false;
			}
			else
			{
				return false;
			}
			return true;
		}
		return InputFilter::setValue( name, value);
	}

	///\brief Implements InputFilter::putInput(const void*,std::size_t,bool)
	virtual void putInput( const void* content, std::size_t contentsize, bool end)
	{
		if (!end) throw std::logic_error( "internal: need buffering input filter");
		m_nodestk.clear();

		m_doc = DocumentReader( (const char*)content, contentsize);
		if (!m_doc.get())
		{
			xmlError* err = xmlGetLastError();
			setState( Error, err->message);
		}
		m_node = xmlDocGetRootElement( m_doc.get());
		xmlDtdPtr dtd = (xmlDtdPtr)m_node->parent;
		if (dtd && dtd->type)
		{
			m_doctype_root = getElementString( dtd->name);
			m_doctype_public = getElementString( dtd->ExternalID);
			m_doctype_system = getElementString( dtd->SystemID);
		}

		const xmlChar* ec = m_doc.get()->encoding;
		m_encoding.reset( new std::string);
		for (int ii=0; ec[ii]!=0; ii++)
		{
			m_encoding->push_back((unsigned char)ec[ii]);
		}
	}

	bool getDocType( DocType& doctype)
	{
		if (!m_doc.get()) return false;
		doctype.rootid = m_doctype_root.size()?m_doctype_root.c_str():0;
		doctype.publicid = m_doctype_public.size()?m_doctype_public.c_str():0;
		doctype.systemid = m_doctype_system.size()?m_doctype_system.c_str():0;
		return true;
	}

	///\brief implement interface member InputFilter::getNext( typename FilterBase::ElementType&,const void*&,std::size_t&)
	virtual bool getNext( InputFilter::ElementType& type, const void*& element, std::size_t& elementsize)
	{
		if (state() == Error) return false;
		setState( Open);
		bool rt = true;
	AGAIN:
		if (!m_doc.get())
		{
			rt = false;
		}
		else if (m_value)
		{
			type = InputFilter::Value;
			getElement( element, elementsize, m_value);
			m_value = 0;
		}
		else if (m_prop && m_propvalues)
		{
			type = InputFilter::Attribute;
			getElement( element, elementsize, m_prop->name);
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
				if (m_taglevel >= 0)
				{
					m_taglevel -= 1;
					m_doc = DocumentReader();
					elementsize = 0;
					type = InputFilter::CloseTag;
					rt = true;
				}
				else
				{
					setState( Error, "illegal state - get next called after end of document");
					rt = false;
				}
			}
			else
			{
				m_node = m_nodestk.back();
				m_nodestk.pop_back();
				m_taglevel -= 1;
				elementsize = 0;
				type = InputFilter::CloseTag;
				rt = true;
			}
		}
		else switch (m_node->type)
		{
			case XML_HTML_DOCUMENT_NODE:
			case XML_DOCB_DOCUMENT_NODE:
			case XML_DOCUMENT_NODE:
			case XML_ELEMENT_NODE:
				type = InputFilter::OpenTag;
				m_prop = m_node->properties;
				if (m_prop) m_propvalues = m_prop->children;
				m_nodestk.push_back( m_node->next);
				getElement( element, elementsize, m_node->name);
				m_node = m_node->children;
				m_taglevel += 1;
				break;

			case XML_ATTRIBUTE_NODE:
				type = InputFilter::Attribute;
				getElement( element, elementsize, m_node->name);
				m_value = m_node->content;
				m_node = m_node->next;
				break;

			case XML_TEXT_NODE:
				type = InputFilter::Value;
				if (!m_withEmpty)
				{
					std::size_t ii=0;
					while (m_node->content[ii] != 0 && m_node->content[ii]<=' ') ++ii;
					if (m_node->content[ii] == 0)
					{
						m_node = m_node->next;
						goto AGAIN;
					}
				}
				getElement( element, elementsize, m_node->content);
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
				goto AGAIN;
		}
		return rt;
	}

private:
	std::string getElementString( const xmlChar* str)
	{
		return str?std::string( (const char*)str, xmlStrlen(str) * sizeof(*str)):std::string();
	}

	void getElement( const void*& element, std::size_t& elementsize, const xmlChar* str)
	{
		if (!str)
		{
			element = "";
			elementsize = 0;
		}
		else
		{
			elementsize = xmlStrlen( str) * sizeof(*str);
			element = str;
		}
	}
private:
	DocumentReader m_doc;
	xmlNode* m_node;
	xmlChar* m_value;
	xmlAttr* m_prop;
	xmlNode* m_propvalues;
	int m_taglevel;
	std::vector<xmlNode*> m_nodestk;
	bool m_withEmpty;
	std::string m_elembuf;
	CountedReference<std::string> m_encoding;
	std::string m_doctype_root;
	std::string m_doctype_public;
	std::string m_doctype_system;
};


class OutputFilterImpl :public OutputFilter
{
public:
	OutputFilterImpl( const CountedReference<std::string>& enc)
		:m_encoding(enc)
		,m_nofroot(0)
		,m_taglevel(0)
		,m_elemitr(0){}

	OutputFilterImpl( const OutputFilterImpl& o)
		:OutputFilter(o)
		,m_encoding(o.m_encoding)
		,m_doc(o.m_doc)
		,m_nofroot(o.m_nofroot)
		,m_taglevel(o.m_taglevel)
		,m_attribname(o.m_attribname)
		,m_valuestrbuf(o.m_valuestrbuf)
		,m_elembuf(o.m_elembuf)
		,m_elemitr(o.m_elemitr){}

	virtual ~OutputFilterImpl(){}

	///\brief Implementation of OutputFilter::copy()
	virtual OutputFilterImpl* copy() const
	{
		return new OutputFilterImpl( *this);
	}

	///\brief Implementation of OutputFilter::setDocType( const std::string&)
	virtual void setDocType( const std::string& value)
	{
		DocType doctype( value);
		if (doctype.rootid)
		{
			m_doctype_root = doctype.rootid;
			if (doctype.publicid) m_doctype_public = doctype.publicid;
			if (doctype.systemid) m_doctype_system = doctype.systemid;
		}
	}

	///\brief Implementation of OutputFilter::print( ElementType, const void*,std::size_t)
	virtual bool print( ElementType type, const void* element, std::size_t elementsize)
	{
		bool rt = true;
		xmlTextWriterPtr xmlout = m_doc.get();

		if (!xmlout)
		{
			const char* ec = m_encoding.get()?m_encoding.get()->c_str():"UTF-8";
			if (m_doctype_root.size())
			{
				m_doc = DocumentWriter( ec, m_doctype_root.c_str(), m_doctype_public.c_str(), m_doctype_system.c_str());
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
						m_elembuf = m_doc.getElement();
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

private:
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

	bool flushBuffer()
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
private:
	CountedReference<std::string> m_encoding;
	DocumentWriter m_doc;
	int m_nofroot;
	int m_taglevel;
	std::string m_attribname;
	std::string m_valuestrbuf;
	std::string m_elembuf;
	std::size_t m_elemitr;
	std::string m_doctype_root;
	std::string m_doctype_public;
	std::string m_doctype_system;
};

}//end anonymous namespace

struct Libxml2Filter :public Filter
{
	Libxml2Filter()
	{
		CountedReference<std::string> enc;
		InputFilterImpl impl( enc);

		m_inputfilter.reset( new BufferingInputFilter( &impl));
		m_outputfilter.reset( new OutputFilterImpl( enc));
	}
};

Filter Libxml2FilterFactory::create( const char*) const
{
	return Libxml2Filter();
}


