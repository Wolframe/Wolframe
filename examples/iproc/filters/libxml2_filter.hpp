/************************************************************************
Copyright (C) 2011 Project Wolframe.
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
///\file libxml2_filter.hpp
///\brief Filter using the libxml2 library for input and output

#ifndef _Wolframe_LIBXML2_FILTER_HPP_INCLUDED
#define _Wolframe_LIBXML2_FILTER_HPP_INCLUDED

#include "bufferingFilterBase.hpp"
#include "protocol/streamIterator.hpp"
#include <cstddef>
#include <vector>
#include <string>
#include <libxml/parser.h>
#include <libxml/tree.h>

namespace _Wolframe {
namespace filter {
namespace libxml2 {

class Content
{
public:
	Content() :m_doc(0),m_node(0),m_value(0),m_prop(0),m_propvalues(0){}

	bool end() const
	{
		return (m_doc == 0);
	}

	bool open( const void* content, std::size_t size)
	{
		if (m_doc) xmlFreeDoc( m_doc);
		m_nodestk.clear();

		int options = XML_PARSE_NOENT | XML_PARSE_DTDLOAD;
		m_doc = xmlReadMemory( (const char*)content, size, "noname.xml", NULL, options);
		if (!m_doc) return false;

		m_node = xmlDocGetRootElement( m_doc);
		return true;
	}

	~Content()
	{
		if (m_doc) xmlFreeDoc( m_doc);
	}

	bool fetch( protocol::InputFilter::ElementType* type, void* buffer, std::size_t buffersize, std::size_t* bufferpos)
	{
		bool rt = true;
	AGAIN:
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
				goto AGAIN;
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
	xmlDocPtr m_doc;
	xmlNode* m_node;
	xmlChar* m_value;
	xmlAttr* m_prop;
	xmlNode* m_propvalues;
	std::vector<xmlNode*> m_nodestk;
};

struct InputFilter :public BufferingInputFilter<Content>
{
	InputFilter(){}
};

}}}//namespace
#endif

