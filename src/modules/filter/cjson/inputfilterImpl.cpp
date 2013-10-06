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
///\file inputfilterImpl.cpp
///\brief Implementation of input filter abstraction for the cjson library
#include "inputfilterImpl.hpp"
#include "utils/parseUtils.hpp"
#include "langbind/charsetEncodings.hpp"
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace _Wolframe::langbind;

bool InputFilterImpl::getValue( const char* name, std::string& val)
{
	if (std::strcmp(name,"encoding") == 0 && m_state == Processing)
	{
		val = m_encoding;
		return true;
	}
	return false;
}

bool InputFilterImpl::getDocType( std::string& val)
{
	if (!m_doctype.empty())
	{
		val = m_doctype;
		return true;
	}
	return false;
}

bool InputFilterImpl::setValue( const char* name, const std::string& value)
{
	return Parent::setValue( name, value);
}

static std::string getCharsetEncodingName( const void* content, std::size_t contentsize)
{
	CharsetClass::Id cl = CharsetClass::guess( (const char*)content, contentsize);
	std::string rt = "UTF-";
	if ((int)cl & (CharsetClass::U1) != 0) rt += "8";
	if ((int)cl & (CharsetClass::U2) != 0) rt += "16";
	if ((int)cl & (CharsetClass::U4) != 0) rt += "32";
	if ((int)cl & (CharsetClass::BE) != 0) rt += "BE";
	if ((int)cl & (CharsetClass::LE) != 0) rt += "LE";
	return rt;
}

void InputFilterImpl::putInput( const void* content, std::size_t contentsize, bool end)
{
	m_content.append( (const char*)content, contentsize);
	if (end)
	{
		if (m_root) throw std::logic_error( "bad operation on JSON input filter: put input after end");
		m_encoding = getCharsetEncodingName( content, contentsize);
		CharsetEncoding enc = langbind::getCharsetEncoding( m_encoding);
		m_content = langbind::convertStringCharsetToUTF8( enc, m_content);
		m_state = Processing;
		cJSON_Context ctx;
		m_root = cJSON_Parse( &ctx, m_content.c_str());
		if (!m_root)
		{
			if (!ctx.errorptr) throw std::bad_alloc();
			utils::LineInfo pos = utils::getLineInfo( m_content.begin(), m_content.begin() + (ctx.errorptr - m_content.c_str()));

			std::string err( ctx.errorptr);
			if (err.size() > 80)
			{
				err.resize( 80);
				err.append( "...");
			}
			throw std::runtime_error( std::string( "error in JSON content at line ") + boost::lexical_cast<std::string>(pos.line) + " column " + boost::lexical_cast<std::string>(pos.column) + " at '" + err + "'");
		}
		m_first = m_root;
		for (;;)
		{
			if (m_first->string && m_first->valuestring)
			{
				if (boost::iequals("doctype",m_first->string))
				{
					if (!m_doctype.empty()) throw std::runtime_error("duplicate 'doctype' definition");
					m_doctype = m_first->valuestring;
					m_first = m_first->next;
				}
				else
				{
					break;
				}
			}
		}
		m_stk.push_back( m_first);
	}
}

bool InputFilterImpl::getNext( InputFilter::ElementType& type, const void*& element, std::size_t& elementsize)
{
	while (!m_stk.empty())
	{
		const cJSON* nd = m_stk.back().m_node;
		if (!nd) throw std::runtime_error( "internal: invalid node in JSON structure");

		for (;;) switch (m_stk.back().m_state)
		{
			case StackElement::StateOpen:
				if (nd->string)
				{
					type = InputFilter::OpenTag;
					element = nd->string;
					elementsize = std::strlen( nd->string);
					m_stk.back().m_state = StackElement::StateChild;
					return true;
				}
				m_stk.back().m_state = StackElement::StateChild;
				continue;

			case StackElement::StateChild:
				if (nd->child)
				{

				}
				break;

			case StackElement::StateValue:
				break;
		}
	}
	return false;
}

bool InputFilterImpl::setFlags( Flags f)
{
	if (0!=((int)f & (int)langbind::FilterBase::SerializeWithIndices))
	{
		return false;
	}
	return InputFilter::setFlags( f);
}


