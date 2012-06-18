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
///\file filter_luafilter.cpp
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

void LuaTableInputFilter::FetchState::getTagElement( TypedInputFilter::Element& element)
{
	element.type = Element::string_;
	element.value.string_.ptr = tag;
	element.value.string_.size = tagsize;
}

LuaTableInputFilter::LuaTableInputFilter( lua_State* ls)
	:LuaExceptionHandlerScope(ls)
	,m_ls(ls)
{
	FetchState fs;
	fs.id = FetchState::Init;
	fs.tag = 0;
	fs.tagsize = 0;
	m_stk.push_back( fs);
}

bool LuaTableInputFilter::getValue( int idx, TypedFilterBase::Element& element)
{
	const char* errelemtype;
	if (!getElementValue( m_ls, idx, element, errelemtype))
	{
		std::ostringstream msg;
		msg << "element type '" << (errelemtype?errelemtype:"unknown") << "' not expected for atomic value in filter";
		setState( InputFilter::Error, msg.str().c_str());
		return false;
	}
	return true;
}

bool LuaTableInputFilter::firstTableElem( const char* tag=0)
{
	FetchState fs;
	lua_pushnil( m_ls);
	if (!lua_next( m_ls, -2))
	{
		return false;
	}
	switch (lua_type( m_ls, -2))
	{
		case LUA_TNUMBER:
		{
			//... first key is number. we treat it as a vector
			if (!tag)
			{
				setState( InputFilter::Error, "cannot build filter for an array because tag as string is missing");
				lua_pop( m_ls, 2);
				return false;
			}
			fs.id = FetchState::VectorIterValue;
			fs.tag = tag;
			fs.tagsize = std::strlen( tag);
			m_stk.push_back( fs);
			return true;
		}
		case LUA_TSTRING:
		{
			//... first key is string, we treat it as a struct
			fs.id = FetchState::TableIterOpen;
			fs.tag = 0;
			fs.tagsize = 0;
			m_stk.push_back( fs);
			return true;
		}
		default:
		{
			setState( InputFilter::Error, "cannot treat table as a struct nor an array");
			lua_pop( m_ls, 2);
			return false;
		}
	}
}

bool LuaTableInputFilter::nextTableElem()
{
	lua_pop( m_ls, 1);
	if (!lua_next( m_ls, -2))
	{
		return false;
	}
	return true;
}

bool LuaTableInputFilter::getNext( ElementType& type, Element& element)
{
	if (!lua_checkstack( m_ls, 10))
	{
		setState( InputFilter::Error, "lua stack overflow");
		return false;
	}
	for (;;)
	{
		if (m_stk.size() == 0) return false;

		switch (m_stk.back().id)
		{
			case FetchState::Init:
				if (lua_istable( m_ls, -1))
				{
					m_stk.back().id = FetchState::Done;
					if (!firstTableElem()) return false;
					continue;
				}
				else
				{
					m_stk.pop_back();
					type = FilterBase::Value;
					return getValue( -1, element);
				}

			case FetchState::VectorIterValue:
				if (lua_istable( m_ls, -1))
				{
					m_stk.back().id = FetchState::VectorIterClose;
					if (!firstTableElem() && state() == InputFilter::Error) return false;
					continue;
				}
				else
				{
					m_stk.back().id = FetchState::VectorIterClose;
					type = FilterBase::Value;
					return getValue( -1, element);
				}

			case FetchState::VectorIterClose:
				if (nextTableElem())
				{
					m_stk.back().id = FetchState::VectorIterReopen;
					m_stk.back().getTagElement( element);
					type = FilterBase::CloseTag;
					return true;
				}
				else
				{
					m_stk.pop_back();
					continue;
				}

			case FetchState::VectorIterReopen:
				m_stk.back().id = FetchState::VectorIterValue;
				m_stk.back().getTagElement( element);
				type = FilterBase::OpenTag;
				return true;

			case FetchState::TableIterOpen:
				if (!getValue( -2, element)) return false;
				m_stk.back().id = FetchState::TableIterValue;
				type = OpenTag;
				return true;

			case FetchState::TableIterValue:
				if (lua_istable( m_ls, -1))
				{
					m_stk.back().id = FetchState::TableIterClose;
					const char* tag = lua_isstring(m_ls,-2)?lua_tostring( m_ls,-2):0;
					if (!firstTableElem(tag) && state() == InputFilter::Error) return false;
					continue;
				}
				else
				{
					m_stk.back().id = FetchState::TableIterClose;
					type = FilterBase::Value;
					return getValue( -1, element);
				}

			case FetchState::TableIterClose:
				if (!getValue( -2, element)) return false;
				m_stk.back().id = FetchState::TableIterNext;
				type = CloseTag;
				return true;

			case FetchState::TableIterNext:
				if (nextTableElem())
				{
					m_stk.back().id = FetchState::TableIterOpen;
				}
				else
				{
					m_stk.pop_back();
				}
				continue;

			case FetchState::Done:
				setState( InputFilter::Open);
				m_stk.pop_back();
				continue;
		}
	}
	setState( InputFilter::Error, "illegal state in lua filter get next");
	return false;
}


