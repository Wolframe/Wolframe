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
#include "luaconfig.hpp"
#include <boost/property_tree/ptree.hpp>
#include <boost/algorithm/string.hpp>
#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem.hpp>
#include <ostream>
#include "configHelpers.hpp"
#include "miscUtils.hpp"

extern "C" {
	#include <lualib.h>
	#include <lauxlib.h>
	#include <lua.h>
}

typedef int (*LuaModuleEntryFunc)( lua_State *l);
static LuaModuleEntryFunc getLuaModuleEntryFunc( const char* name)
{
	if (strcmp(name,"base") == 0) return luaopen_base;
	if (strcmp(name,LUA_TABLIBNAME) == 0) return luaopen_table;
	if (strcmp(name,LUA_IOLIBNAME) == 0) return luaopen_io;
	if (strcmp(name,LUA_OSLIBNAME) == 0) return luaopen_os;
	if (strcmp(name,LUA_STRLIBNAME) == 0) return luaopen_string;
	if (strcmp(name,LUA_MATHLIBNAME) == 0) return luaopen_math;
	if (strcmp(name,LUA_DBLIBNAME) == 0) return luaopen_debug;
	if (strcmp(name,LUA_LOADLIBNAME) == 0) return luaopen_package;
	return 0;
}


using namespace _Wolframe::mtproc::lua;

bool LuaConfiguration::parse( const boost::property_tree::ptree::const_iterator parentNode, const std::string& nodeName)
{
	std::string name;
	unsigned int cnt_main = 0;

	if (!boost::algorithm::iequals( parentNode->first, nodeName))
	{
		LOG_ERROR << displayName() << ": got different configuration than expected ('" << parentNode->first << "' instead of '" << nodeName << "'";
		return false;
	}
	for ( boost::property_tree::ptree::const_iterator it = parentNode->second.begin(); it != parentNode->second.end(); it++)
	{
		if (boost::algorithm::iequals( it->first, "main"))
		{
			if (!Configuration::getStringValue( it, displayName(), name)) return false;
			if (name.size() == 0)
			{
				LOG_ERROR << displayName() << ": empty name for the main script is illegal (configuration option <main>)";
				return false;
			}
			m_main = Module( name);
			cnt_main ++;
		}
		else if (boost::algorithm::iequals( it->first, "module"))
		{
			if (!Configuration::getStringValue( it, displayName(), name)) return false;
			if (name.size() == 0)
			{
				LOG_ERROR << displayName() << ": empty name for a module is illegal (configuration option <module>)";
				return false;
			}
			m_modules.push_back( Module( name));
		}
		else
		{
			LOG_WARNING << displayName() << ": unknown configuration option: <" << it->first << ">";
			return false;
		}
	}
	if (cnt_main == 0)
	{
		LOG_ERROR << displayName() << ": main script to execute is not defined (configuration option <module>";
		return false;
	}
	return true;
}

void LuaConfiguration::Module::setCanonicalPath( const std::string& refPath)
{
	if (!getLuaModuleEntryFunc( m_name.c_str()))
	{
		m_type = PreloadLib;
	}
	else
	{
		m_type = Script;
		boost::filesystem::path pt(m_name);
		if (pt.is_absolute())
		{
			m_path = resolvePath(m_name);
		}
		else
		{
			m_path = resolvePath( boost::filesystem::absolute( m_name, boost::filesystem::path( refPath).branch_path()).string());
		}
		m_name = pt.leaf().string();
	}
}

void LuaConfiguration::setCanonicalPathes( const std::string& refPath)
{
	m_main.setCanonicalPath( refPath);
	for (std::list<Module>::iterator it = m_modules.begin(); it != m_modules.end(); it++)
	{
		if (it->type() != Module::Undefined)
		{
			LOG_ERROR << displayName() << ": canonical path set twice, second definition ignored";
		}
		else
		{
			it->setCanonicalPath( refPath);
		}
	}
}


bool LuaConfiguration::load( lua_State *ls) const
{
	for (std::list<Module>::const_iterator it = m_modules.begin(); it != m_modules.end(); it++)
	{
		if (it->type() == Module::PreloadLib)
		{
			LuaModuleEntryFunc ef;
			if (it->path().size() == 0)
			{
				ef = getLuaModuleEntryFunc( it->name().c_str());
				lua_pushcfunction( ls, ef);
				lua_pushstring( ls, it->name().c_str());
				if (lua_pcall( ls, 1, 1, 0) != 0)
				{
					LOG_ERROR << "Runtime error when loading base module " << it->name() << ":" << lua_tostring( ls, -1);
					return false;
				}
			}
		}
	}
	for (std::list<Module>::const_iterator it = m_modules.begin(); it != m_modules.end(); it++)
	{
		if (it->type() == Module::Script)
		{
			if (luaL_loadfile( ls, it->name().c_str()))
			{
				LOG_ERROR << "Syntax error in lua submodule script " << it->name() << ":" << lua_tostring( ls, -1);
				lua_pop( ls, 1);
				return false;
			}

		}
	}
	if (luaL_loadfile( ls, m_main.name().c_str()))
	{
		LOG_ERROR << "Syntax error in lua main script " << m_main.name() << ":" << lua_tostring( ls, -1);
		lua_pop( ls, 1);
		return false;
	}
	return true;
}

bool LuaConfiguration::check() const
{
	lua_State *ls = luaL_newstate();
	if (!ls)
	{
		LOG_ERROR << "failed to create lua state in configuration check";
		return false;
	}
	bool rt = load( ls);
	lua_close( ls);
	return rt;
}

void LuaConfiguration::print( std::ostream& os)
{
	os << "Configuration of " << displayName() << ":" << std::endl;
	os << "   Main Script: " << m_main.name() << " (" << m_main.path() << ")" << std::endl;

	if( !m_modules.empty())
	{
		os << "   Modules: ";
		for (std::list<Module>::const_iterator it = m_modules.begin(); it != m_modules.end(); it++)
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


