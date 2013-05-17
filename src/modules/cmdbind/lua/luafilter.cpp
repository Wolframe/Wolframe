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
///\file luafilter.cpp
///\brief Implementation of lua filters (serialization/deserialization of lua tables)
#include "luafilter.hpp"
#include "luaDebug.hpp"
#include "logger-v1.hpp"
#include <boost/lexical_cast.hpp>
#include <stdexcept>
#include <cstring>
#include <string>

extern "C" {
	#include <lualib.h>
	#include <lauxlib.h>
	#include <lua.h>
}

using namespace _Wolframe;
using namespace langbind;

#undef WOLFRAME_LOWLEVEL_DEBUG
#ifdef WOLFRAME_LOWLEVEL_DEBUG
static std::string get_lua_elem( lua_State* ls, int idx)
{
	switch (lua_type( ls, idx))
	{
		case LUA_TNIL:		return "nil";
		case LUA_TNUMBER:	return std::string("V(") + boost::lexical_cast<std::string>( lua_tonumber( ls, idx)) + ")";
		case LUA_TBOOLEAN:	return std::string("V(") + (lua_toboolean(ls,idx)?"true":"false") + ")";
		case LUA_TSTRING:	return std::string("V(") + lua_tostring(ls,idx) + ")";
		case LUA_TTABLE:	return "T";
		case LUA_TFUNCTION:	return "F";
		case LUA_TUSERDATA:	return "U";
		case LUA_TLIGHTUSERDATA:return "U";
	}
	return "X";
}

static void print_luaStack( lua_State* ls)
{
	std::string stk;
	for (int ii=-5; ii<=-1; ++ii)
	{
		stk.push_back(' ');
		stk.append( get_lua_elem( ls, ii));
	}
	std::cout << "------------ LUA STACK:" << stk << std::endl;
}

void wrap_lua_pushnil( lua_State* ls)					{std::cout << "lua_pushnil" << "()" << std::endl; ::lua_pushnil( ls); print_luaStack( ls);}
void wrap_lua_pushvalue( lua_State* ls, int arg)			{std::cout << "lua_pushvalue" << "("  << arg << ")" << std::endl; ::lua_pushvalue( ls, arg); print_luaStack( ls);}
void wrap_lua_pushboolean( lua_State* ls, bool arg)			{std::cout << "lua_pushboolean" << "("  << arg << ")" << std::endl; ::lua_pushboolean( ls, arg); print_luaStack( ls);}
void wrap_lua_pushinteger( lua_State* ls, int arg)			{std::cout << "lua_pushinteger" << "("  << arg << ")" << std::endl; ::lua_pushinteger( ls, arg); print_luaStack( ls);}
void wrap_lua_pushnumber( lua_State* ls, double arg)			{std::cout << "lua_pushnumber" << "("  << arg << ")" << std::endl; ::lua_pushnumber( ls, arg); print_luaStack( ls);}
void wrap_lua_pushlstring( lua_State* ls, const char* arg, int n)	{std::cout << "lua_pushlstring" << "("  << arg << "," << n << ")" << std::endl; ::lua_pushlstring( ls, arg, n); print_luaStack( ls);}
void wrap_lua_pushstring( lua_State* ls, const char* arg)		{std::cout << "lua_pushstring" << "("  << arg << ")" << std::endl; ::lua_pushstring( ls, arg); print_luaStack( ls);}
void wrap_lua_pop( lua_State* ls, int arg)				{std::cout << "lua_pop" << "(" << arg << ")" << std::endl; ::lua_pop( ls, arg); print_luaStack( ls);}
void wrap_lua_newtable( lua_State* ls)					{std::cout << "lua_newtable" << "()" << std::endl; ::lua_newtable( ls); print_luaStack( ls);}
void wrap_lua_gettable( lua_State* ls, int arg)				{std::cout << "lua_gettable" << "("  << arg << ")" << std::endl; lua_gettable( ls, arg); print_luaStack( ls);}
void wrap_lua_settable( lua_State* ls, int arg)				{std::cout << "lua_settable" << "("  << arg << ")" << std::endl; ::lua_settable( ls, arg); print_luaStack( ls);}
bool wrap_lua_next( lua_State* ls, int arg)				{std::cout << "lua_next" << "("  << arg << ")" << std::endl; int rt = ::lua_next( ls, arg); print_luaStack( ls); return rt;}
const char* wrap_lua_tostring( lua_State* ls, int arg)			{std::cout << "lua_tostring" << "("  << arg << ")" << std::endl; return ::lua_tostring( ls, arg);}
#else
#define wrap_lua_pushnil( ls)		lua_pushnil(ls)
#define wrap_lua_pushvalue( ls,a)	lua_pushvalue(ls,a)
#define wrap_lua_pushboolean( ls,a)	lua_pushboolean(ls,a)
#define wrap_lua_pushinteger( ls,a)	lua_pushinteger(ls,a)
#define wrap_lua_pushnumber( ls,a)	lua_pushnumber(ls,a)
#define wrap_lua_pushlstring( ls,a,n)	lua_pushlstring(ls,a,n)
#define wrap_lua_pushstring( ls,a)	lua_pushstring(ls,a)
#define wrap_lua_pop( ls,a)		lua_pop(ls,a)
#define wrap_lua_newtable( ls)		lua_newtable(ls)
#define wrap_lua_gettable( ls,a)	lua_gettable(ls,a)
#define wrap_lua_settable( ls,a)	lua_settable(ls,a)
#define wrap_lua_settable( ls,a)	lua_settable(ls,a)
#define wrap_lua_next( ls,a)		lua_next(ls,a)
#define wrap_lua_tostring( ls,a)	lua_tostring(ls,a)
#endif

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

