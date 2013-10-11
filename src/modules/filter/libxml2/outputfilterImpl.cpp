/************************************************************************
Copyright (C) 2011 - 2013 Project Wolframe.
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
///\file outputfilterImpl.cpp
///\brief Implementaion of output filter abstraction for the libxml2 library
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
		const char* ec = encoding();
		if (m_doctype_root.size())
		{
			m_doc = DocumentWriter( ec?ec:"UTF-8", m_doctype_root.c_str(), m_doctype_public.size()?m_doctype_public.c_str():0, m_doctype_system.size()?m_doctype_system.c_str():0);
		}
		else
		{
			m_doc = DocumentWriter( ec?ec:"UTF-8");
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
	m_doctype_root = doctype.rootid;
	m_doctype_public = doctype.publicid;
	m_doctype_system = doctype.systemid;
}

const char* OutputFilterImpl::encoding() const
{
	if (m_encoding.empty())
	{
		if (attributes())
		{
			return attributes()->getEncoding();
		}
		return 0;
	}
	else
	{
		return m_encoding.c_str();
	}
}

bool OutputFilterImpl::getValue( const char* name, std::string& val)
{
	if (std::strcmp( name, "encoding") == 0)
	{
		const char* ee = encoding();
		if (ee)
		{
			val = ee;
			return true;
		}
		return false;
	}
	return Parent::getValue( name, val);
}

bool OutputFilterImpl::setValue( const char* name, const std::string& value)
{
	if (std::strcmp( name, "encoding") == 0)
	{
		m_encoding = value;
		return true;
	}
	return Parent::setValue( name, value);
}

