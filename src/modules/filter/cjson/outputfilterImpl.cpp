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
///\file outputfilterImpl.hpp
///\brief Implementation of output filter abstraction for the cjson library
#include "outputfilterImpl.hpp"
#include <boost/algorithm/string.hpp>
#include "utils/fileUtils.hpp"
#include "logger-v1.hpp"

using namespace _Wolframe;
using namespace _Wolframe::langbind;

std::string OutputFilterImpl::elementpath() const
{
	std::string rt;
	std::vector<StackElement>::const_iterator si = m_stk.begin(), se = m_stk.end();
	for (; si != se; ++si)
	{
		if (rt.size()) rt.push_back( '/');
		rt.append( si->m_name);
	}
	return rt;
}

void OutputFilterImpl::printStructToBuffer()
{
	if (!m_stk.back().m_node)
	{
		m_elembuf.push_back( '\n');
	}
	else
	{
		char* content = cJSON_Print( m_stk.back().m_node);
		if (!content) throw std::bad_alloc();
	
		boost::shared_ptr<char> contentref( content, std::free);
	
		if (m_encattr.encoding == types::String::UTF8)
		{
			m_elembuf.append( content);
			m_elembuf.push_back( '\n');
		}
		else
		{
			std::string res( content);
			res.push_back( '\n');
			types::String convres = types::StringConst( res).translateEncoding( m_encattr.encoding, m_encattr.codepage);
			m_elembuf.append( (const char*)convres.ptr(), convres.size() * convres.elementSize());
		}
	}
}

static bool deleteNameString( cJSON*& st)
{
	// delete st->string:
	if (!st->string) return true;
	cJSON* dummy = cJSON_CreateNull();
	if (!dummy)
	{
		return false;
	}
	dummy->string = st->string;
	st->string = 0;
	cJSON_Delete( dummy);
	return true;
}

void OutputFilterImpl::addStructItem( const std::string id, cJSON* val)
{
	cJSON* item = cJSON_GetObjectItem( m_stk.back().m_node, id.c_str());
	if (item)
	{
		// ... object with this name already exists
		if (item->type == cJSON_Array)
		{
			// ... it is an array -> add val as new item
			cJSON_AddItemToArray( item, val);
		}
		else
		{
			// ... it is an object -> make an array with the 2 duplicates as elements
			item = cJSON_DetachItemFromObject( m_stk.back().m_node, id.c_str());
			if (!deleteNameString( item))
			{
				cJSON_Delete( val);
				cJSON_Delete( item);
				throw std::bad_alloc();
			}
			cJSON* ar = cJSON_CreateArray();
			if (!ar)
			{
				cJSON_Delete( val);
				cJSON_Delete( item);
				throw std::bad_alloc();
			}
			cJSON_AddItemToArray( ar, item);
			cJSON_AddItemToArray( ar, val);

			if (!cJSON_AddItemToObject( m_stk.back().m_node, id.c_str(), ar))
			{
				cJSON_Delete( ar);
				throw std::bad_alloc();
			}
		}
	}
	else
	{
		if (!cJSON_AddItemToObject( m_stk.back().m_node, id.c_str(), val))
		{
			cJSON_Delete( val);
			throw std::bad_alloc();
		}
	}
}

void OutputFilterImpl::closeElement()
{
	if (m_stk.size() <= 1)
	{
		if (m_stk.empty()) throw std::runtime_error("tags not balanced, got a close too much");
		printStructToBuffer();
		m_stk.pop_back();
	}
	else
	{
		std::string id = m_stk.back().m_name;
		cJSON* val = m_stk.back().m_node;
		m_stk.back().m_node = 0;
		m_stk.pop_back();

		if (val)
		{
			if (!m_stk.back().m_node)
			{
				if (id.empty())
				{
					m_stk.back().m_node = cJSON_CreateArray();
				}
				else
				{
					m_stk.back().m_node = cJSON_CreateObject();
				}
				if (!m_stk.back().m_node)
				{
					cJSON_Delete( val);
					throw std::bad_alloc();
				}
			}
			if (m_stk.back().m_node->type == cJSON_Array)
			{
				cJSON_AddItemToArray( m_stk.back().m_node, val);
			}
			else if (m_stk.back().m_node->type == cJSON_Object)
			{
				addStructItem( id, val);
			}
			else
			{
				cJSON* contentval = m_stk.back().m_node;
				m_stk.back().m_node = cJSON_CreateObject();
				if (!m_stk.back().m_node)
				{
					cJSON_Delete( val);
					cJSON_Delete( contentval);
					throw std::bad_alloc();
				}
				if (val)
				{
					if (!cJSON_AddItemToObject( m_stk.back().m_node, id.c_str(),val))
					{
						cJSON_Delete( val);
						cJSON_Delete( contentval);
						throw std::bad_alloc();
					}
				}
				if (!cJSON_AddItemToObject(m_stk.back().m_node,"",contentval))
				{
					cJSON_Delete( contentval);
					throw std::bad_alloc();
				}
			}
		}
	}
}

