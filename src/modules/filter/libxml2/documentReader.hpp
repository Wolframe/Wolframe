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
///\file documentReader.hpp
///\brief Document reader abstraction for the libxml2 library

#ifndef _Wolframe_LIBXML2_DOCUMENT_READER_HPP_INCLUDED
#define _Wolframe_LIBXML2_DOCUMENT_READER_HPP_INCLUDED
#include "libxml/parser.h"
#include "libxml/tree.h"
#include "libxml/encoding.h"
#include "libxml/xmlwriter.h"
#include "libxml/xmlsave.h"
#include <cstdlib>
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace langbind {

class DocumentReader
{
public:
	DocumentReader(){}

	DocumentReader( xmlDocPtr doc);
	DocumentReader( const DocumentReader& o)
		:m_ptr(o.m_ptr){}

	DocumentReader( const char* content, std::size_t contentsize);

	xmlDocPtr get() const
	{
		return m_ptr.get();
	}

	std::string getContent() const;

private:
	boost::shared_ptr<xmlDoc> m_ptr;
};

}}
#endif

