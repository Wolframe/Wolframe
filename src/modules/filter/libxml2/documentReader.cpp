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
///\file documentReader.hpp
///\brief Implementation of document reader abstraction for the libxml2 library
#include "documentReader.hpp"
#include <stdexcept>
#include <sstream>

using namespace _Wolframe;
using namespace _Wolframe::langbind;

DocumentReader::DocumentReader( const char* content, std::size_t contentsize)
{
	int options = XML_PARSE_NOENT | XML_PARSE_COMPACT | XML_PARSE_NONET | XML_PARSE_NODICT;
	xmlDocPtr pp = xmlReadMemory( content, contentsize, "noname.xml", NULL, options);
	if (pp)
	{
		m_ptr = boost::shared_ptr<xmlDoc>( pp, xmlFreeDoc);
	}
}

DocumentReader::DocumentReader( xmlDocPtr doc)
	:m_ptr(doc, xmlFreeDoc){}


std::string DocumentReader::getContent() const
{
	xmlChar* mem;
	int memsize;
	xmlDocDumpMemory( m_ptr.get(), &mem, &memsize);
	if (!mem)
	{
		xmlError* err = xmlGetLastError();
		std::ostringstream s;
		s << "Failed to dump XML document content: " << err->message << " at "
			<< err->line;
		throw std::runtime_error( s.str());
	}
	boost::shared_ptr<xmlChar> contentref( mem, xmlFree);
	return std::string( (char*)mem, memsize);
}

