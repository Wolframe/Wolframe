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
///\file luaCppCall.hpp
///\brief interface for save calling of C++ functions in lua context
#ifndef _Wolframe_LUA_CPP_CALL_HPP_INCLUDED
#define _Wolframe_LUA_CPP_CALL_HPP_INCLUDED
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <cstring>
#include "logger-v1.hpp"

extern "C" {
	#include <lualib.h>
	#include <lauxlib.h>
	#include <lua.h>
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

	void init( const char* funcname, const char* msg)
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

	int luaerror( lua_State* ls) const
	{
		LOG_ERROR << "error in function '" << m_funcname << "' (" << m_buf << ")";
		return luaL_error( ls, "%s", m_buf);
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

}}//namespace
#endif