void OutputFilterImpl::addStructValue( const std::string id, const std::string& value)
{
	if (!m_stk.back().m_node)
	{
		m_stk.back().m_node = cJSON_CreateObject();
		if (!m_stk.back().m_node) throw std::bad_alloc();
	}
	if (m_stk.back().m_node->type == cJSON_Array)
	{
		throw std::runtime_error( "mixing structure with array content in output");
	}
	else if (m_stk.back().m_node->type == cJSON_Object)
	{
		cJSON* val = cJSON_CreateString( value.c_str());
		if (!val) throw std::bad_alloc();

		addStructItem( id, val);
	}
	else
	{
		throw std::runtime_error( "try to add item to non structure");
	}
}

void OutputFilterImpl::setContentValue( const std::string& value)
{
	cJSON* val = cJSON_CreateString( value.c_str());
	if (!val) throw std::bad_alloc();
	if (!m_stk.back().m_node)
	{
		m_stk.back().m_node = val;
		if (!val)
		{
			cJSON_Delete( val);
			throw std::bad_alloc();
		}
	}
	else if (m_stk.back().m_node->type == cJSON_Array)
	{
		cJSON_AddItemToArray( m_stk.back().m_node, val);
	}
	else if (m_stk.back().m_node->type == cJSON_Object)
	{
		if (!cJSON_AddItemToObject( m_stk.back().m_node, "#text" ,val))
		{
			cJSON_Delete( val);
			throw std::bad_alloc();
		}
	}
	else
	{
		cJSON* ar = cJSON_CreateArray();
		if (!ar)
		{
			throw std::bad_alloc();
		}
		cJSON_AddItemToArray( ar, m_stk.back().m_node);
		cJSON_AddItemToArray( ar, val);
		m_stk.back().m_node = ar;
	}
}

void OutputFilterImpl::printHeader()
{
	types::DocMetaData md = getMetaData();
	LOG_DEBUG << "[cjson output] document meta data: {" << md.tostring() << "}";

	const char* encname = md.getAttribute( "encoding");
	if (encname)
	{
		m_encattr = types::String::getEncodingFromName( encname);
	}

	const char* standalone = md.getAttribute( "standalone");
	if (standalone && 0==std::strcmp( standalone, "yes"))
	{
		//... explicit standalone -> no meta data except encoding
	}
	else
	{
		std::string doctype = md.doctype();
		if (doctype.empty())
		{
			const char* doctypeattr = md.getAttribute( "doctype");
			if (doctypeattr)
			{
				addStructValue( "-doctype", doctypeattr);
			}
		}
		else
		{
			addStructValue( "-doctype", doctype);
		}
	}
	if (m_encattr.codepage) addStructValue( "-encoding", encname);

	m_headerprinted = true;
	setState( Open);
}

bool OutputFilterImpl::close()
{
	if (!m_headerprinted)
	{
		printHeader();
	}
	if (!m_stk.empty())
	{
		return print( FilterBase::CloseTag, 0, 0);
	}
	return true;
}

bool OutputFilterImpl::print( ElementType type, const void* element, std::size_t elementsize)
{
	try
	{
		if (m_elemitr && m_elemitr == m_elembuf.size())
		{
			m_elembuf.clear();
			m_elemitr = 0;
		}
		if (m_stk.empty())
		{
			setState( Error, "cjson filter illegal operation: printing after final close");
		}
		if (!m_headerprinted)
		{
			printHeader();
		}
		LOG_DATA << "[json output filter] print " << FilterBase::elementTypeName( type) << " '" << std::string( (const char*)element, elementsize) << "'";
	
		switch (type)
		{
			case OutputFilter::OpenTag:
				m_stk.push_back( StackElement( std::string( (const char*)element, elementsize)));
				break;
	
			case OutputFilter::Attribute:
				if (!m_attribname.empty())
				{
					setState( Error, "cjson filter illegal operation: printing subsequent attributes");
					return false;
				}
				m_attribname.append( (const char*)element, elementsize);
				break;
	
			case OutputFilter::Value:
				if (m_attribname.empty())
				{
					setContentValue( std::string( (const char*)element, elementsize));
				}
				else
				{
					addStructValue( m_attribname, std::string( (const char*)element, elementsize));
					m_attribname.clear();
				}
				break;
	
			case OutputFilter::CloseTag:
				closeElement();
				break;
	
			default:
				setState( Error, "cjson filter: illegal state");
				return false;
		}
		m_lastelemtype = type;
		if (m_elembuf.size() > outputChunkSize())
		{
			setState( EndOfBuffer);
			return false;
		}
		return true;
	}
	catch (const std::bad_alloc& e)
	{
		setState( Error, e.what());
		return false;
	}
	catch (const std::runtime_error& e)
	{
		setState( Error, e.what());
		return false;
	}
}

void OutputFilterImpl::getOutput( const void*& buf, std::size_t& bufsize)
{
	buf = (const void*)(m_elembuf.c_str() + m_elemitr);
	bufsize = m_elembuf.size() - m_elemitr;
	m_elemitr = m_elembuf.size();
}

bool OutputFilterImpl::getValue( const char* id, std::string& val) const
{
	return Parent::getValue( id, val);
}

bool OutputFilterImpl::setValue( const char* id, const std::string& value)
{
	return Parent::setValue( id, value);
}