bool LuaTableOutputFilter::pushValue( const Element& element)
{
	switch (element.type)
	{
		case Element::bool_:
			lua_pushboolean( m_ls, element.value.bool_);
			return true;
		case Element::double_:
			lua_pushnumber( m_ls, (lua_Number)element.value.double_);
			return true;
		case Element::int_:
			lua_pushnumber( m_ls, (lua_Number)element.value.int_);
			return true;
		case Element::uint_:
			lua_pushnumber( m_ls, (lua_Number)element.value.uint_);
			return true;
		case Element::string_:
			lua_pushlstring( m_ls, element.value.string_.ptr, element.value.string_.size);
			return true;
	}
	setState( OutputFilter::Error, "illegal value type printed");
	return false;
}

bool LuaTableOutputFilter::openTag( const Element& element)
{
	++m_depth;
	if (m_depth < 0)
	{
		setState( OutputFilter::Error, "tag stack overflow");
		return false;
	}
	if (!pushValue( element)) return false;		///... LUA STK: t k   (t=Table,k=Key)
	lua_pushvalue( m_ls, -1);			///... LUA STK: t k k
	lua_gettable( m_ls, -3);			///... LUA STK: t k t[k]
	if (lua_isnil( m_ls, -1))
	{
		///... element with this key not yet defined. so we define it
		lua_pop( m_ls, 1);			///... LUA STK: t k
		lua_newtable( m_ls);			///... LUA STK: t k NEWTABLE
		return true;
	}
	// check for t[k][#t[k]] exists -> it is an array:
	std::size_t len = lua_rawlen( m_ls, -1);
	lua_pushnumber( m_ls, (double)len);		///... LUA STK: t k t[k] #t[k]
	lua_gettable( m_ls, -2);			///... LUA STK: t k t[k] t[k][#t[k]]
	if (lua_isnil( m_ls, -1))
	{
		///... the table is a structure. but the element is already defined. we create an array
		lua_pop( m_ls, 2);			///... LUA STK: t k
		lua_newtable( m_ls);			///... LUA STK: t k v
		lua_pushnumber( m_ls, 1);		///... LUA STK: t k v 1
		lua_pushvalue( m_ls, -3);		///... LUA STK: t k v 1 k
		lua_gettable( m_ls, -5);		///... LUA STK: t k v 1 t[k]
		lua_settable( m_ls, -3);		///... LUA STK: t k v             (v[1]=t[k])
		lua_pushnumber( m_ls, 2);		///... LUA STK: t k v 2
		lua_newtable( m_ls);			///... LUA STK: t k v 2 NEWTABLE
	}
	else
	{
		///... the table is an array
		lua_pop( m_ls, 2);			///... LUA STK: t k v
		lua_pushnumber( m_ls, (double)(len+1));	///... LUA STK: t k v len+1
		lua_newtable( m_ls);			///... LUA STK: t k v len+1 NEWTABLE
	}
	return true;
}

bool LuaTableOutputFilter::closeTag()
{
	if (m_depth > 0)
	{
		--m_depth;
		lua_settable( m_ls, -3);
		std::size_t len = lua_rawlen( m_ls, -1);
		lua_pushnumber( m_ls, (double)len);
		lua_gettable( m_ls, -2);
		bool isArray = !(bool)lua_isnil( m_ls, -1);
		lua_pop( m_ls, 1);
		if (isArray)
		{
			///... a vector is popped twice
			lua_settable( m_ls, -3);
		}
		return true;
	}
	else
	{
		setState( OutputFilter::Error, "tags not balanced");
		return false;
	}
}

bool LuaTableOutputFilter::closeAttribute( const Element& element)
{
	lua_pushnil( m_ls);
	if (lua_next( m_ls, -2))
	{
		lua_pop( m_ls, 2);
		setState( OutputFilter::Error, "value without tag or attribute context");
		return false;
	}
	else
	{
		// ... table empty. replace it by value 'element'
		lua_pop( m_ls, 1);
		return pushValue( element);
	}
}

bool LuaTableOutputFilter::print( ElementType type, const Element& element)
{
	if (!lua_checkstack( m_ls, 16))
	{
		setState( OutputFilter::Error, "lua stack overflow");
		return false;
	}
	if (!m_isinit)
	{
		lua_newtable( m_ls);
		m_isinit = true;
	}

	switch (type)
	{
		case OpenTag:
			switch (m_type)
			{
				case Attribute:
				case Value:
					setState( OutputFilter::Error, "unexpected open tag");
					return false;
				case OpenTag:
				case CloseTag:
					m_type = type;
					return openTag( element);
			}
			break;

		case Attribute:
			switch (m_type)
			{
				case Attribute:
				case Value:
				case CloseTag:
					setState( OutputFilter::Error, "unexpected attribute");
					return false;
				case OpenTag:
					m_type = type;
					return openTag( element);
			}
			break;

		case Value:
			switch (m_type)
			{
				case Attribute:
					m_type = OpenTag;
					//... back to open tag
					return closeAttribute( element) && closeTag();
				case OpenTag:
					m_type = type;
					return closeAttribute( element);
				case Value:
					setState( OutputFilter::Error, "print cannot handle subsequent values");
					return false;
				case CloseTag:
					setState( OutputFilter::Error, "print cannot handle values outside tag context");
					return false;
			}

		case CloseTag:
			switch (m_type)
			{
				case Attribute:
					setState( OutputFilter::Error, "print of attribute value missed");
					return false;
				case OpenTag:
					m_type = type;
					// ... table empty. remove it
					lua_pop( m_ls, 2);
					return true;
				case Value:
				case CloseTag:
					m_type = type;
					return closeTag();
			}
	}
	setState( OutputFilter::Error, "illegal state");
	return false;
}



