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

using namespace _Wolframe;
using namespace _Wolframe::langbind;

bool DocumentWriter::init( const char* encoding, bool standalone)
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

DocumentWriter::DocumentWriter( const char* encoding, const char* doctype, const char* publicid, const char* systemid)
{
	if (init( encoding, false))
	{
		xmlTextWriterPtr ww = m_writer.get();
		if (0>xmlTextWriterStartDTD( ww, (const xmlChar*)doctype, (const xmlChar*)publicid, (const xmlChar*)systemid)
		||  0>xmlTextWriterEndDTD(ww))
		{
			m_writer.reset();
			m_writerbuf.reset();
		}
	}
}
