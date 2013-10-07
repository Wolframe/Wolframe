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
	if (std::strcmp(name,"encoding") == 0 && !m_encoding.empty())
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

static std::string guessCharsetEncoding( const void* content, std::size_t contentsize)
{
	CharsetClass::Id cl = CharsetClass::guess( (const char*)content, contentsize);
	std::string rt = "UTF-";
	if (((int)cl & (CharsetClass::U1)) != 0) rt += "8";
	if (((int)cl & (CharsetClass::U2)) != 0) rt += "16";
	if (((int)cl & (CharsetClass::U4)) != 0) rt += "32";
	if (((int)cl & (CharsetClass::BE)) != 0) rt += "BE";
	if (((int)cl & (CharsetClass::LE)) != 0) rt += "LE";
	return rt;
}

boost::shared_ptr<cJSON> InputFilterImpl::parse( const std::string& content)
{
	CharsetEncoding enc = langbind::getCharsetEncoding( m_encoding);
	m_content = langbind::convertStringCharsetToUTF8( enc, content);
	cJSON_Context ctx;
	cJSON* pp = cJSON_Parse( &ctx, m_content.c_str());
	if (!pp)
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
	return boost::shared_ptr<cJSON>( pp, cJSON_Delete);
}

void InputFilterImpl::putInput( const void* content, std::size_t contentsize, bool end)
{
	m_content.append( (const char*)content, contentsize);
	if (end)
	{
		std::string origcontent( m_content);
		if (m_root.get()) throw std::logic_error( "bad operation on JSON input filter: put input after end");
		m_encoding = guessCharsetEncoding( content, contentsize);
		m_root = parse( origcontent);

		const cJSON* first = m_root.get();
		int nof_docattributes = 0;
		bool encodingParsed = false;
		for (;;)
		{
			if (first->string && first->valuestring)
			{
				if (boost::iequals("doctype",first->string))
				{
					++nof_docattributes;
					if (!m_doctype.empty()) throw std::runtime_error("duplicate 'doctype' definition");
					m_doctype = first->valuestring;
					first = first->next;
				}
				else if (boost::iequals("encoding", first->string))
				{
					++nof_docattributes;
					if (!boost::iequals( m_encoding, first->valuestring))
					{
						// ... encoding different than guessed. Parse again
						if (encodingParsed) throw std::runtime_error( "duplicate 'encoding' definition");
						encodingParsed = true;
						m_encoding = first->valuestring;
						m_root = parse( origcontent);
						first = m_root.get();
						while (first && nof_docattributes--) first = first->next;
					}
				}
				else
				{
					break;
				}
			}
		}
		m_stk.push_back( StackElement( first));
	}
}

bool InputFilterImpl::getNext( InputFilter::ElementType& type, const void*& element, std::size_t& elementsize)
{
	while (!m_stk.empty())
	{
		const cJSON* nd = m_stk.back().m_node;
		if (!nd)
		{
			setState( Error, "internal: invalid node in JSON structure");
			return false;
		}
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
				else
				{
					m_stk.back().m_state = StackElement::StateChild;
					continue;
				}
			case StackElement::StateChild:
				if (nd->child)
				{
					m_stk.back().m_state = StackElement::StateValue;
					if (nd->type == cJSON_Array)
					{
						m_stk.push_back( StackElement( nd->child, nd->string));
					}
					else
					{
						m_stk.push_back( StackElement( nd->child));
					}
				}
				continue;

			case StackElement::StateValue:
				switch (nd->type & 0x7F)
				{
					case cJSON_False:
						type = InputFilter::Value;
						element = "false";
						elementsize = std::strlen( nd->string);
						m_stk.back().m_state = StackElement::StateNext;
						return true;
					case cJSON_True:
						type = InputFilter::Value;
						element = "false";
						elementsize = std::strlen( nd->string);
						m_stk.back().m_state = StackElement::StateNext;
						return true;
					case cJSON_NULL:
						m_stk.back().m_state = StackElement::StateNext;
						break;
					case cJSON_String:
					case cJSON_Number:
						if (!nd->valuestring)
						{
							setState( Error, "internal: value node without string value found in JSON structure");
							return false;
						}
						type = InputFilter::Value;
						element = nd->valuestring;
						elementsize = nd->valuestring?std::strlen( nd->valuestring):0;
						m_stk.back().m_state = StackElement::StateNext;
						return true;
					case cJSON_Array:
						m_stk.back().m_state = StackElement::StateNext;
						break;
					case cJSON_Object:
						m_stk.back().m_state = StackElement::StateNext;
						break;
					default:
						setState( Error, "internal: memory corruption found in JSON structure");
						return false;
				}

			case StackElement::StateNext:
				if (nd->string)
				{
					// close the tag opened in 'StackElement::StateOpen':
					type = InputFilter::CloseTag;
					element = 0;
					elementsize = 0;
					m_stk.back().m_node = nd->next;
					m_stk.back().m_state = StackElement::StateCheckEnd;
					return true;
				}
				else
				{
					m_stk.back().m_node = nd->next;
					m_stk.back().m_state = StackElement::StateCheckEnd;
					break;
				}

			case StackElement::StateCheckEnd:
				if (m_stk.back().m_node)
				{
					if (m_stk.back().m_tag)
					{
						type = InputFilter::CloseTag;
						element = 0;
						elementsize = 0;
						m_stk.back().m_state = StackElement::StateReopen;
						return true;
					}
					else
					{
						m_stk.back().m_state = StackElement::StateOpen;
					}
				}
				else
				{
					m_stk.pop_back();
					if (m_stk.empty())
					{
						// final close:
						type = InputFilter::CloseTag;
						element = 0;
						elementsize = 0;
						return true;
					}
				}
				continue;

			case StackElement::StateReopen:
				type = InputFilter::OpenTag;
				element = m_stk.back().m_tag;
				elementsize = std::strlen( m_stk.back().m_tag);
				m_stk.back().m_state = StackElement::StateOpen;
				return true;
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


