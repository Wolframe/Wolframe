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
///\file filter/luafilter.hpp
///\brief Interface for a lua table as filter

#ifndef _Wolframe_LUA_FILTER_HPP_INCLUDED
#define _Wolframe_LUA_FILTER_HPP_INCLUDED
#include "filter/typedfilter.hpp"
#include "langbind/luaException.hpp"
#include <vector>

#if WITH_LUA
extern "C" {
	#include "lua.h"
}

namespace _Wolframe {
namespace langbind {

///\class class LuaInputFilter
///\brief Lua table as typed input filter
class LuaInputFilter :public TypedInputFilter, public LuaExceptionHandlerScope
{
public:
	///\brief Constructor
	///\remark Expects the table to iterate as top element (-1) on the lua stack
	///\remark Expects that the lua stack is not modified by anyone but this class in its lifetime
	LuaInputFilter( lua_State* ls);

	///\brief Copy constructor
	LuaInputFilter( const LuaInputFilter& o)
		:LuaExceptionHandlerScope(o)
		,m_ls(o.m_ls)
		,m_stk(o.m_stk){}

	///\brief Destructor
	///\remark Leaves the iterated table as top element (-1) on the lua stack
	virtual ~LuaInputFilter(){}

	///\brief Implementation of TypedInputFilter::getNext(ElementType&,Element&)
	virtual bool getNext( ElementType& type, Element& element);

private:
	///\class FetchState
	///\brief Internal state of the iterator
	struct FetchState
	{
		enum Id
		{
			Init,			//< init state. The element is without key the top level element on the lua stack
			TableIterOpen,		//< the lua table iterator is on an key value pair and did not return yet the opening tag
			TableIterValue,		//< the opening tag has been fetched and the value is to fetch next or a new iterator has to be opened and pushed on the stack
			TableIterClose,		//< the value has been processed and the close tag is to fetch next
			TableIterNext,		//< the close tag has been fetched and a skip to the next element has to be done
			Done			//< finishing operation for this state
		};
		Id id;				//< state identifier
		const char* tag;		//< caller tag, used enclosing tag by arrays
	};

	bool getValue( int idx, Element& e);	//< fetch the element with index 'idx' as atomic value
	bool firstTableElem( const char* tag);	//< opens a new table iterator on an array or lua table
	bool nextTableElem();			//< fetches the next element of the currently iterated array or lua table

private:
	lua_State* m_ls;			//< lua state
	std::vector<FetchState> m_stk;		//< stack of iterator states
};


///\class class LuaOutputFilter
///\brief Lua table as typed output filter
class LuaOutputFilter :public TypedOutputFilter, public LuaExceptionHandlerScope
{
public:
	///\brief Constructor
	///\remark Creates the output filter result as top element (-1) on the lua stack
	///\remark Expects that the lua stack is not modified by anyone but this class in its lifetime
	LuaOutputFilter( lua_State* ls);

	///\brief Copy constructor
	LuaOutputFilter( const LuaOutputFilter& o)
		:LuaExceptionHandlerScope(o)
		,m_ls(o.m_ls)
		,m_depth(o.m_depth)
		,m_type(o.m_type){}

	///\brief Destructor
	virtual ~LuaOutputFilter(){}

	///\brief Implementation of TypedOutputFilter::print( ElementType type, const Element& element)
	virtual bool print( ElementType type, const Element& element);

private:
	bool pushValue( const Element& element);
	bool openTag( const Element& element);
	bool closeTag();
	bool closeAttribute( const Element& element);

private:
	lua_State* m_ls;
	int m_depth;
	ElementType m_type;
};

}}//namespace
#endif
#endif


