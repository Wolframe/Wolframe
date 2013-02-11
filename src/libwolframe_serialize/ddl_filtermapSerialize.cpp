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
///\file serialize_ddl_filtermapSerialize.cpp

#include "serialize/ddl/filtermapDDLSerialize.hpp"
#include "filter/typedfilter.hpp"
#include <cstring>
#include <sstream>

using namespace _Wolframe;
using namespace serialize;

// forward declaration
static bool fetchObject( Context& ctx, std::vector<FiltermapDDLSerializeState>& stk);

static std::string getElementPath( const FiltermapDDLSerializeStateStack& stk)
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
	bool rt = false;
	if (stk.back().state())
	{
		stk.pop_back();
		return false;
	}
	if (stk.back().value()->initialized())
	{
		const ddl::AtomicType* val = &stk.back().value()->value();
		ctx.setElem( langbind::FilterBase::Value, langbind::TypedFilterBase::Element( val->value().c_str(), val->value().size()));
		rt = true;
	}
	stk.back().state( 1);
	return rt;
}

static bool fetchStruct( Context& ctx, std::vector<FiltermapDDLSerializeState>& stk)
{
	bool rt = false;
	const ddl::StructType* obj = (const ddl::StructType*)stk.back().value();
	std::size_t idx = stk.back().state();
	if (idx < obj->nof_elements())
	{
		ddl::StructType::Map::const_iterator itr = obj->begin() + idx;
		if (!itr->second.initialized())
		{
			stk.back().state( ++idx);
			return false;
		}
		if (idx < obj->nof_attributes())
		{
			if (itr->first.empty())
			{
				throw SerializationErrorException( "error in structure definition: defined untagged value as attribute in structure", getElementPath( stk));
			}
			if (itr->second.contentType() != ddl::StructType::Atomic)
			{
				throw SerializationErrorException( "named atomic value expected for attribute", getElementPath( stk));
			}
			langbind::TypedFilterBase::Element elem( itr->first.c_str(), itr->first.size());
			ctx.setElem( langbind::FilterBase::Attribute, elem);
			rt = true;
			stk.back().state( ++idx);
			stk.push_back( FiltermapDDLSerializeState( &itr->second, elem));
		}
		else
		{
			if (itr->first.empty())
			{
				stk.back().state( ++idx);
				langbind::TypedFilterBase::Element tag;
				stk.push_back( FiltermapDDLSerializeState( &itr->second, tag));
			}
			else
			{
				if (itr->second.contentType() == ddl::StructType::Vector && !ctx.flag( Context::SerializeWithIndices))
				{
					langbind::TypedFilterBase::Element elem( itr->first.c_str(), itr->first.size());
					stk.back().state( ++idx);
					stk.push_back( FiltermapDDLSerializeState( &itr->second, elem));
				}
				else if (itr->first.empty())
				{
					stk.back().state( ++idx);
					stk.push_back( FiltermapDDLSerializeState( &itr->second, langbind::TypedFilterBase::Element()));
				}
				else
				{
					langbind::TypedFilterBase::Element elem( itr->first.c_str(), itr->first.size());
					ctx.setElem( langbind::FilterBase::OpenTag, elem);
					rt = true;
					stk.back().state( ++idx);
					stk.push_back( FiltermapDDLSerializeState( langbind::FilterBase::CloseTag, elem));
					stk.push_back( FiltermapDDLSerializeState( &itr->second, elem));
				}
			}
		}
	}
	else
	{
		stk.pop_back();
		if (stk.size() == 0)
		{
			ctx.setElem( langbind::FilterBase::CloseTag);
			rt = true;
		}
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
		return false;
	}
	ddl::StructType::Map::const_iterator itr = obj->begin()+idx;
	if (ctx.flag( Context::SerializeWithIndices))
	{
		ctx.setElem( langbind::FilterBase::OpenTag, langbind::TypedFilterBase::Element( (unsigned int)idx+1U));
		stk.back().state( idx+1);
		stk.push_back( FiltermapDDLSerializeState( langbind::FilterBase::CloseTag, langbind::TypedFilterBase::Element( (unsigned int)idx+1U)));
		stk.push_back( FiltermapDDLSerializeState( &itr->second, stk.back().tag()));
		rt = true;
	}
	else
	{
		bool hasTag = !stk.back().tag().empty();
		if (hasTag)
		{
			ctx.setElem( langbind::FilterBase::OpenTag, stk.back().tag());
			rt = true;
			stk.back().state( idx+1);
			stk.push_back( FiltermapDDLSerializeState( langbind::FilterBase::CloseTag, stk.back().tag()));
			stk.push_back( FiltermapDDLSerializeState( &itr->second, stk.back().tag()));
		}
		else
		{
			stk.back().state( idx+1);
			stk.push_back( FiltermapDDLSerializeState( &itr->second, stk.back().tag()));
		}
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
	else
	{
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
			case ddl::StructType::Indirection:
			{
				return false;
			}
		}
	}
	return false;
}


bool DDLStructSerializer::call()
{
	if (!m_out.get()) throw std::runtime_error( "no output for serialize");
	while (m_stk.size())
	{
		Context::ElementBuffer elem;
		if (m_ctx.getElem( elem))
		{
			if (!m_out->print( elem.m_type, elem.m_value))
			{
				if (m_out->getError())
				{
					throw SerializationErrorException( m_out->getError(), getElementPath( m_stk));
				}
				m_ctx.setElem( elem);
				return false;
			}
		}
		fetchObject( m_ctx, m_stk);
	}
	return true;
}


bool DDLStructSerializer::getNext( langbind::FilterBase::ElementType& type, langbind::TypedFilterBase::Element& value)
{
	Context::ElementBuffer elem;
	while (m_stk.size() && !m_ctx.getElem( elem))
	{
		fetchObject( m_ctx, m_stk);
	}
	if (!m_stk.size()) return false;

	type = elem.m_type;
	value = elem.m_value;
	setState( langbind::InputFilter::Open);
	return true;
}

DDLStructSerializer::DDLStructSerializer( const ddl::StructType* st)
	:types::TypeSignature("serialize::DDLStructSerializer", __LINE__)
	,m_st(st)
{
	if (!m_st) throw std::runtime_error( "empty form passed to serializer");
	m_stk.push_back( FiltermapDDLSerializeState( st, langbind::TypedFilterBase::Element()));
}

DDLStructSerializer::DDLStructSerializer( const DDLStructSerializer& o)
	:types::TypeSignature(o)
	,TypedInputFilter(o)
	,m_st(o.m_st)
	,m_ctx(o.m_ctx)
	,m_out(o.m_out)
	,m_stk(o.m_stk){}

DDLStructSerializer& DDLStructSerializer::operator =( const DDLStructSerializer& o)
{
	types::TypeSignature::operator=(o);
	TypedInputFilter::operator=(o);
	m_st = o.m_st;
	m_ctx = o.m_ctx;
	m_out = o.m_out;
	m_stk = o.m_stk;
	return *this;
}

void DDLStructSerializer::init( const langbind::TypedOutputFilterR& out, Context::Flags flags)
{
	m_ctx.clear();
	m_ctx.setFlags(flags);
	m_stk.clear();
	m_stk.push_back( FiltermapDDLSerializeState( m_st, langbind::TypedFilterBase::Element()));
	m_out = out;
}

