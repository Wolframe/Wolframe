/************************************************************************
Copyright (C) 2011 - 2014 Project Wolframe.
All rights reserved.

This file is part of Project Wolframe.

Commercial Usage
Licensees holding valid Project Wolframe Commercial licenses may
use this file in accordance with the Project Wolframe
Commercial License Agreement provided with the Software or,
alternatively, in accordance with the terms contained
in a written agreement between the licensee and Project Wolframe.

GNU General Public License Usage
Alternatively, you can redistribute this file and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Wolframe is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
///\file inputfilterImpl.cpp
///\brief Implementation of input filter abstraction for the libxml2 library
#include "inputfilterImpl.hpp"
#include "utils/fileUtils.hpp"
#include "logger-v1.hpp"

using namespace _Wolframe;
using namespace _Wolframe::langbind;

bool InputFilterImpl::getValue( const char* id, std::string& val) const
{
	if (std::strcmp( id, "empty") == 0)
	{
		val = m_withEmpty?"true":"false";
		return true;
	}
	return InputFilter::getValue( id, val);
}

bool InputFilterImpl::setValue( const char* id, const std::string& value)
{
	if (std::strcmp( id, "empty") == 0)
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
	return InputFilter::setValue( id, value);
}

void InputFilterImpl::putInput( const void* content, std::size_t contentsize, bool end)
{
	if (!end) throw std::logic_error( "internal: need buffering input filter");
	m_nodestk.clear();

	try
	{
#if WITH_LIBXSLT
		if (m_xsltMapper.defined())
		{
			m_doc = m_xsltMapper.apply( DocumentReader( (const char*)content, contentsize));
		}
		else
		{
			m_doc = DocumentReader( (const char*)content, contentsize);
		}
#else
		m_doc = DocumentReader( (const char*)content, contentsize);
#endif
	}
	catch (const std::runtime_error& err)
	{
		setState( InputFilter::Error, err.what());
		return;
	}
	catch (const std::bad_alloc& err)
	{
		setState( InputFilter::Error, "out of memory");
		return;
	}
	catch (const std::logic_error& err)
	{
		LOG_FATAL << "logic error in libxml2 filer: " << err.what();
		setState( InputFilter::Error, "logic error in libxml2 filer. See logs");
		return;
	}
	if (!m_doc.get())
	{
		xmlError* err = xmlGetLastError();
		setState( Error, err->message);
	}
	else
	{
		initDocMetaData();
		LOG_DEBUG << "[libxml2 input] document meta data: {" << getMetaDataRef()->tostring() << "}";
		setState( Open);
	}
}

void InputFilterImpl::initDocMetaData()
{
	m_node = xmlDocGetRootElement( m_doc.get());
	const xmlChar* ec = m_doc.get()->encoding;
	if (!ec)
	{
		setAttribute( "encoding", "UTF-8");
	}
	else
	{
		std::string encoding;
		for (int ii=0; ec[ii]!=0; ii++)
		{
			encoding.push_back((unsigned char)ec[ii]);
		}
		setAttribute( "encoding", encoding);
	}
	// Inspect !DOCTYPE entity for meta data:
	xmlNode* nd = m_doc.get()->children;
	while (nd && nd->type != XML_DTD_NODE)
	{
		nd = nd->next;
	}
	if (nd)
	{
		xmlDtdPtr dtd = (xmlDtdPtr)nd;
		if (dtd->SystemID)
		{
			setAttribute( "SYSTEM", (const char*)dtd->SystemID);
			setDoctype( types::DocMetaData::extractStem( (const char*)dtd->SystemID));
		}
		if (dtd->ExternalID)
		{
			setAttribute( "PUBLIC", (const char*)dtd->ExternalID);
		}
	}
	// Inspect root xmlns attributes for meta data:
	if (m_node && (m_node->type == XML_ELEMENT_NODE || m_node->type == XML_DOCUMENT_NODE))
	{
		if (m_node->name)
		{
			setAttribute( "root", (const char*)m_node->name);
		}
		if (m_node->nsDef && m_node->nsDef->href)
		{
			std::string xmlns( getElementString( m_node->nsDef->href));
			setAttribute( "xmlns", xmlns);
		}
		xmlAttr* rootattr = m_node->properties;
		while (rootattr)
		{
			xmlNode* rootvalues = 0;
			if (rootattr) rootvalues = rootattr->children;
			std::string prefix;
			if (rootattr->ns && rootattr->ns->prefix)
			{
				prefix = getElementString( rootattr->ns->prefix);
				if (rootattr->ns->href)
				{
					setAttribute( std::string("xmlns:") + prefix, getElementString( rootattr->ns->href));
				}
			}
			std::string attrname = getElementString( rootattr->name);
			std::string attrvalue;
			while (rootvalues)
			{
				attrvalue.append( getElementString( rootvalues->content));
				rootvalues = rootvalues->next;
			}
			if (prefix.size())
			{
				setAttribute( prefix + ":" + attrname, attrvalue);
				if (attrname == "schemaLocation")
				{
					setDoctype( types::DocMetaData::extractStem( attrvalue));
				}
			}
			else
			{
				LOG_WARNING << "unknown XML root element attribute '" << attrname << "'";
			}
			rootattr = rootattr->next;
		}
	}
	m_nodestk.push_back( m_node->next);
	m_node = m_node->children;
	m_taglevel += 1;
}

bool InputFilterImpl::getNext( InputFilter::ElementType& type, const void*& element, std::size_t& elementsize)
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
				elementsize = 0;
				type = InputFilter::CloseTag;
				rt = true;
			}
			else
			{
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

const types::DocMetaData* InputFilterImpl::getMetaData()
{
	if (!m_doc.get())
	{
		setState( EndOfMessage);
		return 0;
	}
	return getMetaDataRef().get();
}

std::string InputFilterImpl::getElementString( const xmlChar* str)
{
	return str?std::string( (const char*)str, xmlStrlen(str) * sizeof(*str)):std::string();
}

void InputFilterImpl::getElement( const void*& element, std::size_t& elementsize, const xmlChar* str)
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

bool InputFilterImpl::checkSetFlags( Flags f) const
{
	return (0==((int)f & (int)langbind::FilterBase::SerializeWithIndices));
}

bool InputFilterImpl::setFlags( Flags f)
{
	if (0!=((int)f & (int)langbind::FilterBase::SerializeWithIndices))
	{
		return false;
	}
	return InputFilter::setFlags( f);
}

bool InputFilterImpl::checkMetaData( const types::DocMetaData& md)
{
	if (state() == Start)
	{
		setState( Error, "input filter did not parse its meta data yet - cannot check them therefore");
	}
	// Check the XML root element:
	const char* form_rootelem = md.getAttribute( "root");
	const char* doc_rootelem = getMetaDataRef()->getAttribute( "root");
	if (form_rootelem)
	{
		if (!doc_rootelem)
		{
			setState( Error, "input document has no root element defined");
			return false;
		}
		if (0!=std::strcmp(form_rootelem,doc_rootelem))
		{
			std::string msg = std::string("input document root element '") + doc_rootelem + "' does not match the root element '" + form_rootelem + "'' required";
			setState( Error, msg.c_str());
			return false;
		}
	}
	return true;
}


