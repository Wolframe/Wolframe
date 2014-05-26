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
#include "utils/sourceLineInfo.hpp"
#include "types/string.hpp"
#include "logger-v1.hpp"
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace _Wolframe::langbind;

void InputFilterImpl::getRest( const void*& ptr, std::size_t& size, bool& end)
{
	if (m_root.get())
	{
		if (m_stk.size() == 1
			&& m_stk.back().m_node == m_firstnode
			&& m_stk.back().m_state == StackElement::StateOpen)
		{
			//... initial state, no element fetched yet
			ptr = (const void*)m_content.c_str();
			size = m_content.size();
		}
		else
		{
			ptr = 0;
			size = 0;
		}
		end = true;
	}
	else
	{
		ptr = (const void*)m_content.c_str();
		size = m_content.size();
		end = false;
	}
}

bool InputFilterImpl::getValue( const char* id, std::string& val) const
{
	return Parent::getValue( id, val);
}

const types::DocMetaData* InputFilterImpl::getMetaData()
{
	if (!m_root.get())
	{
		setState( EndOfMessage);
		return 0;
	}
	return getMetaDataRef().get();
}

bool InputFilterImpl::setValue( const char* id, const std::string& value)
{
	return Parent::setValue( id, value);
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
	if (m_encattr.encoding == types::String::UTF8)
	{
		m_content = content;
	}
	else
	{
		m_content = types::StringConst( content.c_str(), content.size(), m_encattr.encoding, m_encattr.codepage).tostring();
	}
	cJSON_Context ctx;
	cJSON* pp = cJSON_Parse( &ctx, m_content.c_str());
	if (!pp)
	{
		if (!ctx.errorptr) throw std::bad_alloc();
		utils::SourceLineInfo pos = utils::getSourceLineInfo( m_content.begin(), m_content.begin() + (ctx.errorptr - m_content.c_str()));

		std::string err( ctx.errorptr);
		if (err.size() > 80)
		{
			err.resize( 80);
			err.append( "...");
		}
		throw std::runtime_error( std::string( "error in JSON content at line ") + boost::lexical_cast<std::string>(pos.line()) + " column " + boost::lexical_cast<std::string>(pos.column()) + " at '" + err + "'");
	}
	return boost::shared_ptr<cJSON>( pp, cJSON_Delete);
}

