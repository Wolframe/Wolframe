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
///\file inputfilterImpl.cpp
///\brief Implementation of input filter abstraction for the cjson library
#include "inputfilterImpl.hpp"
#include "utils/parseUtils.hpp"
#include "types/string.hpp"
#include "logger-v1.hpp"
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace _Wolframe::langbind;

bool InputFilterImpl::getValue( const char* name, std::string& val)
{
	if (std::strcmp(name,"encoding") == 0 && m_encattr_defined)
	{
		val = types::String::encodingName( m_encattr.encoding, m_encattr.codepage);
		return true;
	}
	return false;
}

bool InputFilterImpl::getDocType( types::DocType& doctype)
{
	if (!m_root.get())
	{
		setState( EndOfMessage);
		return false;
	}
	doctype = m_doctype;
	return true;
}

bool InputFilterImpl::getMetadata()
{
	if (m_root.get())
	{
		return true;
	}
	else
	{
		setState( EndOfMessage);
		return false;
	}
}

const char* InputFilterImpl::getEncoding() const
{
	if (!m_encattr_defined) return 0;
	return types::String::encodingName( m_encattr.encoding, m_encattr.codepage);
}

bool InputFilterImpl::setValue( const char* name, const std::string& value)
{
	return Parent::setValue( name, value);
}

static types::String::Encoding guessCharsetEncoding( const void* content, std::size_t contentsize)
{
	types::String::EncodingClass::Id cl = types::String::guessEncoding( (const char*)content, contentsize);
	switch (cl)
	{
		case types::String::EncodingClass::FAIL:
			throw std::runtime_error( "failed to guess charset encoding for JSON filter");
		case types::String::EncodingClass::NONE:
			throw std::runtime_error( "cannot guess charset encoding for JSON filter");
		case types::String::EncodingClass::UCS1:
			return types::String::UTF8;
		case types::String::EncodingClass::UCS2BE:
			return types::String::UTF16BE;
		case types::String::EncodingClass::UCS2LE:
			return types::String::UTF16LE;
		case types::String::EncodingClass::UCS4BE:
			return types::String::UCS4BE;
		case types::String::EncodingClass::UCS4LE:
			return types::String::UCS4LE;
	}
	throw std::runtime_error( "cannot guess charset encoding for JSON filter");
}

boost::shared_ptr<cJSON> InputFilterImpl::parse( const std::string& content)
{
	if (!m_encattr_defined || m_encattr.encoding == types::String::UTF8)
	{
		m_content = content;
	}
	else
	{
		m_content = types::StringConst( content.c_str(), content.size(), m_encattr.encoding, m_encattr.codepage)
				.tostring();
	}
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
		m_encattr.encoding = guessCharsetEncoding( m_content.c_str(), m_content.size());
		m_encattr.codepage = 0;
		m_encattr_defined = true;
		m_root = parse( origcontent);

		const cJSON* first = m_root.get();
		int nof_docattributes = 0;
		bool encodingParsed = false;

		if (!first->string && !first->valuestring && !first->next && first->type == cJSON_Object)
		{
			//CJSON creates a toplevel object for multiple root nodes:
			first = first->child;
		}
		const char* rootelem = 0;
		const char* doctypeid = 0;
		
		for (;;)
		{
			if (first->string && first->valuestring)
			{
				if (boost::iequals("doctype",first->string))
				{
					++nof_docattributes;
					if (doctypeid) throw std::runtime_error("duplicate 'doctype' definition");
					doctypeid = first->valuestring;
					first = first->next;
					continue;
				}
				else if (boost::iequals("encoding", first->string))
				{
					++nof_docattributes;
					types::String::EncodingAttrib ea = types::String::getEncodingFromName( first->valuestring);
					if (m_encattr.encoding != ea.encoding || ea.codepage != 0)
					{
						// ... encoding different than guessed. Parse again
						if (encodingParsed) throw std::runtime_error( "duplicate 'encoding' definition");
						encodingParsed = true;
						m_encattr = ea;
						m_root = parse( origcontent);
						first = m_root.get();
						while (first && nof_docattributes--) first = first->next;
					}
					else
					{
						first = first->next;
					}
					continue;
				}
			}
			break;
		}
		if (first->string && !first->next)
		{
			rootelem = first->string;
		}
		if (doctypeid)
		{
			if (rootelem)
			{
				m_doctype = types::DocType( doctypeid, rootelem, types::DocType::SchemaPath());
			}
			else
			{
				throw std::runtime_error( "document type defined, but no singular root element");
			}
		}
		m_stk.push_back( StackElement( first));
	}
}

