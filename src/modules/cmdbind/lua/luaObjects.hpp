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
///\file luaObjects.hpp
///\brief interface to system objects for processor language bindings
#ifndef _Wolframe_langbind_LUA_OBJECTS_HPP_INCLUDED
#define _Wolframe_langbind_LUA_OBJECTS_HPP_INCLUDED
#include "filter/filter.hpp"
#include "langbind/appObjects.hpp"
#include "processor/procProvider.hpp"
#include <map>
#include <list>
#include <boost/shared_ptr.hpp>

extern "C" {
	#include "lua.h"
}

namespace _Wolframe {
namespace langbind {

class LuaModule
{
public:
	LuaModule()						:m_initializer(0){}
	LuaModule( const LuaModule& o)				:m_name(o.m_name),m_initializer(o.m_initializer){}
	LuaModule( const std::string& n, const lua_CFunction f)	:m_name(n),m_initializer(f){}
	void load( lua_State* ls);
private:
	std::string m_name;
	lua_CFunction m_initializer;
};

class LuaScript
{
public:
	LuaScript( const std::string& path_);
	LuaScript( const LuaScript& o)
		:m_functions(o.m_functions)
		,m_path(o.m_path)
		,m_content(o.m_content){}
	~LuaScript(){}

	const std::vector<std::string>& functions() const		{return m_functions;}
	const std::string& path() const					{return m_path;}
	const std::string& content() const				{return m_content;}

private:
	std::vector<std::string> m_functions;
	std::string m_path;
	std::string m_content;
};


///\class LuaModuleMap
///\brief Map of available Lua functions
class LuaModuleMap
{
public:
	LuaModuleMap(){}

	void defineLuaModule( const std::string& name, const LuaModule& script);
	bool getLuaModule( const std::string& name, LuaModule& rt) const;

private:
	std::map<std::string,LuaModule> m_map;
};


class LuaScriptInstance
{
public:
	///\brief Constructor
	///\param[in] script script executed by this instance
	///\param[in] modulemap_ map of modules that can be loaded
	LuaScriptInstance( const LuaScript* script, const LuaModuleMap* modulemap_);
	~LuaScriptInstance();

	lua_State* ls()				{return m_ls;}
	lua_State* thread()			{return m_thread;}
	const LuaScript* script() const		{return m_script;}

	std::string luaErrorMessage( lua_State* ls_, int index=-1);
	std::string luaUserErrorMessage( lua_State* ls_, int index=-1);

	///\brief Create the context for executing a Lua script with all objects initialized
	///\param[in] input_ input definition for the input to process
	///\param[in] output_ output definition for the output to print
	///\param[in] provider_ processor provider for allocation of objects accessed
	void init( const Input& input_, const Output& output_, const proc::ProcessorProvider* provider_);

	///\brief Create the context for executing a Lua script without input/output but all other objects initialized
	///\param[in] provider_ processor provider for allocation of objects accessed
	void init( const proc::ProcessorProvider* provider_);

	///\brief Fetch a lua object as typed input filter from the lua stack of the executed lua thread context
	///\param[in] idx index of the object on the lua stack
	///\return the lua object
	TypedInputFilterR getObject( int idx);

	///\brief Push a lua object as typed input filter on the lua stack of the executed lua thread context
	///\param[in] obj the object to push
	void pushObject( const TypedInputFilterR& obj);

private:
	///\brief Create the context for executing a Lua script without input/output but all other objects initialized
	///\param[in] provider_ processor provider for allocation of objects accessed
	///\param[in] callMain wheter to call the script for initialization of its objects or not
	void initbase( const proc::ProcessorProvider* provider_, bool callMain);

private:
	friend class LuaScript;

	lua_State* m_ls;
	lua_State* m_thread;
	int m_threadref;
	const LuaScript* m_script;
	const LuaModuleMap* m_modulemap;

private:
	LuaScriptInstance( const LuaScriptInstance&){} //non copyable
};

typedef boost::shared_ptr<LuaScriptInstance> LuaScriptInstanceR;


///\class LuaFunctionMap
///\brief Map of available Lua functions
class LuaFunctionMap
{
public:
	///\brief Constructor
	LuaFunctionMap( const LuaModuleMap* modulemap_)
		:m_modulemap(modulemap_){}

	///\brief Destructor
	virtual ~LuaFunctionMap();

	///\brief Define a lua function
	///\param[in] procname name of the function
	///\param[in] script with the function
	void defineLuaFunction( const std::string& procname, const LuaScript& script);
	///\brief Get an empty the context for a Lua script
	bool getLuaScriptInstance( const std::string& procname, LuaScriptInstanceR& rt) const;

	///\brief Get the list of commands
	std::list<std::string> commands() const;

private:
	LuaFunctionMap( const LuaFunctionMap&){}	//non copyable

private:
	std::vector<LuaScript*> m_ar;
	std::map<std::string,std::size_t> m_pathmap;
	std::map<std::string,std::size_t> m_procmap;
	const LuaModuleMap* m_modulemap;
};

}}//namespace
#endif