void InputFilterImpl::putInput( const void* content, std::size_t contentsize, bool end)
{
	try
	{
		setState( Start);
		m_content.append( (const char*)content, contentsize);
		if (end)
		{
			std::string origcontent( m_content);
			if (m_root.get()) throw std::logic_error( "bad operation on JSON input filter: put input after end");
			m_encattr.encoding = guessCharsetEncoding( m_content.c_str(), m_content.size());
			m_encattr.codepage = 0;
			if (m_encattr.encoding != types::String::UTF8)
			{
				setAttribute( "encoding", m_encattr.encodingName());
			}
			m_root = parse( origcontent);
			m_firstnode = m_root.get();
			int nof_docattributes = 0;
			bool encodingParsed = false;
			bool doctypeParsed = false;
	
			if (!m_firstnode->string && !m_firstnode->valuestring && !m_firstnode->next && m_firstnode->type == cJSON_Object)
			{
				//CJSON creates a toplevel object for multiple root nodes:
				m_firstnode = m_firstnode->child;
			}

			for (;;)
			{
				if (m_firstnode->string && m_firstnode->valuestring)
				{
					if (boost::iequals("-doctype",m_firstnode->string))
					{
						++nof_docattributes;
						if (doctypeParsed) throw std::runtime_error("duplicate 'doctype' definition");
						setDoctype( m_firstnode->valuestring);
						m_firstnode = m_firstnode->next;
						continue;
					}
					else if (boost::iequals("-encoding", m_firstnode->string))
					{
						++nof_docattributes;
						types::String::EncodingAttrib ea = types::String::getEncodingFromName( m_firstnode->valuestring);
						setAttribute( "encoding", m_firstnode->valuestring);

						if (m_encattr.encoding != ea.encoding || ea.codepage != 0)
						{
							// ... encoding different than guessed. Parse again
							if (encodingParsed) throw std::runtime_error( "duplicate 'encoding' definition");
							encodingParsed = true;
							m_encattr = ea;
							m_root = parse( origcontent);
							m_firstnode = m_root.get();
							while (m_firstnode && nof_docattributes--) m_firstnode = m_firstnode->next;
						}
						else
						{
							m_firstnode = m_firstnode->next;
						}
						continue;
					}
				}
				break;
			}
			m_stk.push_back( StackElement( m_firstnode));
			LOG_DEBUG << "[cjson input] document meta data: {" << getMetaDataRef()->tostring() << "}";
			setState( Open);
		}
	}
	catch (const std::runtime_error& err)
	{
		setState( InputFilter::Error, err.what());
		return;
	}
	catch (const std::bad_alloc& err)
	{
		setState( InputFilter::Error, "out of memory");
		return;
	}
	catch (const std::logic_error& err)
	{
		LOG_FATAL << "logic error in JSON filer: " << err.what();
		setState( InputFilter::Error, "logic error in libxml2 filer. See logs");
		return;
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
		if (state() != Error)
		{
			setState( EndOfMessage);
		}
		return false;
	}
	while (!m_stk.empty())
	{
		const cJSON* nd = m_stk.back().m_node;
		if (!nd && m_stk.back().m_state != StackElement::StateCloseNode && m_stk.back().m_state != StackElement::StateCheckEnd)
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
						LOG_DATA << "[json input filter] get next " << FilterBase::elementTypeName( type) << " '" << std::string( (const char*)element, elementsize) << "'";
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

							if (flag( FilterBase::SerializeWithIndices))
							{
								type = InputFilter::OpenTag;
								element = "";
								elementsize = 0;
								LOG_DATA << "[json input filter] get next " << FilterBase::elementTypeName( type) << " '" << std::string( (const char*)element, elementsize) << "'";
								return true;
							}
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
						LOG_DATA << "[json input filter] get next " << FilterBase::elementTypeName( type) << " '" << std::string( (const char*)element, elementsize) << "'";
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
						LOG_DATA << "[json input filter] get next " << FilterBase::elementTypeName( type) << " '" << std::string( (const char*)element, elementsize) << "'";
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
						LOG_DATA << "[json input filter] get next " << FilterBase::elementTypeName( type) << " '" << std::string( (const char*)element, elementsize) << "'";
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
							LOG_DATA << "[json input filter] get next " << FilterBase::elementTypeName( type) << " '" << std::string( (const char*)element, elementsize) << "'";
							return true;
						}
						else
						{
							m_stk.back().m_state = StackElement::StateOpen;
						}
						continue;
					}
					else
					{
						if (m_stk.back().m_tag && flag( FilterBase::SerializeWithIndices))
						{
							type = InputFilter::CloseTag;
							element = 0;
							elementsize = 0;
							m_stk.back().m_state = StackElement::StateCloseNode;
							LOG_DATA << "[json input filter] get next " << FilterBase::elementTypeName( type) << " '" << std::string( (const char*)element, elementsize) << "'";
							return true;
						}
						m_stk.back().m_state = StackElement::StateCloseNode;
						continue;
					}

				case StackElement::StateCloseNode:
					m_stk.pop_back();
					if (m_stk.empty())
					{
						// final close:
						type = InputFilter::CloseTag;
						element = 0;
						elementsize = 0;
						LOG_DATA << "[json input filter] get next " << FilterBase::elementTypeName( type) << " '" << std::string( (const char*)element, elementsize) << "'";
						return true;
					}
					nd = m_stk.back().m_node;
					continue;

				case StackElement::StateReopen:
					if (flag( FilterBase::SerializeWithIndices))
					{
						type = InputFilter::OpenTag;
						element = 0;
						elementsize = 0;
					}
					else
					{
						type = InputFilter::OpenTag;
						element = m_stk.back().m_tag;
						elementsize = std::strlen( m_stk.back().m_tag);
					}
					m_stk.back().m_state = StackElement::StateOpen;
					LOG_DATA << "[json input filter] get next " << FilterBase::elementTypeName( type) << " '" << std::string( (const char*)element, elementsize) << "'";
					return true;
			}
		}
	}
	return false;
}

bool InputFilterImpl::checkSetFlags( Flags) const
{
	return true;
}

bool InputFilterImpl::setFlags( Flags f)
{
	return FilterBase::setFlags( f);
}

