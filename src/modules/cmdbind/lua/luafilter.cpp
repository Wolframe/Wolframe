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
#include <cmath>

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
	int nn = lua_gettop(ls);

	for (int ii=-nn; ii<=-1; ++ii)
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
const char* wrap_lua_tolstring( lua_State* ls, int arg, size_t* size)	{std::cout << "lua_tolstring" << "("  << arg << ")" << std::endl; return ::lua_tolstring( ls, arg, size);}
double wrap_lua_tonumber( lua_State* ls, int arg)			{std::cout << "lua_tonumber" << "("  << arg << ")" << std::endl; return ::lua_tonumber( ls, arg);}
bool wrap_lua_toboolean( lua_State* ls, int arg)			{std::cout << "lua_toboolean" << "("  << arg << ")" << std::endl; return ::lua_toboolean( ls, arg);}
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
#define wrap_lua_next( ls,a)		lua_next(ls,a)
#define wrap_lua_tostring( ls,a)	lua_tostring(ls,a)
#define wrap_lua_tolstring( ls,a,n)	lua_tolstring(ls,a,n)
#define wrap_lua_tonumber( ls,a)	lua_tonumber(ls,a)
#define wrap_lua_toboolean( ls,a)	lua_toboolean(ls,a)
#endif

static bool getElementValue( lua_State* ls, int idx, types::VariantConst& element, const char*& errelemtype)
{
	const char* lstr;
	std::size_t lstrlen = 0;
	switch (lua_type( ls, idx))
	{
		case LUA_TNIL:
			return false;

		case LUA_TNUMBER:
		{
			double num = (double)wrap_lua_tonumber(ls, idx);
			double flo = std::floor( num);
			if (num - flo <= std::numeric_limits<double>::epsilon())
			{
				element = boost::numeric_cast<types::Variant::Data::Int>(num);
			}
			else
			{
				element = num;
			}
			return true;
		}
		case LUA_TBOOLEAN:
			element = (bool)wrap_lua_toboolean(ls, idx);
			return true;

		case LUA_TSTRING:
			lstr = (const char*)wrap_lua_tolstring( ls, idx, &lstrlen);
			element.init( lstr, lstrlen);
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
	FetchState fs( FetchState::Init);
	m_stk.push_back( fs);
}

void LuaTableInputFilter::FetchState::getTagElement( types::VariantConst& element)
{
	element.init( tag, tagsize);
}

bool LuaTableInputFilter::getValue( int idx, types::VariantConst& element)
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
	wrap_lua_pushnil( m_ls);
	if (!wrap_lua_next( m_ls, -2))
	{
		return false;
	}
	switch (lua_type( m_ls, -2))
	{
		case LUA_TNUMBER:
		{
			if (flag( SerializeWithIndices))
			{
				//... first key is number, but we have to serialize with indices, so we treat is as a simple table
				FetchState fs( FetchState::TableIterOpen);
				m_stk.push_back( fs);
				return true;
			}
			//... first key is number and we do not need to serialize with indices, so we treat it as a vector with repeating open tag for vector elements
			FetchState fs( FetchState::VectorIterValue, tag, tag?std::strlen( tag):0);
			m_stk.push_back( fs);
			return true;
		}
		case LUA_TSTRING:
		{
			//... first key is string, we treat it as a struct
			FetchState fs( FetchState::TableIterOpen);
			m_stk.push_back( fs);
			return true;
		}
		default:
		{
			setState( InputFilter::Error, "cannot treat table as a struct nor an array");
			wrap_lua_pop( m_ls, 2);
			return false;
		}
	}
}

bool LuaTableInputFilter::nextTableElem()
{
	if (!lua_istable( m_ls, -3))
	{
		setState( InputFilter::Error, "illegal state (internal): nextTableElem called on non table");
		return false;
	}
	wrap_lua_pop( m_ls, 1);
	if (!wrap_lua_next( m_ls, -2))
	{
		return false;
	}
	return true;
}

static const char* getTagName( lua_State* ls, int idx)
{
	if (lua_type( ls, idx) == LUA_TSTRING)
	{
		return lua_tostring( ls, idx);
	}
	else
	{
		return 0;
	}
}

bool LuaTableInputFilter::getNext( ElementType& type, types::VariantConst& element)
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
			case FetchState::TopLevel:
				if (lua_istable( m_ls, -1))
				{
					m_stk.back().id = FetchState::Done;
					if (!firstTableElem() && state() == InputFilter::Error) return false;
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
					if (!m_stk.back().tag) continue;

					m_stk.back().getTagElement( element);
					type = CloseTag;
					return true;
				}
				else if (state() == InputFilter::Error)
				{
					return false;
				}
				else
				{
					m_stk.pop_back();
					continue;
				}

			case FetchState::VectorIterReopen:
				m_stk.back().id = FetchState::VectorIterValue;
				if (!m_stk.back().tag) continue;

				m_stk.back().getTagElement( element);
				type = OpenTag;
				return true;

			case FetchState::TableIterOpen:
				if (!getValue( -2, element)) return false;
				if (element.type() == types::Variant::String && element.charsize() == 0)
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
					const char* tag = getTagName( m_ls,-2);
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
					const char* tag = (lua_type(m_ls,-2)==LUA_TSTRING)?lua_tostring( m_ls,-2):0;
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
				element.init();
				m_stk.back().id = FetchState::TableIterNext;
				type = CloseTag;
				return true;

			case FetchState::TableIterNext:
				if (nextTableElem())
				{
					m_stk.back().id = FetchState::TableIterOpen;
				}
				else if (state() == InputFilter::Error)
				{
					return false;
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
					element.init();
					return true;
				}
				continue;
		}
	}
	setState( InputFilter::Error, "illegal state in lua filter get next");
	return false;
}


