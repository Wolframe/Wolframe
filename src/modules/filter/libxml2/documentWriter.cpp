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
///\file documentWriter.hpp
///\brief Implemtation of document writer abstraction for the libxml2 library
#include "documentWriter.hpp"
#include <stdexcept>

using namespace _Wolframe;
using namespace _Wolframe::langbind;

static const xmlChar* getXmlString( const char* aa)
{
	return (const xmlChar*)aa;
}

static std::string errorMessage( const char* msg)
{
	std::string rt;
	xmlError* err = xmlGetLastError();
	if (err)
	{
		if (msg)
		{
			rt.append( msg);
			rt.append( ":");
		}
		rt.append( (const char*)err->message);
	}
	else
	{
		rt.append( msg?msg:"unknown error");
	}
	return rt;
	
}

DocumentWriter::DocumentWriter( const char* encoding, const char* standalone, const char* root, const char* publicid, const char* systemid)
{
	xmlBufferPtr bb = xmlBufferCreate();
	if (!bb) throw std::runtime_error( errorMessage( "failed to create libxml2 buffer"));

	m_writerbuf = boost::shared_ptr<xmlBuffer>( bb, xmlBufferFree);
	xmlTextWriterPtr ww = xmlNewTextWriterMemory( bb, 0);
	if (!ww) throw std::runtime_error( errorMessage( "failed to create libxml2 writer"));

	m_writer = boost::shared_ptr<xmlTextWriter>( ww, xmlFreeTextWriter);
	if (0>xmlTextWriterStartDocument( ww, "1.0", encoding, standalone))
	{
		throw std::runtime_error( errorMessage( "failed to write document header (XML header)"));
	}
	if (systemid)
	{
		if (0>xmlTextWriterStartDTD( ww, (const xmlChar*)root, (const xmlChar*)publicid, (const xmlChar*)systemid)
		||  0>xmlTextWriterEndDTD(ww))
		{
			throw std::runtime_error( errorMessage( "failed to write document header (DTD)"));
		}
	}
	if (!root)
	{
		throw std::runtime_error( "no XML root element defined");
	}
	if (0>xmlTextWriterStartElement( ww, getXmlString( root)))
	{
		throw std::runtime_error( errorMessage( "libxml2 filter: write root element error"));
	}
}


