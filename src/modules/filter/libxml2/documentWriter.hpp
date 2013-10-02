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
///\file documentWriter.hpp
///\brief Document writer abstraction for the libxml2 library

#ifndef _Wolframe_LIBXML2_DOCUMENT_WRITER_HPP_INCLUDED
#define _Wolframe_LIBXML2_DOCUMENT_WRITER_HPP_INCLUDED
#include "libxml/parser.h"
#include "libxml/tree.h"
#include "libxml/encoding.h"
#include "libxml/xmlwriter.h"
#include "libxml/xmlsave.h"
#include <cstdlib>
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace langbind {

class DocumentWriter
{
public:
	DocumentWriter(){}

	DocumentWriter( const DocumentWriter& o)
		:m_writerbuf(o.m_writerbuf)
		,m_writer(o.m_writer){}

	bool init( const char* encoding, bool standalone);

	DocumentWriter( const char* encoding, const char* doctype, const char* publicid, const char* systemid);

	DocumentWriter( const char* encoding)
	{
		init( encoding, true);
	}

	xmlTextWriterPtr get() const
	{
		return m_writer.get();
	}

	const std::string getContent()
	{
		return std::string(
			(const char*)xmlBufferContent( m_writerbuf.get()),
			xmlBufferLength( m_writerbuf.get()));

	}

private:
	boost::shared_ptr<xmlBuffer> m_writerbuf;
	boost::shared_ptr<xmlTextWriter> m_writer;
};

}}
#endif

