/************************************************************************
Copyright (C) 2011 Project Wolframe.
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
///\file serialize_ddl_filtermapSerialize.cpp

#include "serialize/ddl/filtermapDDLSerialize.hpp"
#include "filter/typedfilter.hpp"
#include <cstring>
#include <sstream>

using namespace _Wolframe;
using namespace serialize;

// forward declaration
static bool fetchObject( Context& ctx, std::vector<FiltermapDDLSerializeState>& stk);

static std::string getPrintPath( const FiltermapDDLSerializeStateStack& stk)
{
	std::string rt;
	FiltermapDDLSerializeStateStack::const_iterator itr=stk.begin(), end=stk.end();
	for (; itr != end; ++itr)
	{
		if (itr->value() && itr->value()->contentType() != ddl::StructType::Vector)
		{
			std::string tag = itr->tag().tostring();
			if (tag.size())
			{
				rt.append( "/");
				rt.append( tag);
			}
		}
	}
	return rt;
}

static bool fetchAtom( Context& ctx, std::vector<FiltermapDDLSerializeState>& stk)
{
	const ddl::AtomicType* val = &stk.back().value()->value();
	std::string ee;
	if (!val->get( ee))
	{
		ctx.setError( "value conversion error");
		return false;
	}
	ctx.setElem( langbind::FilterBase::Value, langbind::TypedFilterBase::Element( ee.c_str(), ee.size()));
	stk.pop_back();
	return true;
}

static bool fetchStruct( Context& ctx, std::vector<FiltermapDDLSerializeState>& stk)
{
	bool rt = false;
	const ddl::StructType* obj = (const ddl::StructType*)stk.back().value();
	std::size_t idx = stk.back().state();
	if (idx < obj->nof_elements())
	{
		ddl::StructType::Map::const_iterator itr = obj->begin() + idx;
		if (idx < obj->nof_attributes())
		{
			if (itr->second.contentType() != ddl::StructType::Atomic)
			{
				ctx.setError( "atomic value expected for attribute");
				return false;
			}
			langbind::TypedFilterBase::Element elem( itr->first.c_str(), itr->first.size());
			ctx.setElem( langbind::FilterBase::Attribute, elem);
			stk.push_back( FiltermapDDLSerializeState( &itr->second, elem));
			stk.back().state( ++idx);
			rt = true;
		}
		else
		{
			langbind::TypedFilterBase::Element elem( itr->first.c_str(), itr->first.size());
			ctx.setElem( langbind::FilterBase::OpenTag, elem);
			stk.back().state( ++idx);
			stk.push_back( FiltermapDDLSerializeState( langbind::FilterBase::CloseTag, elem));
			stk.push_back( FiltermapDDLSerializeState( &itr->second, elem));
			rt = true;
		}
	}
	else
	{
		stk.pop_back();
	}
	return rt;
}

static bool fetchVector( Context& ctx, std::vector<FiltermapDDLSerializeState>& stk)
{
	bool rt = false;
	const ddl::StructType* obj = (const ddl::StructType*)stk.back().value();
	std::size_t idx = stk.back().state();
	if (idx >= obj->nof_elements())
	{
		stk.pop_back();
		return true;
	}
	ddl::StructType::Map::const_iterator itr = obj->begin()+idx;
	if (idx >= 1)
	{
		ctx.setElem( langbind::FilterBase::CloseTag, stk.back().tag());
		rt = true;
	}
	stk.back().state( idx+1);
	stk.push_back( FiltermapDDLSerializeState( &itr->second, stk.back().tag()));	//... print element
	if (idx >= 1)
	{
		stk.push_back( FiltermapDDLSerializeState( langbind::FilterBase::OpenTag, stk.back().tag()));
	}
	return rt;
}

static bool fetchObject( Context& ctx, std::vector<FiltermapDDLSerializeState>& stk)
{
	if (!stk.back().value())
	{
		ctx.setElem( stk.back().type(), stk.back().tag());
		stk.pop_back();
		return true;
	}
	switch (stk.back().value()->contentType())
	{
		case ddl::StructType::Atomic:
		{
			return fetchAtom( ctx, stk);
		}
		case ddl::StructType::Vector:
		{
			return fetchVector( ctx, stk);
		}
		case ddl::StructType::Struct:
		{
			return fetchStruct( ctx, stk);
		}
	}
	ctx.setError( "illegal state in print DDL form");
	return false;
}


DDLStructSerializer::CallResult DDLStructSerializer::print( langbind::TypedOutputFilter& out)
{
	try
	{
		while (m_stk.size())
		{
			Context::ElementBuffer elem;
			if (m_ctx.getElem( elem))
			{
				if (!out.print( elem.m_type, elem.m_value))
				{
					if (out.getError())
					{
						m_ctx.setError( out.getError());
						std::string path = getPrintPath( m_stk);
						m_ctx.setTag( path.c_str());
						m_ctx.setElem( elem);
						return Error;
					}
					m_ctx.setElem( elem);
					return Yield;
				}
			}
			if (!fetchObject( m_ctx, m_stk))
			{
				if (m_ctx.getLastError())
				{
					std::string path = getPrintPath( m_stk);
					m_ctx.setTag( path.c_str());
					return Error;
				}
			}
		}
	}
	catch (std::exception& e)
	{
		m_ctx.setError( e.what());
		return Error;
	}
	return Ok;
}


bool DDLStructSerializer::getNext( langbind::FilterBase::ElementType& type, langbind::TypedFilterBase::Element& value)
{
	try
	{
		if (!m_stk.size()) return false;

		Context::ElementBuffer elem;
		while (!m_ctx.getElem( elem))
		{
			if (!fetchObject( m_ctx, m_stk))
			{
				if (m_ctx.getLastError())
				{
					std::string path = getPrintPath( m_stk);
					m_ctx.setTag( path.c_str());
					return false;
				}
			}
		}
		type = elem.m_type;
		value = elem.m_value;
	}
	catch (std::exception& e)
	{
		m_ctx.setError( e.what());
		return false;
	}
	return true;
}

