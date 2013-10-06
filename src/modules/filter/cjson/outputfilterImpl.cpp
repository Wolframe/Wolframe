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

using namespace _Wolframe;
using namespace _Wolframe::langbind;

void OutputFilterImpl::closeElement()
{
	if (m_stk.size() == 1)
	{
		throw std::runtime_error( "tags not balanced in input");
	}
	std::string name = m_stk.back().m_name;
	cJSON* val = m_stk.back().m_node;
	m_stk.back().m_node = 0;
	m_stk.pop_back();

	if (m_stk.back().m_node->type == cJSON_Array)
	{
		cJSON_AddItemToArray( m_stk.back().m_node, val);
	}
	else if (m_stk.back().m_node->type != cJSON_Object)
	{
		if (!cJSON_AddItemToObject(m_stk.back().m_node,name.c_str(),val))
		{
			cJSON_Delete( val);
			throw std::bad_alloc();
		}
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
		if (!cJSON_AddItemToObject(m_stk.back().m_node,name.c_str(),val))
		{
			cJSON_Delete( val);
			cJSON_Delete( contentval);
			throw std::bad_alloc();
		}
		if (!cJSON_AddItemToObject(m_stk.back().m_node,"",contentval))
		{
			cJSON_Delete( contentval);
			throw std::bad_alloc();
		}
	}
}

void OutputFilterImpl::addStructItem( const std::string name, const std::string& value)
{
	if (!m_stk.back().m_node)
	{
		m_stk.back().m_node = cJSON_CreateObject();
		if (!m_stk.back().m_node) throw std::bad_alloc();
	}
	else if (m_stk.back().m_node->type == cJSON_Array)
	{
		throw std::runtime_error( "mixing structure with array content in output");
	}
	else if (m_stk.back().m_node->type != cJSON_Object)
	{
		throw std::runtime_error( "try to add item to non structure");
	}
	cJSON* val = cJSON_CreateString( value.c_str());
	if (!val) throw std::bad_alloc();

	if (!cJSON_AddItemToObject(m_stk.back().m_node,name.c_str(),val))
	{
		cJSON_Delete( val);
		throw std::bad_alloc();
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
		if (!cJSON_AddItemToObject( m_stk.back().m_node,"",val))
		{
			cJSON_Delete( val);
			throw std::bad_alloc();
		}
	}
	else
	{
		throw std::runtime_error( "try to add content value to non structure or array");
	}
}

bool OutputFilterImpl::print( ElementType type, const void* element, std::size_t elementsize)
{
	switch (type)
	{
		case OutputFilter::OpenTag:
			if (!m_headerPrinted)
			{
				if (!m_doctype.empty()) addStructItem( "doctype", m_doctype);
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
				addStructItem( m_attribname, std::string( (const char*)element, elementsize));
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
	return true;
}

void OutputFilterImpl::setDocType( const std::string& value)
{
	if (m_headerPrinted)
	{
		throw std::runtime_error( "cannot set doctype anymore after elements printed");
	}
	types::DocType doctype( value);
	if (doctype.rootid)
	{
		m_doctype = doctype.systemid;
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

