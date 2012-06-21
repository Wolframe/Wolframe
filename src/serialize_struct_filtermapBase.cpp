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
///\file serialize/struct/filtermapBase.cpp
///\brief Implements the non intrusive base class of serialization/deserialization of filters
#include "serialize/struct/filtermapBase.hpp"
#include "filter/typingfilter.hpp"
#include <cstring>
#include <stdexcept>

using namespace _Wolframe;
using namespace serialize;

StructSerializer::StructSerializer( const void* obj, const FiltermapDescriptionBase* descr, Context::Flags flags)
	:m_obj(obj)
	,m_descr(descr)
	,m_ctx(flags)
{
	m_stk.push_back( FiltermapPrintState( 0, m_descr->print(), m_obj));
}

StructSerializer::StructSerializer( const StructSerializer& o)
	:m_obj(o.m_obj)
	,m_descr(o.m_descr)
	,m_ctx(o.m_ctx)
	,m_stk(o.m_stk){}

void StructSerializer::init()
{
	m_ctx.clear();
	m_stk.clear();
	m_stk.push_back( FiltermapPrintState( 0, m_descr->print(), m_obj));
}

static std::string getParsePath( const FiltermapParseStateStack& stk)
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

static std::string getPrintPath( const FiltermapPrintStateStack& stk)
{
	std::string rt;
	FiltermapPrintStateStack::const_iterator itr=stk.begin(), end=stk.end();
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

bool FiltermapDescriptionBase::parse( void* obj, langbind::TypedInputFilter& tin, Context& ctx, FiltermapParseStateStack& stk) const
{
	bool rt = true;
	try
	{
		if (stk.size() == 0)
		{
			if (!m_parse) throw std::runtime_error( "null parser called");
			stk.push_back( FiltermapParseState( 0, m_parse, obj));
		}
		while (rt && stk.size())
		{
			rt = stk.back().parse()( tin, ctx, stk);
		}
		if (tin.state() == langbind::InputFilter::Open && !ctx.getLastError() && stk.size() == 1)
		{
			return true;
		}
		if (!rt && ctx.getLastError())
		{
			std::string path = getParsePath( stk);
			ctx.setTag( path.c_str());
		}
	}
	catch (std::exception& e)
	{
		ctx.setError( e.what());
		rt = false;
	}
	return rt;
}

StructSerializer::CallResult StructSerializer::print( langbind::TypedOutputFilter& out)
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
			if (!m_stk.back().print()( m_ctx, m_stk))
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

bool StructSerializer::getNext( langbind::FilterBase::ElementType& type, langbind::TypedFilterBase::Element& value)
{
	try
	{
		if (!m_stk.size()) return false;

		Context::ElementBuffer elem;
		while (!m_ctx.getElem( elem))
		{
			if (!m_stk.back().print()( m_ctx, m_stk))
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