bool LuaTableOutputFilter::pushValue( const types::VariantConst& element)
{
	switch (element.type())
	{
		case types::Variant::Null:
			wrap_lua_pushnil( m_ls);
			return true;
		case types::Variant::Bool:
			wrap_lua_pushboolean( m_ls, element.tobool());
			return true;
		case types::Variant::Double:
			wrap_lua_pushnumber( m_ls, (lua_Number)element.todouble());
			return true;
		case types::Variant::Int:
			wrap_lua_pushinteger( m_ls, (lua_Integer)element.toint());
			return true;
		case types::Variant::UInt:
			wrap_lua_pushinteger( m_ls, (lua_Integer)element.touint());
			return true;
		case types::Variant::String:
			wrap_lua_pushlstring( m_ls, element.charptr(), element.charsize());
			wrap_lua_tostring( m_ls, -1); //PF:BUGFIX lua 5.1.4 needs this one
			return true;
	}
	setState( OutputFilter::Error, "illegal value type of element");
	return false;
}

bool LuaTableOutputFilter::openTag( const types::VariantConst& element)
{
	if (!pushValue( element)) return false;			///... LUA STK: t k   (t=Table,k=Key)
	wrap_lua_pushvalue( m_ls, -1);				///... LUA STK: t k k
	wrap_lua_gettable( m_ls, -3);				///... LUA STK: t k t[k]
	if (lua_isnil( m_ls, -1))
	{
		///... element with this key not yet defined. so we define it
		wrap_lua_pop( m_ls, 1);				///... LUA STK: t k
		wrap_lua_newtable( m_ls);			///... LUA STK: t k NEWTABLE
		m_statestk.push_back( Struct);
		return true;
	}
	// check for t[k] beeing table and t[k][#t[k]] exists -> t[k] is an array:
	if (lua_istable( m_ls, -1))					///... LUA STK: t k t[k]
	{
		std::size_t len = lua_rawlen( m_ls, -1);
		wrap_lua_pushinteger( m_ls, len);			///... LUA STK: t k t[k] #t[k]
		wrap_lua_gettable( m_ls, -2);				///... LUA STK: t k t[k] t[k][#t[k]]
		if (!lua_isnil( m_ls, -1))
		{
			///... the table is an array
			wrap_lua_pop( m_ls, 1);				///... LUA STK: t k t[k]
			wrap_lua_pushinteger( m_ls, len+1);		///... LUA STK: t k ar len+1
			wrap_lua_newtable( m_ls);			///... LUA STK: t k ar len+1 NEWTABLE
			m_statestk.push_back( Vector);
			return true;
		}
		else
		{
			wrap_lua_pop( m_ls, 1);				///... LUA STK: t k t[k]
		}
	}
	///... the element is a structure or atomic element already defined. we create an array
	wrap_lua_pop( m_ls, 1);				///... LUA STK: t k
	wrap_lua_newtable( m_ls);			///... LUA STK: t k ar
	wrap_lua_pushinteger( m_ls, 1);			///... LUA STK: t k ar 1
	wrap_lua_pushvalue( m_ls, -3);			///... LUA STK: t k ar 1 k
	wrap_lua_gettable( m_ls, -5);			///... LUA STK: t k ar 1 t[k]
	wrap_lua_settable( m_ls, -3);			///... LUA STK: t k ar             (ar[1]=t[k])
	wrap_lua_pushinteger( m_ls, 2);			///... LUA STK: t k ar 2
	wrap_lua_newtable( m_ls);			///... LUA STK: t k ar 2 NEWTABLE
	m_statestk.push_back( Vector);
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

bool LuaTableOutputFilter::closeAttribute( const types::VariantConst& element)
{
	if (!lua_istable( m_ls, -1))
	{
		setState( OutputFilter::Error, "illegal state (internal): closeAttribute called on non table");
		return false;
	}
	wrap_lua_pushnil( m_ls);			//... LUA STK: t a t nil
	if (wrap_lua_next( m_ls, -2))
	{
		// ... non empty table, we create an element with an empty key
		wrap_lua_pop( m_ls, 2);
		return openTag( types::VariantConst("")) && closeAttribute( element) && closeTag();
	}
	else
	{
		// ... table empty. replace it by value 'element'
		wrap_lua_pop( m_ls, 1);			//... LUA STK: t a
		return pushValue( element);		//... LUA STK: t a v
	}
}

bool LuaTableOutputFilter::print( ElementType type, const types::VariantConst& element)
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
				case Value:
					setState( OutputFilter::Error, "got two subsequent values without enclosing tag (without vector index or table element name)");
					return false;

				case OpenTag:
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



