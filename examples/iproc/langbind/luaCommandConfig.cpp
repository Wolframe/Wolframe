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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
#include "luaCommandConfig.hpp"
#include "luaDebug.hpp"
#include <boost/property_tree/ptree.hpp>
#include <boost/algorithm/string.hpp>
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
#include <ostream>
#include "config/ConfigurationTree.hpp"
#include "config/valueParser.hpp"
#include "miscUtils.hpp"

extern "C" {
	#include <lualib.h>
	#include <lauxlib.h>
	#include <lua.h>
}

using namespace _Wolframe;
using namespace _Wolframe::langbind;

static LuaCommandConfig::ModuleLoad getLuaModuleEntryFunc( const char* name)
{
	if (strcmp(name,LUA_TABLIBNAME) == 0) return luaopen_table;
	if (strcmp(name,LUA_IOLIBNAME) == 0) return luaopen_io;
	if (strcmp(name,LUA_OSLIBNAME) == 0) return luaopen_os;
	if (strcmp(name,LUA_STRLIBNAME) == 0) return luaopen_string;
	if (strcmp(name,LUA_MATHLIBNAME) == 0) return luaopen_math;
	if (strcmp(name,LUA_DBLIBNAME) == 0) return luaopen_debug;
	if (strcmp(name,LUA_LOADLIBNAME) == 0) return luaopen_package;
	return 0;
}

void LuaCommandConfig::Module::setType()
{
	if (m_type == LuaCommandConfig::Module::Undefined)
	{
		m_load = getLuaModuleEntryFunc( m_name.c_str());
		if (m_load)
		{
			m_type = PreloadLib;
		}
		else
		{
			m_type = Script;
		}
	}
}

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

bool LuaCommandConfig::Module::load( lua_State* ls) const
{
	if (m_type == PreloadLib && m_load)
	{
		if (m_load == luaopen_table)
		{
			LOG_INFO << "Defined module " << LUA_TABLIBNAME << " in configuration that is loaded by default";
		}
		else
		{
			m_load(ls);
		}
	}
	else if (m_type == UserLib)
	{
		lua_pushcfunction( ls, m_load);
		lua_pushstring( ls, m_name.c_str());
		if (lua_pcall( ls, 1, 1, 0) != 0)
		{
			LOG_ERROR << "Runtime error when loading base module " << m_name << ":" << lua_tostring( ls, -1);
			return false;
		}
	}
	else if (m_type == Script)
	{
		if (luaL_loadfile( ls, m_path.empty()?m_name.c_str():m_path.c_str()))
		{
			LOG_ERROR << "Failed to load script '" << m_name << "':" << lua_tostring( ls, -1);
			lua_pop( ls, 1);
			return false;
		}
		// register logging function already here because then it can be used in the script initilization part
		lua_pushstring( ls, "printlog");
		lua_pushcfunction( ls, &function_printlog);
		lua_settable( ls, LUA_GLOBALSINDEX);

		// call main, we may have to initialize LUA modules there
		if (lua_pcall( ls, 0, LUA_MULTRET, 0) != 0)
		{
			LOG_FATAL << "Unable to call main entry of script: " << lua_tostring( ls, -1 );
			lua_pop( ls, 1 );
			throw new std::runtime_error( "Can't initialize LUA processor" );
		}
	}
	else
	{
		LOG_WARNING << "Could not load module '" << m_name << "'";
	}
	return true;
}

bool LuaCommandConfig::Module::check() const
{
	switch (m_type)
	{
		case PreloadLib:
		case UserLib:
			break;
		case Script:
			if (!boost::filesystem::exists( m_path))
			{
				LOG_ERROR << "Script " << m_name << " ( " << m_path << ") does not exist";
				return false;
			}
			break;
		case Undefined:
			break;
	}
	return true;
}

bool LuaCommandConfig::load( lua_State *ls) const
{
	luaopen_base( ls);
	luaopen_table( ls);

	for (std::vector<Module>::const_iterator it = m_modules.begin(); it != m_modules.end(); it++)
	{
		if (it->type() == Module::PreloadLib)
		{
			if (!it->load( ls)) return false;
		}
	}
	for (std::vector<Module>::const_iterator it = m_modules.begin(); it != m_modules.end(); it++)
	{
		if (it->type() != Module::PreloadLib)
		{
			if (!it->load( ls)) return false;
		}
	}
	if (!m_mainmodule.load( ls)) return false;
	return true;
}

bool LuaCommandConfig::check() const
{
	bool rt = true;
	for (std::vector<Module>::const_iterator it = m_modules.begin(); it != m_modules.end(); it++)
	{
		rt &= it->check();
	}
	return rt;
}

bool LuaCommandConfig::test() const
{
	lua_State *ls = luaL_newstate();
	if (!ls)
	{
		LOG_ERROR << "failed to create lua state in configuration test";
		return false;
	}
	bool rt = load( ls);
	lua_close( ls);
	return rt;
}

void LuaCommandConfig::print( std::ostream& os, size_t /*indent*/) const
{
	os << "Configuration of Lua Script Processor" << ":" << std::endl;
	os << "   Main Script: " << m_mainmodule.name() << " (" << m_mainmodule.path() << ")" << std::endl;
	os << "   Main Function: " << m_main << std::endl;

	if( !m_modules.empty())
	{
		os << "   Modules: ";
		for (std::vector<Module>::const_iterator it = m_modules.begin(); it != m_modules.end(); it++)
		{
			os << "      " << Module::typeName(it->type()) << " " << it->name();
			if (it->type() == Module::Script)
			{
				os << " (" << it->path() << ")";
			}
			os << std::endl;
		}
		os << std::endl;
	}
}