LuaTableInputFilter::LuaTableInputFilter( lua_State* ls)
	:types::TypeSignature("langbind::LuaTableInputFilter", __LINE__)
	,LuaExceptionHandlerScope(ls)
	,m_ls(ls)
{
	FetchState fs;
	fs.id = FetchState::Init;
	fs.tag = 0;
	fs.tagsize = 0;
	m_stk.push_back( fs);
}

void LuaTableInputFilter::FetchState::getTagElement( TypedInputFilter::Element& element)
{
	element.type = Element::string_;
	element.value.string_.ptr = tag;
	element.value.string_.size = tagsize;
}

bool LuaTableInputFilter::getValue( int idx, TypedFilterBase::Element& element)
{
	const char* errelemtype = 0;
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
					type = CloseTag;
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
				type = OpenTag;
				return true;

			case FetchState::TableIterOpen:
				if (!getValue( -2, element)) return false;
				if (element.type == TypedInputFilter::Element::string_ && element.value.string_.size == 0)
				{
					m_stk.back().id = FetchState::TableIterValueNoTag;
					continue;
				}
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

			case FetchState::TableIterValueNoTag:
				if (lua_istable( m_ls, -1))
				{
					m_stk.back().id = FetchState::TableIterNext;
					const char* tag = lua_isstring(m_ls,-2)?lua_tostring( m_ls,-2):0;
					if (!firstTableElem(tag) && state() == InputFilter::Error) return false;
					continue;
				}
				else
				{
					m_stk.back().id = FetchState::TableIterNext;
					type = FilterBase::Value;
					return getValue( -1, element);
				}

			case FetchState::TableIterClose:
				element = TypedInputFilter::Element();
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
				if (m_stk.size() == 0)
				{
					type = CloseTag;
					element = Element();
					return true;
				}
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
			wrap_lua_pushboolean( m_ls, element.value.bool_);
			return true;
		case Element::double_:
			wrap_lua_pushnumber( m_ls, (lua_Number)element.value.double_);
			return true;
		case Element::int_:
			wrap_lua_pushinteger( m_ls, (lua_Integer)element.value.int_);
			return true;
		case Element::uint_:
			wrap_lua_pushinteger( m_ls, (lua_Integer)element.value.uint_);
			return true;
		case Element::string_:
			wrap_lua_pushlstring( m_ls, element.value.string_.ptr, element.value.string_.size);
			wrap_lua_tostring( m_ls, -1); //PF:BUGFIX lua 5.1.4 needs this one
			return true;
		case Element::blob_:
			wrap_lua_pushlstring( m_ls, (const char*)element.value.blob_.ptr, element.value.blob_.size);
			wrap_lua_tostring( m_ls, -1); //PF:BUGFIX lua 5.1.4 needs this one
			return true;
	}
	setState( OutputFilter::Error, "illegal value type of element");
	return false;
}

