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
///\file langbind_luaException.cpp
///\brief implementation of throwing C++ exceptions for lua functions called in a C++ context (outside the interpreter)
#include "langbind/luaException.hpp"
#include <stdexcept>
#include <cstring>

extern "C" {
	#include <lualib.h>
	#include <lauxlib.h>
	#include <lua.h>
}

using namespace _Wolframe;
using namespace langbind;

void LuaErrorMessage::init( const char* funcname, const char* msg)
{
	std::size_t nn = std::strlen( msg);
	if (nn >= bufsize)
	{
		nn = bufsize-1;
	}
	std::memcpy( m_buf, msg, nn);
	m_buf[ nn] = '\0';

	nn = std::strlen( funcname);
	if (nn >= funcnamesize)
	{
		nn = funcnamesize-1;
	}
	std::memcpy( m_funcname, funcname, nn);
	m_funcname[ nn] = '\0';
}

static int luaException( lua_State* ls)
{
	const char* errmsg = lua_tostring( ls, -1);
	throw std::runtime_error( errmsg?errmsg:"unspecified lua exception");
	return 0;
}

LuaExceptionHandlerScope::LuaExceptionHandlerScope( lua_State* ls)
	:m_ls(ls)
	,m_panicf( lua_atpanic( ls, luaException))
{}

LuaExceptionHandlerScope::~LuaExceptionHandlerScope()
{
	lua_atpanic( m_ls, m_panicf);
}


