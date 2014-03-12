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

using namespace _Wolframe;
using namespace _Wolframe::langbind;

bool InputFilterImpl::getValue( const char* name, std::string& val)
{
	if (std::strcmp( name, "empty") == 0)
	{
		val = m_withEmpty?"true":"false";
		return true;
	}
	return InputFilter::getValue( name, val);
}

bool InputFilterImpl::setValue( const char* name, const std::string& value)
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

void InputFilterImpl::putInput( const void* content, std::size_t contentsize, bool end)
{
	if (!end) throw std::logic_error( "internal: need buffering input filter");
	m_nodestk.clear();

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
	if (!m_doc.get())
	{
		xmlError* err = xmlGetLastError();
		setState( Error, err->message);
	}
	else
	{
		m_node = xmlDocGetRootElement( m_doc.get());

		const xmlChar* ec = m_doc.get()->encoding;
		if (!ec)
		{
			m_encoding = "UTF-8";
		}
		else
		{
			m_encoding.clear();
			for (int ii=0; ec[ii]!=0; ii++)
			{
				m_encoding.push_back((unsigned char)ec[ii]);
			}
		}
	}
}

bool InputFilterImpl::getDocType( types::DocType& doctype)
{
	if (!m_doc.get())
	{
		return false;
	}
	xmlNode* nd = m_doc.get()->children;
	while (nd && nd->type != XML_DTD_NODE)
	{
		nd = nd->next;
	}
	if (nd)
	{
		xmlDtdPtr dtd = (xmlDtdPtr)nd;
		std::string systemid = dtd->SystemID?(const char*)dtd->SystemID:"";
		std::string ext = utils::getFileExtension( systemid);
		std::string id = utils::getFileStem( systemid);
		std::string dir;
		std::size_t namesize = ext.size() + id.size();
		if (namesize < systemid.size())
		{
			dir = std::string( systemid.c_str(), systemid.size() - namesize);
		}
		const char* root = dtd->name?(const char*)dtd->name:"";
		doctype.init( id, root, types::DocType::SchemaPath( dir, ext));
	}
	else
	{
		doctype.clear();
	}
	return true;
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

bool InputFilterImpl::setFlags( Flags f)
{
	if (0!=((int)f & (int)langbind::FilterBase::SerializeWithIndices))
	{
		return false;
	}
	return InputFilter::setFlags( f);
}

