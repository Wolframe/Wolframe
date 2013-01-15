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
/**
Idea of a filter based on a lua function returning v,t tuples
Given up.
Does not work according my current Lua knowlegde because the called function must not yield.
Yielding is not allowed across C/Lua function boundaries, e.g. Lua calling C, calling Lua that yields
execution. This makes it to be a dangerous feature. It raises more questions than it answers.
Therefore it is not implemented.
**/

#ifndef _Wolframe_LUA_GENERATOR_INPUT_FILTER_HPP_INCLUDED
#define _Wolframe_LUA_GENERATOR_INPUT_FILTER_HPP_INCLUDED
#include "filter/typedfilter.hpp"
#include "langbind/luaException.hpp"
#include <vector>

#if WITH_LUA
extern "C" {
	#include "lua.h"
}

namespace _Wolframe {
namespace langbind {

///\class class LuaGeneratorInputFilter
///\brief Lua generator function (function with closure) as typed input filter
///\remark The lua function has no arguments and returns a pair (value, tag) like an input filter does
class LuaGeneratorInputFilter :public TypedInputFilter, public LuaExceptionHandlerScope
{
public:
	///\brief Constructor
	///\remark Expects that the lua stack is not modified by anyone but this class in the lifetime after the first call of LuaTableInputFilter::getNext(ElementType&,Element&)
	explicit LuaGeneratorInputFilter( lua_State* ls)
		:LuaExceptionHandlerScope(ls)
		,m_ls(ls)
		,m_thread(m_thread)
		,m_lasttype(InputFilter::Value){}

	///\brief Copy constructor
	///\param[in] o lua generator input filter to copy
	LuaGeneratorInputFilter( const LuaGeneratorInputFilter& o)
		:TypedInputFilter(o)
		,LuaExceptionHandlerScope(o)
		,m_ls(o.m_ls)
		,m_thread(o.m_thread)
		,m_threadref(o.m_threadref)
		,m_lasttype(o.m_lasttype)
		,m_attrvalue(o.m_attrvalue){}

	///\brief Destructor
	///\remark Leaves the generator function as top element (-1) on the lua stack
	virtual ~LuaGeneratorInputFilter();

	///\brief Implementation of TypedInputFilter::getNext(ElementType&,Element&)
	///\remark Expects the generator function to iterate on as top element (-1) on the lua stack when called the first time
	virtual bool getNext( ElementType& type, Element& element);
private:
	bool getValue( int idx, Element& e);	//< fetch the element with index 'idx' as atomic value

private:
	lua_State* m_ls;			//< lua state
	lua_State* m_thread;			//< stack of iterator states
	int m_threadref;			//< reference to thread object to prevent garbage collecting
	ElementType m_lasttype;			//< last element type
	Element m_attrvalue;			//< cached attribute value
};

}}//namespace
#endif
#endif


