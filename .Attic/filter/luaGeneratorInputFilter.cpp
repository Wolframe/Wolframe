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
///\brief implementation of lua filters
#include "filter/luafilter.hpp"
#include "langbind/luaDebug.hpp"
#include <boost/lexical_cast.hpp>
#include <stdexcept>
#include <cstring>

extern "C" {
	#include <lualib.h>
	#include <lauxlib.h>
	#include <lua.h>
}

using namespace _Wolframe;
using namespace langbind;

LuaGeneratorInputFilter::~LuaGeneratorInputFilter()
{
	if (m_thread)
	{
		lua_pop( m_ls, 2);					///...STK: func
		luaL_unref( m_ls, LUA_REGISTRYINDEX, m_threadref);	///...allow garbage collecting of the thread
	}
}

static bool getElementValue( lua_State* ls, int idx, TypedInputFilter::Element& element, const char*& errelemtype)
{
	switch (lua_type( ls, idx))
	{
		case LUA_TNIL:
			return false;

		case LUA_TNUMBER:
			element.type = TypedFilterBase::Element::double_;
			element.value.double_ = (double)lua_tonumber(ls, idx);
			return true;

		case LUA_TBOOLEAN:
			element.type = TypedFilterBase::Element::bool_;
			element.value.bool_ = (bool)lua_toboolean(ls, idx);
			return true;

		case LUA_TSTRING:
			element.type = TypedFilterBase::Element::string_;
			element.value.string_.ptr = (const char*)lua_tostring(ls, idx);
			element.value.string_.size = std::strlen( element.value.string_.ptr);
			return true;
		default:
			errelemtype = lua_typename( ls, lua_type( ls, idx));
			return false;
	}
}

bool LuaGeneratorInputFilter::getValue( int idx, TypedInputFilter::Element& element)
{
	const char* errelemtype;
	if (!getElementValue( m_thread, idx, element, errelemtype))
	{
		std::ostringstream msg;
		msg << "element type '" << (errelemtype?errelemtype:"unknown") << "' not expected for atomic value in filter";
		setState( InputFilter::Error, msg.str().c_str());
		return false;
	}
	if (element.type == TypedFilterBase::Element::bool_ && element.value.bool_ == false)
	{
		return false;
	}
	return true;
}

bool LuaGeneratorInputFilter::getNext( ElementType& type, Element& element)
{
	setState( InputFilter::Open);
	if (!m_thread)
	{
		m_thread = lua_newthread( m_ls);			///...STK: func thd
		lua_pushvalue( m_ls, -1);				///...STK: func thd thd
		m_threadref = luaL_ref( m_ls, LUA_REGISTRYINDEX);	///...STK: func thd
		lua_pushvalue( m_ls, -2);				///...STK: func thd func
	}
	if (m_lasttype == Attribute)
	{
		type = m_lasttype = Value;
		element = m_attrvalue;
		return true;
	}
	int rt = lua_resume( m_thread, NULL, 0);
	if (rt == LUA_YIELD)
	{
		setState( InputFilter::EndOfMessage);
		return false;
	}
	if (rt != 0)
	{
		const char* msg = lua_tostring( m_thread, -1);
		setState( InputFilter::Error, msg);
		return false;
	}
	if (getValue( -2, element))			//...fetch tag
	{
		if (state() == InputFilter::Error) return false;
		if (getValue( -1, m_attrvalue))		//...fetch attribute value
		{
			type = m_lasttype = Attribute;
			return true;
		}
		else
		{
			if (state() == InputFilter::Error) return false;
			type = m_lasttype = OpenTag;
			return true;
		}
	}
	else
	{
		// no tag given:
		if (state() == InputFilter::Error) return false;
		if (getValue( -1, element))		//...fetch value
		{
			type = m_lasttype = Value;
			return true;
		}
		else
		{
			// no tag, no value -> close tag
			if (state() == InputFilter::Error) return false;
			type = m_lasttype = CloseTag;
			return true;
		}
	}
}

