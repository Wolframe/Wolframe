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
///\file luaGetFunctionClosure.cpp
///\brief Interface to the Closure of the :get int the lua binding

#include "luaGetFunctionClosure.hpp"
#include "luaException.hpp"

using namespace _Wolframe;
using namespace _Wolframe::langbind;

static void push_element( lua_State* ls, const void* element, std::size_t elementsize)
{
	lua_pushlstring( ls, (const char*)element, elementsize);
	lua_tostring( ls, -1); //PF:BUGFIX lua 5.1.4 needs this one
}

InputFilterClosure::ItemType InputFilterClosure::fetch( lua_State* ls)
{
	const void* element;
	std::size_t elementsize;
	InputFilter::ElementType elemtype;

	if (!m_inputfilter.get() || m_taglevel < 0)
	{
		return EndOfData;
	}

AGAIN:
	if (!m_inputfilter->getNext( elemtype, element, elementsize))
	{
		switch (m_inputfilter->state())
		{
			case InputFilter::EndOfMessage:
				return DoYield;

			case InputFilter::Error:
				throw std::runtime_error( m_inputfilter->getError()?m_inputfilter->getError():"unknown error in input filter");

			case InputFilter::Open:
				return EndOfData;
		}
		throw std::runtime_error( "illegal state in input filter");
	}
	else
	{
		switch (elemtype)
		{
			case InputFilter::OpenTag:
				lua_pushboolean( ls, 0);
				push_element( ls, element, elementsize);
				m_type = elemtype;
				++m_taglevel;
				return Data;

			case InputFilter::Value:
				if (m_type == InputFilter::Attribute)
				{
					push_element( ls, element, elementsize);
					push_element( ls, m_attrbuf.c_str(), m_attrbuf.size());
					m_attrbuf.clear();
					m_type = InputFilter::OpenTag;
				}
				else
				{
					push_element( ls, element, elementsize);
					lua_pushboolean( ls, 0);
					m_type = elemtype;
				}
				return Data;

			 case InputFilter::Attribute:
				m_attrbuf.clear();
				m_attrbuf.append( (const char*)element, elementsize);
				m_type = elemtype;
				goto AGAIN;

			 case InputFilter::CloseTag:
				if (m_taglevel == 0)
				{
					return EndOfData;
				}
				m_type = elemtype;
				--m_taglevel;
				lua_pushboolean( ls, 0);
				lua_pushboolean( ls, 0);
				return Data;
		}
	}
	throw std::runtime_error( "illegal state in generator function - typed fetch");
}


static void push_element( lua_State* ls, const types::VariantConst& element)
{
	switch (element.type())
	{
		case types::Variant::Null:
			lua_pushnil( ls);
			break;
		case types::Variant::Custom:
		{
			types::Variant baseval;
			try
			{
				element.customref()->getBaseTypeValue( baseval);
				if (baseval.type() != types::Variant::Custom)
				{
					push_element( ls, baseval);
					break;
				}
			}
			catch (const std::runtime_error& e)
			{
				throw std::runtime_error( std::string("cannot convert value to base type: ") + e.what());
			}
			throw std::runtime_error( "cannot convert value to base type");
		}
		case types::Variant::Bool:
			lua_pushboolean( ls, element.tobool());
			break;
		case types::Variant::Double:
			lua_pushnumber( ls, (lua_Number)element.todouble());
			break;
		case types::Variant::Int:
			lua_pushinteger( ls, (lua_Integer)element.toint());
			break;
		case types::Variant::UInt:
			lua_pushinteger( ls, (lua_Integer)element.touint());
			break;
		case types::Variant::Timestamp:
		{
			LuaExceptionHandlerScope escope(ls);
			{
				std::string strval = element.tostring();
				lua_pushlstring( ls, strval.c_str(), strval.size());
				lua_tostring( ls, -1); //PF:BUGFIX lua 5.1.4 needs this one
			}
			break;
		}
		case types::Variant::BigNumber:
		{
			LuaExceptionHandlerScope escope(ls);
			{
				std::string strval = element.tostring();
				lua_pushlstring( ls, strval.c_str(), strval.size());
				lua_tostring( ls, -1); //PF:BUGFIX lua 5.1.4 needs this one
			}
			break;
		}
		case types::Variant::String:
			lua_pushlstring( ls, element.charptr(), element.charsize());
			lua_tostring( ls, -1); //PF:BUGFIX lua 5.1.4 needs this one
			break;
		default:
			lua_pushnil( ls);
			break;
	}
}

TypedInputFilterClosure::ItemType TypedInputFilterClosure::fetch( lua_State* ls)
{
	InputFilter::ElementType elemtype;
	types::VariantConst element;

	if (!m_inputfilter.get() || m_taglevel < 0)
	{
		return EndOfData;
	}

AGAIN:
	if (!m_inputfilter->getNext( elemtype, element))
	{
		switch (m_inputfilter->state())
		{
			case InputFilter::EndOfMessage:
				return DoYield;

			case InputFilter::Error:
				throw std::runtime_error( m_inputfilter->getError()?m_inputfilter->getError():"unknown error in input filter");

			case InputFilter::Open:
				return EndOfData;
		}
		throw std::runtime_error( "illegal state in input filter closure");
	}
	else
	{
		switch (elemtype)
		{
			case InputFilter::OpenTag:
				lua_pushboolean( ls, 0);
				push_element( ls, element);
				m_type = elemtype;
				++m_taglevel;
				return Data;

			case InputFilter::Value:
				if (m_type == InputFilter::Attribute)
				{
					push_element( ls, element);
					push_element( ls, m_attrbuf.c_str(), m_attrbuf.size());
					m_attrbuf.clear();
					m_type = InputFilter::OpenTag;
				}
				else
				{
					push_element( ls, element);
					lua_pushboolean( ls, 0);
					m_type = elemtype;
				}
				return Data;

			 case InputFilter::Attribute:
				m_attrbuf = element.tostring();
				m_type = elemtype;
				goto AGAIN;

			 case InputFilter::CloseTag:
				if (m_taglevel == 0)
				{
					--m_taglevel;
					return EndOfData;
				}
				m_type = elemtype;
				--m_taglevel;
				lua_pushboolean( ls, 0);
				lua_pushboolean( ls, 0);
				return Data;
		}
	}
	throw std::runtime_error( "illegal state in generator function - typed fetch");
}


