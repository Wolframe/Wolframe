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

bool OutputFilterImpl::flushBuffer()
{
	bool rt = true;
	// if we have the whole document, then we start to print it and return an error, as long as we still have data:
	if (m_elemitr < m_elembuf.size())
	{
		m_elemitr += write( m_elembuf.c_str() + m_elemitr, m_elembuf.size() - m_elemitr);
		if (m_elemitr == m_elembuf.size())
		{
			setState( OutputFilter::Open);
			rt = true;
		}
		else
		{
			setState( OutputFilter::EndOfBuffer);
			rt = false;
		}
	}
	else
	{
		setState( OutputFilter::Open);
	}
	return rt;
}

void OutputFilterImpl::printStructToBuffer()
{
	if (m_stk.size() != 1) throw std::logic_error( "internal: illegal call of 'printStructToBuffer()'");

	char* content = cJSON_Print( m_stk.back().m_node);
	if (!content) throw std::bad_alloc();

	boost::shared_ptr<char> contentref( content, std::free);

	setEncoding();
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
	m_stk.clear();
	m_stk.push_back( StackElement(""));
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

void OutputFilterImpl::addStructItem( const std::string name, cJSON* val)
{
	cJSON* item = cJSON_GetObjectItem( m_stk.back().m_node, name.c_str());
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
			item = cJSON_DetachItemFromObject( m_stk.back().m_node, name.c_str());
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

			if (!cJSON_AddItemToObject( m_stk.back().m_node, name.c_str(), ar))
			{
				cJSON_Delete( ar);
				throw std::bad_alloc();
			}
		}
	}
	else
	{
		if (!cJSON_AddItemToObject( m_stk.back().m_node, name.c_str(), val))
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
		if (!m_stk.empty()) m_stk.pop_back();
	}
	else
	{
		std::string name = m_stk.back().m_name;
		cJSON* val = m_stk.back().m_node;
		m_stk.back().m_node = 0;
		m_stk.pop_back();

		if (val)
		{
			if (!m_stk.back().m_node)
			{
				if (name.empty())
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
				addStructItem( name, val);
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
					if (!cJSON_AddItemToObject(m_stk.back().m_node,name.c_str(),val))
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
		if (m_stk.size() == 1)
		{
			// close of a root element -> we print the document content to output
			printStructToBuffer();
		}
	}
}

void OutputFilterImpl::addStructValue( const std::string name, const std::string& value)
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

		addStructItem( name, val);
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
	if (m_stk.size() == 1)
	{
		// set of a root element -> we print the document content to output
		printStructToBuffer();
	}
}

bool OutputFilterImpl::print( ElementType type, const void* element, std::size_t elementsize)
{
	if (!flushBuffer()) return false;
	if (m_stk.empty())
	{
		setState( Error, "cjson filter illegal operation: printing after final close");
	}
	switch (type)
	{
		case OutputFilter::OpenTag:
			if (!m_headerPrinted)
			{
				if (!m_doctypeid.empty()) addStructValue( "doctype", m_doctypeid);
				m_headerPrinted = true;
			}
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
	return flushBuffer();
}

void OutputFilterImpl::setDocType( const std::string& value)
{
	if (m_headerPrinted)
	{
		throw std::runtime_error( "cannot set doctype anymore after elements printed");
	}
	types::DocType doctype( value);
	m_doctypeid = doctype.systemid;
}

void OutputFilterImpl::setEncoding()
{
	if (!m_encattr_defined)
	{
		if (attributes())
		{
			const char* encstr = attributes()->getEncoding();
			if (encstr)
			{
				m_encattr = types::String::getEncodingFromName( encstr);
			}
			m_encattr_defined = true;
		}
	}
}

bool OutputFilterImpl::getValue( const char* name, std::string& val)
{
	if (std::strcmp( name, "encoding") == 0)
	{
		if (m_encattr_defined)
		{
			val = types::String::encodingName( m_encattr.encoding, m_encattr.codepage);
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
		if (m_headerPrinted)
		{
			setState( Error, "setting of the encoding not allowed after first print operation");
			return false;
		}
		m_encattr = types::String::getEncodingFromName( value);
		return true;
	}
	return Parent::setValue( name, value);
}

