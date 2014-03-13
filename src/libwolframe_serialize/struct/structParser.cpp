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
///\file struct/structParser.cpp
///\brief Implements deserialization parser
#include "serialize/struct/structParser.hpp"
#include "serialize/struct/serializeStack.hpp"
#include "serialize/serializationErrorException.hpp"
#include "filter/typingfilter.hpp"
#include "utils/printFormats.hpp"
#include <cstring>
#include <stdexcept>

using namespace _Wolframe;
using namespace serialize;

StructParser::StructParser( const ObjectReference& obj, const StructDescriptionBase* descr_)
	:m_ptr(obj.get())
	,m_obj(obj)
	,m_descr(descr_)
{
	ParseState st( 0, m_descr->parse(), m_ptr);
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

std::string StructParser::getElementPath( const ParseStateStack& stk)
{
	std::string rt;
	ParseStateStack::const_iterator itr=stk.begin(), end=stk.end();
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
	if (i->flag( langbind::FilterBase::PropagateNoCase))
	{
		m_ctx.setFlags( Context::CaseInsensitiveCompare);
	}
	m_ctx.setFlags(flags);
	if (i->flag( langbind::FilterBase::PropagateNoAttr))
	{
		m_ctx.unsetFlags( Context::ValidateAttributes);
	}
	m_stk.clear();
	m_stk.push_back( ParseState( 0, m_descr->parse(), m_ptr));
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

