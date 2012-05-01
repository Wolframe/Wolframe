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
#include "countedReference.hpp"
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
private:
	struct Module
	{
		std::string m_name;
		lua_CFunction m_initializer;

		Module( const Module& o)				:m_name(o.m_name),m_initializer(o.m_initializer){}
		Module( const std::string& n, const lua_CFunction f)	:m_name(n),m_initializer(f){}
	};

public:
	LuaScript( const char* path_);
	LuaScript( const LuaScript& o)
		:m_modules(o.m_modules),m_path(o.m_path),m_content(o.m_content){}
	~LuaScript(){}

	void addModule( const std::string& n, lua_CFunction f)		{m_modules.push_back( Module( n, f));}

	const std::vector<Module>& modules() const			{return m_modules;}
	const std::string& path() const					{return m_path;}
	const std::string& content() const				{return m_content;}

private:
	std::vector<Module> m_modules;
	std::string m_path;
	std::string m_content;
};

class LuaScriptInstance
{
public:
	explicit LuaScriptInstance( const LuaScript* script);
	~LuaScriptInstance();

	lua_State* ls()				{return m_ls;}
	lua_State* thread()			{return m_thread;}
private:
	lua_State* m_ls;
	lua_State* m_thread;
	int m_threadref;
	const LuaScript* m_script;

private:
	LuaScriptInstance( const LuaScriptInstance&){}
};

typedef CountedReference<LuaScriptInstance> LuaScriptInstanceR;


///\class LuaFunctionMap
///\brief Map of available Lua functions
class LuaFunctionMap
{
public:
	LuaFunctionMap(){}
	~LuaFunctionMap();

	void defineLuaFunction( const char* procname, const LuaScript& script);
	bool getLuaScriptInstance( const char* procname, LuaScriptInstanceR& rt) const;
private:
	LuaFunctionMap( const LuaFunctionMap&){}

private:
	std::vector<LuaScript*> m_ar;
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
	void defineLuaFunction( const char*, const LuaScript&)
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
