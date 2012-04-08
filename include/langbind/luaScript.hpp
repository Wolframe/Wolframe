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
///\file luaScript.hpp
///\brief interface for the scripting language Lua
#ifndef _Wolframe_langbind_LUASCRIPT_HPP_INCLUDED
#define _Wolframe_langbind_LUASCRIPT_HPP_INCLUDED
#include <string>
#include <map>
#include <vector>
#include <stdexcept>
#if WITH_LUA
extern "C" {
	#include "lua.h"
}

namespace _Wolframe {
namespace langbind {

class LuaScript
{
public:
	LuaScript( const char* path_);
	LuaScript( const LuaScript& o)
		:m_path(o.m_path),m_content(o.m_content){}
	~LuaScript(){}

	const std::string& path() const		{return m_path;}
	const std::string& content() const	{return m_content;}
private:
	std::string m_path;
	std::string m_content;
};

class LuaScriptInstance
{
public:
	LuaScriptInstance( const LuaScript* script);
	LuaScriptInstance( const LuaScriptInstance& o)
		:m_ls(o.m_ls),m_script(o.m_script){}
	~LuaScriptInstance();

	lua_State* ls()				{return m_ls;}
private:
	lua_State* m_ls;
	const LuaScript* m_script;
};


///\class LuaFunctionMap
///\brief Map of available Lua functions
class LuaFunctionMap
{
public:
	LuaFunctionMap(){}
	~LuaFunctionMap(){}

	void defineLuaFunction( const char* procname, const char* scriptpath);
	bool getLuaScriptInstance( const char* procname, LuaScriptInstance& rt) const;
private:
	std::vector<LuaScript> m_ar;
	std::map<std::string,std::size_t> m_pathmap;
	std::map<std::string,std::size_t> m_procmap;
};
}}//namespace

#else
namespace _Wolframe {
namespace langbind {

struct LuaScriptInstance {};
struct LuaFunctionMap
{
	void defineLuaFunction( const char*, const char*)
	{
		throw std::runtime_error("Wolframe built without lua support");
	}
	bool getLuaScriptInstance( const char*, LuaScriptInstance&) const
	{
		return false;
	}
};
}}//namespace

#endif
#endif
