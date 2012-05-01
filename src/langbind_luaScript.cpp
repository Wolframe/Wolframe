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
///\file langbind/luaScript.cpp
///\brief implementation of a Lua script
#include "langbind/luaScript.hpp"
#include "langbind/luaDebug.hpp"
#include "logger-v1.hpp"
#include <algorithm>
#include <string>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <cctype>
extern "C" {
	#include <lualib.h>
	#include <lauxlib.h>
	#include <lua.h>
}

using namespace _Wolframe;
using namespace langbind;

static int function_printlog( lua_State *ls)
{
	/* first parameter maps to a log level, rest gets printed depending on
	 * whether it's a string or a number
	 */
	int ii,nn = lua_gettop(ls);
	if (nn <= 0)
	{
		LOG_ERROR << "no arguments passed to 'printlog'";
		return 0;
	}
	const char *logLevel = luaL_checkstring( ls, 1);
	std::string logmsg;

	for (ii=2; ii<=nn; ii++)
	{
		if (!getDescription( ls, ii, logmsg))
		{
			LOG_ERROR << "failed to map 'printLog' arguments to a string";
		}
	}
	_Wolframe::log::LogLevel::Level lv = _Wolframe::log::LogLevel::strToLogLevel( logLevel);
	if (lv == _Wolframe::log::LogLevel::LOGLEVEL_UNDEFINED)
	{
		LOG_ERROR << "'printLog' called with undefined loglevel '" << logLevel << "' as first argument";
	}
	else
	{
		_Wolframe::log::Logger( _Wolframe::log::LogBackend::instance() ).Get( lv )
			<< _Wolframe::log::LogComponent::LogLua
			<< logmsg;
	}
	return 0;
}

LuaScript::LuaScript( const char* path_)
	:m_path(path_)
{
	char buf;
	std::fstream ff;
	ff.open( path_, std::ios::in);
	while (ff.read( &buf, sizeof(buf)))
	{
		m_content.push_back( buf);
	}
	if ((ff.rdstate() & std::ifstream::eofbit) == 0)
	{
		LOG_ERROR << "failed to read lua script from file: '" << path_ << "'";
		throw std::runtime_error( "read lua script from file");
	}
	ff.close();
}

LuaScriptInstance::LuaScriptInstance( const LuaScript* script_)
	:m_ls(0),m_thread(0),m_threadref(0),m_script(script_)
{
	m_ls = luaL_newstate();
	if (!m_ls) throw std::runtime_error( "failed to create lua state");

	// create thread and prevent garbage collecting of it (http://permalink.gmane.org/gmane.comp.lang.lua.general/22680)
	m_thread = lua_newthread( m_ls);
	lua_pushvalue( m_ls, -1);
	m_threadref = luaL_ref( m_ls, LUA_REGISTRYINDEX);

	if (luaL_loadbuffer( m_ls, m_script->content().c_str(), m_script->content().size(), m_script->path().c_str()))
	{
		std::ostringstream buf;
		buf << "Failed to load script '" << m_script->path() << "':" << lua_tostring( m_ls, -1);
		throw std::runtime_error( buf.str());
	}
	// open standard lua libraries
	luaL_openlibs( m_ls);

	// register logging function already here because then it can be used in the script initilization part
	lua_pushcfunction( m_ls, &function_printlog);
	lua_setglobal( m_ls, "printlog");

	// open additional libraries defined for this script
	std::vector<LuaScript::Module>::const_iterator ii=m_script->modules().begin(), ee=m_script->modules().end();
	for (;ii!=ee; ++ii)
	{
		if (ii->m_initializer( m_ls))
		{
			std::ostringstream buf;
			buf << "module '" << ii->m_name << "' initialization failed: " << lua_tostring( m_ls, -1);
			throw std::runtime_error( buf.str());
		}
	}

	// call main, we may have to initialize LUA modules there
	if (lua_pcall( m_ls, 0, LUA_MULTRET, 0) != 0)
	{
		std::ostringstream buf;
		buf << "Unable to call main entry of script: " << lua_tostring( m_ls, -1 );
		throw std::runtime_error( buf.str());
	}
}

LuaScriptInstance::~LuaScriptInstance()
{
	if (m_ls)
	{
		luaL_unref( m_ls, LUA_REGISTRYINDEX, m_threadref);
		lua_close( m_ls);
	}
}

LuaFunctionMap::~LuaFunctionMap()
{
	std::vector<LuaScript*>::iterator ii=m_ar.begin(),ee=m_ar.end();
	while (ii != ee)
	{
		delete *ii;
		++ii;
	}
}

void LuaFunctionMap::defineLuaFunction( const char* name, const LuaScript& script)
{
	std::string nam( name);
	std::transform( nam.begin(), nam.end(), nam.begin(), (int(*)(int)) std::tolower);
	{
		std::map<std::string,std::size_t>::const_iterator ii=m_procmap.find( nam),ee=m_procmap.end();
		if (ii != ee)
		{
			std::ostringstream buf;
			buf << "Duplicate definition of function '" << nam << "'";
			throw std::runtime_error( buf.str());
		}
	}
	std::size_t scriptId;
	std::map<std::string,std::size_t>::const_iterator ii=m_pathmap.find( script.path()),ee=m_pathmap.end();
	if (ii != ee)
	{
		scriptId = ii->second;
	}
	else
	{
		scriptId = m_ar.size();
		m_ar.push_back( new LuaScript( script));	//< load its content from file
		LuaScriptInstance( m_ar.back());		//< check, if it can be compiled
		m_pathmap[ script.path()] = scriptId;
	}
	m_procmap[ nam] = scriptId;
}

bool LuaFunctionMap::getLuaScriptInstance( const char* procname, LuaScriptInstanceR& rt) const
{
	std::string nam( procname);
	std::transform( nam.begin(), nam.end(), nam.begin(), (int(*)(int)) std::tolower);

	std::map<std::string,std::size_t>::const_iterator ii=m_procmap.find( nam),ee=m_procmap.end();
	if (ii == ee) return false;
	rt = LuaScriptInstanceR( new LuaScriptInstance( m_ar[ ii->second]));
	return true;
}

