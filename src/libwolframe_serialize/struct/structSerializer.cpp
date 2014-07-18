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
///\file struct/structSerializer.cpp
///\brief Implements serialization
#include "serialize/struct/structSerializer.hpp"
#include "serialize/struct/serializeStack.hpp"
#include "serializationErrorException.hpp"
#include "filter/typingfilter.hpp"
#include "utils/printFormats.hpp"
#include <cstring>
#include <stdexcept>
#include "logger-v1.hpp"

using namespace _Wolframe;
using namespace serialize;

StructSerializer::StructSerializer( const ObjectReference& obj, const StructDescriptionBase* descr_)
	:TypedInputFilter("serializer")
	,m_ptr(obj.get())
	,m_obj(obj)
	,m_descr(descr_)
	,m_ctx()
{
	m_stk.push_back( SerializeState( 0, m_descr->fetch(), m_ptr));
}

StructSerializer::StructSerializer( const void* obj, const StructDescriptionBase* descr_)
	:TypedInputFilter("serializer")
	,m_ptr(obj)
	,m_descr(descr_)
	,m_ctx()
{
	if (m_descr)
	{
		m_stk.push_back( SerializeState( 0, m_descr->fetch(), m_ptr));
	}
}

StructSerializer::StructSerializer( const StructSerializer& o)
	:TypedInputFilter(o)
	,m_ptr(o.m_ptr)
	,m_obj(o.m_obj)
	,m_descr(o.m_descr)
	,m_ctx(o.m_ctx)
	,m_out(o.m_out)
	,m_stk(o.m_stk){}

void StructSerializer::init( const langbind::TypedOutputFilterR& out, serialize::Flags::Enum flags_)
{
	m_ctx.clear();
	m_ctx.setFlags(flags_);
	m_stk.clear();
	m_stk.push_back( SerializeState( 0, m_descr->fetch(), m_ptr));
	m_out = out;
}

void StructSerializer::reset()
{
	m_ctx.clear();
	m_stk.clear();
	m_stk.push_back( SerializeState( 0, m_descr->fetch(), m_ptr));
}

langbind::TypedInputFilter* StructSerializer::copy() const
{
	return new StructSerializer(*this);
}

std::string StructSerializer::getElementPath( const SerializeStateStack& stk)
{
	std::string rt;
	SerializeStateStack::const_iterator itr=stk.begin(), end=stk.end();
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
		const Context::ElementBuffer* elem = m_ctx.getElem();
		if (elem)
		{
			if (!m_out->print( elem->m_type, elem->m_value))
			{
				if (m_out->getError())
				{
					throw SerializationErrorException( m_out->getError(), getElementPath( m_stk));
				}
				m_ctx.setElementUnconsumed();
				return false;
			}
			LOG_DATA << "[C++ structure serialization print] element " << langbind::InputFilter::elementTypeName( elem->m_type) << " '" << utils::getLogString( elem->m_value) << "'";
		}
		m_stk.back().fetch()( m_ctx, m_stk);
	}
	return true;
}

bool StructSerializer::getNext( langbind::FilterBase::ElementType& type, types::VariantConst& value)
{
	const Context::ElementBuffer* elem;
	while (m_stk.size() && (elem = m_ctx.getElem()) == 0)
	{
		m_stk.back().fetch()( m_ctx, m_stk);
	}
	if (!m_stk.size()) return false;

	type = elem->m_type;
	value = elem->m_value;
	setState( langbind::InputFilter::Open);
	LOG_DATA << "[C++ structure serialization get] element " << langbind::InputFilter::elementTypeName( elem->m_type) << " " << utils::getLogString( elem->m_value);
	return true;
}

bool StructSerializer::setFlags( FilterBase::Flags f)
{
	bool rt = true;
	rt &= langbind::TypedInputFilter::setFlags( f);
	if (flag( langbind::TypedInputFilter::SerializeWithIndices))
	{
		m_ctx.setFlags( serialize::Flags::SerializeWithIndices);
	}
	return rt;
}

