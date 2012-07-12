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
///\file langbind/luaException.hpp
///\brief interface for throwing C++ exceptions for lua functions called in a C++ context (outside the interpreter)
#ifndef _Wolframe_langbind_LUA_EXCEPTION_HPP_INCLUDED
#define _Wolframe_langbind_LUA_EXCEPTION_HPP_INCLUDED
#include <sstream>
#include <iostream>

extern "C" {
	#include <lua.h>
	#include <lauxlib.h>
}

namespace _Wolframe {
namespace langbind {

///\class LuaErrorMessage
///\brief Buffer for error message in Lua scope (not leaking when a lua error exception longjump happens)
class LuaErrorMessage
{
public:
	LuaErrorMessage()
	{
		m_buf[0] = '\0';
	}

	void init( const char* funcname, const char* msg);

	int luaerror( lua_State* ls) const
	{
		return luaL_error( ls, "error in function '%s' (%s)", m_funcname, m_buf);
	}
private:
	enum {bufsize=224,funcnamesize=32};
	char m_buf[ bufsize];
	char m_funcname[ funcnamesize];
};

///\brief Class for calling C++ in a potentially exception throwing context remapping throws to lua errors with complete clean up
template <class Functor>
struct LuaFunctionCppCall
{
	static int run( const char* name, lua_State* ls)
	{
		LuaErrorMessage luaerr;
		try
		{
			return Functor::call( ls);
		}
		catch (std::exception& e)
		{
			luaerr.init( name, e.what());
		}
		return luaerr.luaerror( ls);
	}
};

///\class LuaExceptionHandlerScope
///\brief Class for calling a Lua function in a C++ scope. Lua longjumps are catched and translated to C++ exceptions
class LuaExceptionHandlerScope
{
public:
	explicit LuaExceptionHandlerScope( lua_State* ls);
	LuaExceptionHandlerScope( const LuaExceptionHandlerScope& o)
		:m_panicf(o.m_panicf){}
	~LuaExceptionHandlerScope();
private:
	lua_State* m_ls;
	lua_CFunction m_panicf;
};

}}//namespace
#endif




