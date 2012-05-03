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
#include "cmdbind/luaCommandHandler.hpp"
#include "langbind/luaDebug.hpp"
#include "langbind/appObjects.hpp"
#include "langbind/luaObjects.hpp"
#include "logger-v1.hpp"
#include <stdexcept>
#include <cstddef>
#include <boost/lexical_cast.hpp>

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

using namespace _Wolframe;
using namespace langbind;
using namespace cmdbind;

LuaCommandHandler::CallResult LuaCommandHandler::call( const char*& errorCode)
{
	int rt = 0;
	errorCode = 0;

	if (!m_interp.get())
	{
		try
		{
			m_interp = createLuaScriptInstance( m_name.c_str(), m_inputfilter, m_outputfilter);
		}
		catch (const std::exception& e)
		{
			LOG_ERROR << "Failed to load script and initialize execution context: " << e.what();
			errorCode = "init script";
			return Error;
		}
		// call the function (for the first time)
		lua_getglobal( m_interp->thread(), m_name.c_str());
		std::vector<std::string>::const_iterator itr=m_argBuffer.begin(),end=m_argBuffer.end();
		for (;itr != end; ++itr)
		{
			lua_pushlstring( m_interp->thread(), itr->c_str(), itr->size());
		}
		rt = lua_resume( m_interp->thread(), NULL, m_argBuffer.size());
	}
	else
	{
		// call the function (subsequently until termination)
		rt = lua_resume( m_interp->thread(), NULL, 0);
	}
	if (rt == LUA_YIELD)
	{
		return Yield;
	}
	else if (rt != 0)
	{
		const char* msg = lua_tostring( m_interp->thread(), -1);
		LOG_ERROR << "error calling function '" << m_name.c_str() << "':" << msg;
		errorCode = "lua call failed";
		return Error;
	}
	return Ok;
}


