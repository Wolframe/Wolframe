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
///\file outputfilterImpl.hpp
///\brief Implementation of output filter abstraction for the rapidjson library
#include "outputfilterImpl.hpp"
#include "rapidjson/document.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/reader.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

using namespace _Wolframe;
using namespace _Wolframe::langbind;

bool OutputFilterImpl::flushBuffer()
{
}

bool OutputFilterImpl::print( ElementType type, const void* element, std::size_t elementsize)
{
	switch (type)
	{
		case OutputFilter::OpenTag:
			m_attribname.clear();
			break;

		case OutputFilter::Attribute:
			if (m_attribname.size())
			{
				setState( Error, "rapidjson filter: illegal operation");
				return false;
			}
			m_attribname.clear();
			m_attribname.append( (const char*)element, elementsize);
			break;

		case OutputFilter::Value:
			if (m_attribname.empty())
			{
			}
			else
			{
				m_attribname.clear();
			}
			break;

		case OutputFilter::CloseTag:
			m_attribname.clear();
			break;

		default:
			setState( Error, "rapidjson filter: illegal state");
			return false;
	}
	return true;
}

void OutputFilterImpl::setDocType( const std::string& value)
{
	types::DocType doctype( value);
	if (doctype.rootid)
	{
		m_doctype_root = doctype.rootid;
		if (doctype.systemid) m_doctype_system = doctype.systemid;
	}
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

