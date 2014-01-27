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
///\file module/luaExtensionBuilder.hpp
///\brief Interface template for object builder of form functions
#ifndef _Wolframe_MODULE_LUA_LANGUAGE_EXTENSION_OBJECT_BUILDER_TEMPLATE_HPP_INCLUDED
#define _Wolframe_MODULE_LUA_LANGUAGE_EXTENSION_OBJECT_BUILDER_TEMPLATE_HPP_INCLUDED
#include "processor/moduleInterface.hpp"
#include "module/constructor.hpp"
#include <string>
#include <cstring>
#include <stdexcept>
#if WITH_LUA
extern "C" {
	#include "lua.h"
}
#else
#error Lua support not enabled
#endif

namespace _Wolframe {
namespace module {

class LuaExtensionConstructor :public SimpleObjectConstructor< lua_CFunction >
{
public:
	LuaExtensionConstructor( const char* className_, const char* moduleName_, lua_CFunction func_)
		:m_className(className_)
		,m_moduleName(moduleName_)
		,m_func(func_) {}

	virtual ~LuaExtensionConstructor(){}

	virtual ObjectConstructorBase::ObjectType objectType() const
	{
		return LANGUAGE_EXTENSION_OBJECT;
	}
	virtual const char* objectClassName() const
	{
		return m_className.c_str();
	}
	virtual lua_CFunction object() const
	{
		return m_func;
	}
	const std::string& moduleName()
	{
		return m_moduleName;
	}

private:
	std::string m_className;
	std::string m_moduleName;
	lua_CFunction m_func;
};

class LuaExtensionBuilder :public SimpleBuilder
{
public:
	LuaExtensionBuilder( const char* className_, lua_CFunction func_)
		:SimpleBuilder( className_)
		,m_func(func_)
	{
		if (!classNameMatches( className_))
		{
			throw std::logic_error( "lua language extension module does not have prefix 'LuaExtension:'");
		}
	}

	virtual ~LuaExtensionBuilder(){}

	virtual ObjectConstructorBase::ObjectType objectType() const
	{
		return ObjectConstructorBase::LANGUAGE_EXTENSION_OBJECT;
	}
	virtual ObjectConstructorBase* constructor()
	{
		return new LuaExtensionConstructor( m_className, strchr(m_className, ':')+1, m_func);
	}

	static bool classNameMatches( const char* className_)
	{
		static const char* prefix = "LuaExtension:";
		const char* cc = std::strchr( className_, ':');
		return (cc && memcmp( className_, prefix, cc-className_+1) == 0);
	}
private:
	lua_CFunction m_func;
};

}}//namespace

#endif

