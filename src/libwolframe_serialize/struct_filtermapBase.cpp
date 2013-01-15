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
///\file serialize/struct/filtermapBase.cpp
///\brief Implements the non intrusive base class of serialization/deserialization
#include "serialize/struct/filtermapParseStack.hpp"
#include "serialize/struct/filtermapBase.hpp"
#include "filter/typingfilter.hpp"
#include <cstring>
#include <stdexcept>

using namespace _Wolframe;
using namespace serialize;

StructParser::StructParser( const ObjectReference& obj, const StructDescriptionBase* descr_)
	:m_ptr(obj.get())
	,m_obj(obj)
	,m_descr(descr_)
{
	FiltermapParseState st( 0, m_descr->parse(), m_ptr);
	m_stk.push_back( st);
}

StructParser::StructParser( void* obj, const StructDescriptionBase* descr_)
	:m_ptr(obj)
	,m_descr(descr_)
{}

StructParser::StructParser( const StructParser& o)
	:m_ptr(o.m_ptr)
	,m_obj(o.m_obj)
	,m_descr(o.m_descr)
	,m_ctx(o.m_ctx)
	,m_inp(o.m_inp)
	,m_stk(o.m_stk)
{}

std::string StructParser::getElementPath( const FiltermapParseStateStack& stk)
{
	std::string rt;
	FiltermapParseStateStack::const_iterator itr=stk.begin(), end=stk.end();
	for (; itr != end; ++itr)
	{
		if (itr->name())
		{
			rt.append( "/");
			rt.append( itr->name());
		}
	}
	return rt;
}

void StructParser::init( const langbind::TypedInputFilterR& i, Context::Flags flags)
{
	m_inp = i;
	m_ctx.clear();
	m_ctx.setFlags(flags);
	m_stk.clear();
	m_stk.push_back( FiltermapParseState( 0, m_descr->parse(), m_ptr));
}

bool StructParser::call()
{
	bool rt = true;
	if (!m_ptr) throw std::runtime_error( "try to fill null object");
	if (!m_inp.get()) throw std::runtime_error( "no input for parse");

	while (rt && m_stk.size())
	{
		rt = m_stk.back().parse()( *m_inp, m_ctx, m_stk);
	}
	return rt;
}



StructSerializer::StructSerializer( const ObjectReference& obj, const StructDescriptionBase* descr_)
	:types::TypeSignature("serialize::StructSerializer", __LINE__)
	,m_ptr(obj.get())
	,m_obj(obj)
	,m_descr(descr_)
	,m_ctx()
{
	m_stk.push_back( FiltermapSerializeState( 0, m_descr->fetch(), m_ptr));
}

StructSerializer::StructSerializer( const void* obj, const StructDescriptionBase* descr_)
	:types::TypeSignature("serialize::StructSerializer", __LINE__)
	,m_ptr(obj)
	,m_descr(descr_)
	,m_ctx()
{
	m_stk.push_back( FiltermapSerializeState( 0, m_descr->fetch(), m_ptr));
}

StructSerializer::StructSerializer( const StructSerializer& o)
	:types::TypeSignature(o)
	,TypedInputFilter(o)
	,m_ptr(o.m_ptr)
	,m_obj(o.m_obj)
	,m_descr(o.m_descr)
	,m_ctx(o.m_ctx)
	,m_out(o.m_out)
	,m_stk(o.m_stk){}

void StructSerializer::init( const langbind::TypedOutputFilterR& out, Context::Flags flags)
{
	m_ctx.clear();
	m_ctx.setFlags(flags);
	m_stk.clear();
	m_stk.push_back( FiltermapSerializeState( 0, m_descr->fetch(), m_ptr));
	m_out = out;
}

void StructSerializer::reset()
{
	m_ctx.clear();
	m_stk.clear();
	m_stk.push_back( FiltermapSerializeState( 0, m_descr->fetch(), m_ptr));
}

langbind::TypedInputFilter* StructSerializer::copy() const
{
	return new StructSerializer(*this);
}

std::string StructSerializer::getElementPath( const FiltermapSerializeStateStack& stk)
{
	std::string rt;
	FiltermapSerializeStateStack::const_iterator itr=stk.begin(), end=stk.end();
	for (; itr != end; ++itr)
	{
		if (itr->name())
		{
			rt.append( "/");
			rt.append( itr->name());
		}
	}
	return rt;
}

bool StructSerializer::call()
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
		m_stk.back().fetch()( m_ctx, m_stk);
	}
	return true;
}

bool StructSerializer::getNext( langbind::FilterBase::ElementType& type, langbind::TypedFilterBase::Element& value)
{
	Context::ElementBuffer elem;
	while (m_stk.size() && !m_ctx.getElem( elem))
	{
		m_stk.back().fetch()( m_ctx, m_stk);
	}
	if (!m_stk.size()) return false;

	type = elem.m_type;
	value = elem.m_value;
	setState( langbind::InputFilter::Open);
	return true;
}


class OneElementTypedInputFilter
	:public langbind::TypedInputFilter
{
public:
	OneElementTypedInputFilter( const std::string& value)
		:types::TypeSignature("serialize::OneElementTypedInputFilter", __LINE__)
		,m_value(value)
		,m_consumed(false){}
	OneElementTypedInputFilter( const OneElementTypedInputFilter& o)
		:types::TypeSignature(o)
		,langbind::TypedInputFilter(o)
		,m_value(o.m_value)
		,m_consumed(o.m_consumed){}

	virtual TypedInputFilter* copy() const	{return new OneElementTypedInputFilter(*this);}

	virtual bool getNext( ElementType& type, Element& element)
	{
		if (m_consumed) return false;
		type = TypedInputFilter::Value;
		element = m_value;
		m_consumed = true;
		return true;
	}
private:
	std::string m_value;
	bool m_consumed;
};


bool StructDescriptionBase::setAtomicValue( void* obj, std::size_t idx, const std::string& value) const
{
	if (idx >= nof_elements()) throw std::runtime_error( "structure element index out of range");

	Map::const_iterator itr = begin() + idx;
	void* objelemptr = (char*)obj + itr->second.ofs();
	if (itr->second.type() != StructDescriptionBase::Atomic)
	{
		throw std::runtime_error( "atomic value expected for call of set atomic value");
	}
	Context ctx;
	FiltermapParseStateStack stk;
	OneElementTypedInputFilter inp( value);
	stk.push_back( FiltermapParseState( itr->first.c_str(), itr->second.parse(), objelemptr));
	return stk.back().parse()( inp, ctx, stk);
}