bool LuaTableOutputFilter::openTag( const Element& element)
{
	if (!pushValue( element)) return false;		///... LUA STK: t k   (t=Table,k=Key)
	wrap_lua_pushvalue( m_ls, -1);			///... LUA STK: t k k
	wrap_lua_gettable( m_ls, -3);			///... LUA STK: t k t[k]
	if (lua_isnil( m_ls, -1))
	{
		///... element with this key not yet defined. so we define it
		wrap_lua_pop( m_ls, 1);			///... LUA STK: t k
		wrap_lua_newtable( m_ls);			///... LUA STK: t k NEWTABLE
		m_statestk.push_back( Struct);
		return true;
	}
	// check for t[k][#t[k]] exists -> it is an array:
	std::size_t len = lua_rawlen( m_ls, -1);
	wrap_lua_pushinteger( m_ls, len);			///... LUA STK: t k t[k] #t[k]
	wrap_lua_gettable( m_ls, -2);			///... LUA STK: t k t[k] t[k][#t[k]]
	bool isArray = !lua_isnil( m_ls, -1);
	if (isArray)
	{
		///... the table is an array
		wrap_lua_pop( m_ls, 1);			///... LUA STK: t k ar
		wrap_lua_pushinteger( m_ls, len+1);		///... LUA STK: t k ar len+1
		wrap_lua_newtable( m_ls);			///... LUA STK: t k ar len+1 NEWTABLE
		m_statestk.push_back( Vector);
	}
	else
	{
		///... the table is a structure. but the element is already defined. we create an array
		wrap_lua_pop( m_ls, 2);			///... LUA STK: t k
		wrap_lua_newtable( m_ls);			///... LUA STK: t k ar
		wrap_lua_pushinteger( m_ls, 1);		///... LUA STK: t k ar 1
		wrap_lua_pushvalue( m_ls, -3);		///... LUA STK: t k ar 1 k
		wrap_lua_gettable( m_ls, -5);		///... LUA STK: t k ar 1 t[k]
		wrap_lua_settable( m_ls, -3);		///... LUA STK: t k ar             (ar[1]=t[k])
		wrap_lua_pushinteger( m_ls, 2);		///... LUA STK: t k ar 2
		wrap_lua_newtable( m_ls);			///... LUA STK: t k ar 2 NEWTABLE
		m_statestk.push_back( Vector);
	}
	return true;
}

bool LuaTableOutputFilter::closeTag()
{
	if (m_statestk.size() > 0)
	{
		if (m_statestk.back() == Vector)
		{
			m_statestk.pop_back();				//... LUA STK: t k ar i v
			wrap_lua_settable( m_ls, -3);			//... LUA STK: t k ar		(ar[i] = v)
			wrap_lua_settable( m_ls, -3);			//... LUA STK: t		(t[k] = ar)
		}
		else
		{
			m_statestk.pop_back();				//... LUA STK: t k v
			wrap_lua_settable( m_ls, -3);			//... LUA STK: t		(t[k] = v)
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
	wrap_lua_pushnil( m_ls);			//... LUA STK: t a t nil
	if (wrap_lua_next( m_ls, -2))
	{
		// ... non empty table, we create an element with an empty key
		wrap_lua_pop( m_ls, 2);
		return openTag( Element()) && closeAttribute( element) && closeTag();
	}
	else
	{
		// ... table empty. replace it by value 'element'
		wrap_lua_pop( m_ls, 1);			//... LUA STK: t a t
		m_statestk.back() = Atomic;		//... LUA STK: t a
		return pushValue( element);		//... LUA STK: t a v
	}
}

bool LuaTableOutputFilter::print( ElementType type, const Element& element)
{
	LOG_DATA << "[lua table] push element " << langbind::InputFilter::elementTypeName( type) << " '" << element.tostring() << "'";
	if (!lua_checkstack( m_ls, 16))
	{
		setState( OutputFilter::Error, "lua stack overflow");
		return false;
	}
	if (m_statestk.size() == 0)
	{
		m_statestk.push_back( Struct);
		wrap_lua_newtable( m_ls);
	}

	switch (type)
	{
		case OpenTag:
			m_hasElement = false;
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
			m_hasElement = true;
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
			m_hasElement = true;
			switch (m_type)
			{
				case Attribute:
					m_type = OpenTag;
					//... back to open tag
					return closeAttribute( element) && closeTag();
				case OpenTag:
				case Value:
				case CloseTag:
					m_type = type;
					return closeAttribute( element);
			}

		case CloseTag:
			switch (m_type)
			{
				case Attribute:
					setState( OutputFilter::Error, "output filter of attribute value missed");
					return false;
				case OpenTag:
					m_type = type;
					if (!m_hasElement)
					{
						wrap_lua_pop( m_ls, 2);
						return true;
					}
					/*no break here!*/
				case Value:
				case CloseTag:
					m_type = type;
					return closeTag();
			}
	}
	setState( OutputFilter::Error, "illegal state");
	return false;
}