bool InputFilterImpl::getNodeValue( const void*& element, std::size_t& elementsize)
{
	const cJSON* nd = m_stk.back().m_node;
	switch (nd->type & 0x7F)
	{
		case cJSON_False:
			element = "false";
			elementsize = 5;
			return true;
		case cJSON_True:
			element = "true";
			elementsize = 4;
			return true;
		case cJSON_NULL:
			return false;

		case cJSON_String:
		case cJSON_Number:
			if (!nd->valuestring)
			{
				setState( Error, "internal: value node without string value found in JSON structure");
				return false;
			}
			element = nd->valuestring;
			elementsize = nd->valuestring?std::strlen( nd->valuestring):0;
			return true;
		case cJSON_Array:
			return false;
		case cJSON_Object:
			return false;
		default:
			setState( Error, "internal: memory corruption found in JSON structure");
			return false;
	}
}

bool InputFilterImpl::getNext( InputFilter::ElementType& type, const void*& element, std::size_t& elementsize)
{
	if (!m_root.get())
	{
		setState( EndOfMessage);
		return false;
	}
	while (!m_stk.empty())
	{
		const cJSON* nd = m_stk.back().m_node;
		if (!nd && m_stk.back().m_state != StackElement::StateCheckEnd)
		{
			setState( Error, "internal: invalid node in JSON structure");
			return false;
		}
		for (;;)
		{
			LOG_DATA << "[json input filter] State " << StackElement::stateName( m_stk.back().m_state);

			switch (m_stk.back().m_state)
			{
				case StackElement::StateOpen:
					if (nd->string)
					{
						if (nd->string[0] == '-')
						{
							if (nd->type == cJSON_NULL)
							{
								m_stk.back().m_state = StackElement::StateChild;
							}
							else
							{
								type = InputFilter::Attribute;
								element = nd->string+1;
								elementsize = std::strlen( nd->string+1);
								m_stk.back().m_state = StackElement::StateAttributeValue;
							}
						}
						else if (nd->string[0] == '#')
						{
							if (std::strcmp( nd->string, "#text") == 0)
							{
								m_stk.back().m_state = StackElement::StateContentValue;
								continue;
							}
							else
							{
								setState( Error, "invalid node name in JSON structure");
								return false;
							}
						}
						else
						{
							type = InputFilter::OpenTag;
							element = nd->string;
							elementsize = std::strlen( nd->string);
							m_stk.back().m_state = StackElement::StateChild;
						}
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
							nd = m_stk.back().m_node;
						}
						else
						{
							m_stk.push_back( StackElement( nd->child));
							nd = m_stk.back().m_node;
						}
					}
					else
					{
						m_stk.back().m_state = StackElement::StateValue;
					}
					continue;

				case StackElement::StateAttributeValue:
				case StackElement::StateContentValue:
					if (getNodeValue( element, elementsize))
					{
						m_stk.back().m_node = nd->next;
						m_stk.back().m_state = StackElement::StateCheckEnd;
						type = InputFilter::Value;
						return true;
					}
					else if (state() == InputFilter::Open)
					{
						m_stk.back().m_node = nd->next;
						m_stk.back().m_state = StackElement::StateCheckEnd;
						continue;
					}
					else
					{
						return false;
					}
				case StackElement::StateValue:
					if (getNodeValue( element, elementsize))
					{
						m_stk.back().m_state = StackElement::StateNext;
						type = InputFilter::Value;
						return true;
					}
					else if (state() == InputFilter::Open)
					{
						m_stk.back().m_state = StackElement::StateNext;
						continue;
					}
					else
					{
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
						nd = m_stk.back().m_node;
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


