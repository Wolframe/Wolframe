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
///\file langbind/luaGetFunctionClosure.hpp
///\brief Interface to the Closure of the :get int the lua binding
#ifndef _Wolframe_langbind_LUA_GET_FUNCTION_CLOSURE_HPP_INCLUDED
#define _Wolframe_langbind_LUA_GET_FUNCTION_CLOSURE_HPP_INCLUDED
#include "langbind/appObjects.hpp"
#include "filter/inputfilterScope.hpp"
#include "filter/typedfilterScope.hpp"
#include "types/countedReference.hpp"
#include <boost/shared_ptr.hpp>

#if WITH_LUA
extern "C" {
	#include "lua.h"
}
#else
#error Lua support not enabled
#endif

namespace _Wolframe {
namespace langbind {

///\class InputFilterClosure
///\brief Closure for the iterator on input (in Lua returned by 'input:get()')
class InputFilterClosure
{
public:
	///\enum ItemType
	///\brief Input loop state
	enum ItemType
	{
		EndOfData,	//< End of processed content reached
		Data,		//< normal processing, loop can continue
		DoYield		//< have to yield and request more network input
	};

	///\brief Constructor
	///\param[in] ig input filter reference from input
	explicit InputFilterClosure( const InputFilterR& ig)
		:m_inputfilter(ig)
		,m_type(InputFilter::OpenTag)
		,m_taglevel(0){}

	InputFilterClosure( const InputFilterClosure& o)
		:m_inputfilter(o.m_inputfilter)
		,m_type(o.m_type)
		,m_attrbuf(o.m_attrbuf)
		,m_taglevel(o.m_taglevel){}

	///\brief Get the next pair of elements on the lua stack if possible
	ItemType fetch( lua_State* ls);

	const InputFilterR& inputfilter() const			{return m_inputfilter;}

	///\brief Find out if we can use the function in this state as argument of a function
	///\return true, if yes
	bool isValidAsScope() const				{return (m_type==InputFilter::OpenTag);}

	InputFilterClosure scope()
	{
		if (m_taglevel < 0) return InputFilterClosure( InputFilterR( new InputFilterScope()));
		--m_taglevel; //consumed by returned scope
		return InputFilterClosure( InputFilterR( new InputFilterScope( inputfilter())));
	}

private:
	InputFilterR m_inputfilter;				//< rerefence to input with filter
	InputFilter::ElementType m_type;			//< current state (last value type parsed)
	std::string m_attrbuf;					//< buffer for attribute name
	int m_taglevel;						//< tag level
};


///\class TypedInputFilterClosure
///\brief Closure for the iterator on a structure (in Lua returned by 'form:get()')
class TypedInputFilterClosure
{
public:
	///\enum ItemType
	///\brief Input loop state
	enum ItemType
	{
		EndOfData,	//< End of processed content reached
		Data,		//< normal processing, loop can continue
		DoYield		//< have to yield and request more network input
	};

	///\brief Constructor
	///\param[in] i input filter reference
	explicit TypedInputFilterClosure( const TypedInputFilterR& i)
		:m_inputfilter(i)
		,m_type(InputFilter::OpenTag)
		,m_taglevel(0){}

	TypedInputFilterClosure( const TypedInputFilterClosure& o)
		:m_inputfilter(o.m_inputfilter)
		,m_type(o.m_type)
		,m_attrbuf(o.m_attrbuf)
		,m_taglevel(o.m_taglevel){}

	///\brief Get the next pair of elements on the lua stack if possible
	///\return state returned
	ItemType fetch( lua_State* ls);

	const TypedInputFilterR& inputfilter() const		{return m_inputfilter;}

	///\brief Find out if we can use the function in this state as argument of a function
	///\return true, if yes
	bool isValidAsScope() const				{return (m_type==InputFilter::OpenTag);}

	TypedInputFilterClosure scope()
	{
		if (m_taglevel < 0) return TypedInputFilterClosure( TypedInputFilterR( new TypedInputFilterScope()));
		--m_taglevel; //consumed by returned scope
		return TypedInputFilterClosure( TypedInputFilterR( new TypedInputFilterScope( inputfilter())));
	}

private:
	TypedInputFilterR m_inputfilter;			//< rerefence to input with filter
	InputFilter::ElementType m_type;			//< current state (last value type parsed)
	std::string m_attrbuf;					//< buffer for attribute name
	int m_taglevel;						//< tag level
};

}}//namespace
#endif
