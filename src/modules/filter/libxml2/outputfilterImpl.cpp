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
///\file outputfilterImpl.cpp
///\brief Implementaion of output filter abstraction for the libxml2 library
#include "outputfilterImpl.hpp"
#include "logger-v1.hpp"

using namespace _Wolframe;
using namespace _Wolframe::langbind;

OutputFilterImpl::OutputFilterImpl( const XsltMapper& xsltMapper_, const types::DocMetaDataR& inheritMetaData_)
	:utils::TypeSignature("langbind::OutputFilterImpl (libxml2)", __LINE__)
	,OutputFilter("libxslt", inheritMetaData_)
	,m_xsltMapper(xsltMapper_)
	,m_taglevel(0)
	,m_emptyDocument(false)
	,m_elemitr(0)
	{}

OutputFilterImpl::OutputFilterImpl( const types::DocMetaDataR& inheritMetaData_)
	:utils::TypeSignature("langbind::OutputFilterImpl (libxml2)", __LINE__)
	,OutputFilter("libxml2", inheritMetaData_)
	,m_taglevel(0)
	,m_emptyDocument(false)
	,m_elemitr(0)
	{}

OutputFilterImpl::OutputFilterImpl( const OutputFilterImpl& o)
	:utils::TypeSignature("langbind::OutputFilterImpl (libxml2)", __LINE__)
	,OutputFilter(o)
	,m_doc(o.m_doc)
	,m_xsltMapper(o.m_xsltMapper)
	,m_taglevel(o.m_taglevel)
	,m_emptyDocument(o.m_emptyDocument)
	,m_attribname(o.m_attribname)
	,m_valuestrbuf(o.m_valuestrbuf)
	,m_elembuf(o.m_elembuf)
	,m_elemitr(o.m_elemitr)
	{}

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

void OutputFilterImpl::setXmlError( const char* msg)
{
	xmlError* err = xmlGetLastError();
	if (err)
	{
		std::string msgstr = std::string(msg) + ": " + (const char*)err->message;
		setState( Error, msgstr.c_str());
	}
	else
	{
		setState( Error, msg);
	}
}

static bool metadataContainSchemaReference( const types::DocMetaData& md)
{
	std::vector<types::DocMetaData::Attribute>::const_iterator ai = md.attributes().begin(), ae = md.attributes().end();
	for (; ai != ae; ++ai)
	{
		if (0==std::memcmp( ai->name.c_str(), "xmlns", 5)
		||  0==std::memcmp( ai->name.c_str(), "xsi:", 4))
		{
			return true;
		}
	}
	return false;
}

bool OutputFilterImpl::printHeader()
{
	types::DocMetaData md( getMetaData());
	LOG_DEBUG << "[libxml2 output] document meta data: {" << md.tostring() << "}";
	const char* standalone = md.getAttribute( "standalone");
	const char* encoding = md.getAttribute( "encoding");
	if (!encoding) encoding = "UTF-8";
	const char* root = md.getAttribute( "root");
	if (!root)
	{
		setState( Error, "no XML root element defined");
		return false;
	}
	const char* doctype_public = md.getAttribute( "PUBLIC");
	const char* doctype_system = md.getAttribute( "SYSTEM");
	std::string doctype_system_buf;
	if (doctype_system && !md.doctype().empty())
	{
		doctype_system_buf = types::DocMetaData::replaceStem( doctype_system, md.doctype());
		doctype_system = doctype_system_buf.c_str();
	}
	if (!standalone)
	{
		if (doctype_system)
		{
			standalone = "no";
		}
		else if (!metadataContainSchemaReference( md))
		{
			standalone = "yes";
		}
	}
	try
	{
		m_doc = DocumentWriter( encoding, standalone, root, doctype_public, doctype_system);
		xmlTextWriterPtr xmlout = m_doc.get();
		if (xmlout)
		{
			std::vector<types::DocMetaData::Attribute>::const_iterator ai = md.attributes().begin(), ae = md.attributes().end();
			for (; ai != ae; ++ai)
			{
				std::string value;
				const char* valueptr = ai->value.c_str();

				const char* cc = std::strchr( ai->name.c_str(), ':');
				if (cc != 0)
				{
					if (0==std::strcmp(cc+1,"schemaLocation")
					||  0==std::strcmp(cc+1,"noNamespaceSchemaLocation"))
					{
						if (!md.doctype().empty())
						{
							value = types::DocMetaData::replaceStem( ai->value, md.doctype());
							valueptr = value.c_str();
						}
					}
				}
				if (0==std::memcmp( ai->name.c_str(), "xmlns", 5)
				||  0==std::memcmp( ai->name.c_str(), "xsi:", 4))
				{
					if (0>xmlTextWriterWriteAttribute( xmlout, getXmlString(ai->name.c_str()), getXmlString(valueptr)))
					{
						setXmlError( "libxml2 filter: write XML header attribute error");
						return false;
					}
				}
			 }
		}
	}
	catch (const std::runtime_error& e)
	{
		setState( Error, e.what());
		return false;
	}
	setState( Open);
	m_taglevel = 1;
	return true;
}

bool OutputFilterImpl::close()
{
	if (m_taglevel == 0)
	{
		xmlTextWriterPtr xmlout = m_doc.get();
		if (!xmlout)
		{
			// ... document is empty and got close without anything printed yet. So we have to print the header:
			if (!printHeader())
			{
				return false;
			}
		}
		return flushBuffer();
	}
	if (m_taglevel > 1)
	{
		setState( Error, "libxml2 document close but tags not balanced");
		return false;
	}
	if (m_taglevel > 0)
	{
		return print( FilterBase::CloseTag, 0, 0);
	}
	return true;
}

bool OutputFilterImpl::print( ElementType type, const void* element, std::size_t elementsize)
{
	bool rt = true;
	xmlTextWriterPtr xmlout = m_doc.get();

	if (!xmlout)
	{
		if (m_emptyDocument)
		{
			setState( Error, "libxml2 illegal print operation after final close (empty document)");
			return false;
		}
		if (!printHeader())
		{
			return false;
		}
		if (type == FilterBase::CloseTag)
		{
			m_emptyDocument = true;
		}
		xmlout = m_doc.get();
	}
	if (m_taglevel == 0)
	{
		return flushBuffer();
	}
	switch (type)
	{
		case FilterBase::OpenTag:
			m_attribname.clear();
			if (0>xmlTextWriterStartElement( xmlout, getElement( element, elementsize)))
			{
				setXmlError( "libxml2 write start element error");
				rt = false;
			}
			m_taglevel += 1;
			break;

		case FilterBase::Attribute:
			if (m_attribname.size())
			{
				setXmlError( "libxml2 illegal operation");
				rt = false;
			}
			m_attribname.clear();
			m_attribname.append( (const char*)element, elementsize);
			break;

		case FilterBase::Value:
			if (m_attribname.empty())
			{
				if (0>xmlTextWriterWriteString( xmlout, getElement( element, elementsize)))
				{
					setXmlError( "libxml2 write value error");
					rt = false;
					break;
				}
			}
			else if (0>xmlTextWriterWriteAttribute( xmlout, getXmlString(m_attribname), getElement( element, elementsize)))
			{
				setXmlError( "libxml2 write attribute error");
				rt = false;
				break;
			}
			else
			{
				m_attribname.clear();
			}
			break;

		case FilterBase::CloseTag:
			if (0>xmlTextWriterEndElement( xmlout))
			{
				setXmlError( "libxml2 write close tag error");
				rt = false;
				break;
			}
			m_taglevel -= 1;
			if (m_taglevel == 0)
			{
				if (0>xmlTextWriterEndDocument( xmlout))
				{
					setXmlError( "libxml2 write end document error");
					rt = false;
					break;
				}
#if WITH_LIBXSLT
				if (m_xsltMapper.defined())
				{
					m_elembuf = m_xsltMapper.apply( m_doc.getContent());
				}
				else
				{
					m_elembuf = m_doc.getContent();
				}
#else
				m_elembuf = m_doc.getContent();
#endif
				m_elemitr = 0;
				m_taglevel = 0;
				m_attribname.clear();
				return flushBuffer();
			}
			break;

		default:
			setState( Error, "libxml2 filter: illegal state");
			rt = false;
	}
	return rt;
}

bool OutputFilterImpl::getValue( const char* id, std::string& val) const
{
	return Parent::getValue( id, val);
}

bool OutputFilterImpl::setValue( const char* id, const std::string& value)
{
	return Parent::setValue( id, value);
}

