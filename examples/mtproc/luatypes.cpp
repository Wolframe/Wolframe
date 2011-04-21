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
#include "luatypes.hpp"
#include "logger.hpp"
#include "langbind.hpp"
#include "protocol/formatoutput.hpp"
#include "protocol/generator.hpp"
#include <stdexcept>
#include <cstddef>
extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

using namespace _Wolframe;
using namespace mtproc;

namespace luaname
{
	static const char* GeneratorClosure = "wolframe.InputGeneratorClosure";
	static const char* Input = "wolframe.Input";
	static const char* Output = "wolframe.Output";
	static const char* Filter = "wolframe.Filter";
}

template <class ObjectType>
struct LuaObject :public ObjectType
{
	LuaObject( const ObjectType& o)   :ObjectType(o) {}

	static int destroy( lua_State* ls)
	{
		LuaObject *THIS = *(LuaObject**)lua_touserdata( ls, 1);
		if (THIS) THIS->~LuaObject();
		return 0;
	}

	static void create( lua_State* ls, const char* metatableName)
	{
		luaL_newmetatable( ls, metatableName);
		/* set its __gc field */
		lua_pushstring( ls, "__gc");
		lua_pushcfunction( ls, destroy);
		lua_settable( ls, -3);
	}

	typedef int (*Method)( lua_State *ls);
	static void defineMethod( lua_State* ls, const char* metatableName, const char* name, Method method)
	{
		luaL_getmetatable( ls, metatableName);
		lua_pushstring( ls, name);
		lua_pushcfunction( ls, method);
		lua_settable( ls, -3);
	}

	void* operator new (std::size_t num_bytes, lua_State* ls) throw (std::bad_alloc)
	{
		void* rt = lua_newuserdata( ls, num_bytes);
		if (rt == 0) throw std::bad_alloc();
		return rt;
	}

	/// \brief does nothing because the LUA garbage collector does the job.
	/// \warning CAUTION: DO NOT CALL THIS FUNCTION ! DOES NOT WORK ON MSVC 9.0. (The compiler links with the std delete)
	///          (just avoids C4291 warning)
	void operator delete (void *, lua_State* ) {}

	static void createGlobal( lua_State* ls, const char* metatableName, const char* name, const ObjectType& instance)
	{
		create( ls, metatableName);
		(void)new (ls) LuaObject( instance);
		luaL_getmetatable( ls, metatableName);
		lua_setmetatable( ls, -2);
		lua_setglobal( ls, name);
	}
};

static int function_generator( lua_State* ls)
{
	const char* item[2];
	GeneratorClosure* closure = (GeneratorClosure*)lua_touserdata( ls, lua_upvalueindex( 1));

	switch (closure->fetch( item[0], item[1]))
	{
		case GeneratorClosure::DoYield:
			return lua_yield( ls, 0);

		case GeneratorClosure::EndOfData:
			return 0;

		case GeneratorClosure::Error:
			luaL_error( ls, "error in iterator");
			return 0;

		case GeneratorClosure::Data:
			if (item[0]) lua_pushstring( ls, item[0]); else lua_pushnil( ls);
			if (item[1]) lua_pushstring( ls, item[1]); else lua_pushnil( ls);
			return 2;
	}
	luaL_error( ls, "illegal state produced by generator");
	return 0;
}

static int function_filter( lua_State* ls)
{
	void* ud = lua_touserdata( ls, lua_upvalueindex(1));
	System* system = (System*) ud;
	if (lua_gettop( ls) != 1) return luaL_error( ls, "invalid number of arguments (1 string as parameter expected)");
	if (!lua_isstring( ls, 1)) return luaL_error( ls, "invalid type of argument (string expected)");
	const char* name = lua_tostring( ls, 1);
	(void)new (ls) LuaObject<Filter>( Filter( system, name));
	return 1;
}

static int function_as( lua_State* ){/*Aba??*/return 0;}

#if 0 /// commented out
void* toudata_udkey( lua_State* L, int index, const char* id)
{
	lua_getmetatable(L,index);
	const void* p1 = lua_topointer(L,-1);
	luaL_getmetatable(L,id);
	const void* p2 = lua_topointer(L,-1);

	lua_pop(L,2);
	return p1 == p2 ? lua_touserdata(L, index) : NULL;
}

static int function_as( lua_State* ls)
{
	if (lua_gettop( ls) != 2) return luaL_error( ls, "invalid number of arguments (1 filter type value as parameter of method 'as' expected)");
	LuaObject<Input>* input = toudata_udkey ( ls, 1, luaname::Input);
	LuaObject<Output>* output = toudata_udkey( ls, 1, luaname::Output);
	LuaObject<Filter>* filter = (LuaObject<Filter>*) toudata_udkey( ls, 2);
	if (!filter)
	{
		luaL_error( ls, "filter type value expected as first argument");
	}
	else if (input)
	{
		boost::shared_ptr<protocol::Generator> filtergenerator( filter);
		*filtergenerator = *input->m_generator;
		input->m_generator = filtergenerator;
	}
	else if (output)
	{
		boost::shared_ptr<protocol::FormatOutput> filteroutput( filter->m_formatoutput);
		*filteroutput = *output->m_formatoutput;
		output->m_formatoutput = filteroutput;
	}
	else
	{
		luaL_error( ls, "unexpected error: object of mehod unknown or corrupt");
	}
	return 0;
}
#endif

static void create_function_filter( lua_State* ls, System* system)
{
	lua_pushlightuserdata( ls, system);
	lua_pushcclosure( ls, &function_filter, 1);

	lua_pushstring( ls, "filter");
	lua_pushcfunction( ls, &function_filter);
	lua_settable( ls, LUA_REGISTRYINDEX);

	LuaObject<Filter>::create( ls, luaname::Filter);
}

struct Interpreter::State
{
	lua_State* ls;
	lua_State* thread;

	State( const lua::Configuration& config) :ls(0),thread(0)
	{
		ls = luaL_newstate();
		if (!ls) throw std::bad_alloc();
		if (!config.load( ls)) throw std::bad_alloc();
	}
	~State()
	{
		if (ls) lua_close( ls);
	}
};

Interpreter::Interpreter( System* system, const lua::Configuration& config, Input& input, Output& output)
		:m_input(input),m_output(output),m_system(system)
{
	m_state = new State( config);

	LuaObject<Input>::createGlobal( m_state->ls, luaname::Input, "input", m_input);
	LuaObject<Input>::defineMethod( m_state->ls, luaname::Input, "as", &function_as);

	LuaObject<Output>::createGlobal( m_state->ls, luaname::Output, "output", m_output);
	LuaObject<Output>::defineMethod( m_state->ls, luaname::Output, "as", &function_as);

	create_function_filter( m_state->ls, m_system);
}

Interpreter::~Interpreter()
{
	delete m_state;
}

int Interpreter::call( unsigned int argc, const char** argv)
{
	int rt;
	if (argc == 0)
	{
		LOG_ERROR << "interpreter called with no arguments (first argument funtion name missing)";
		return -1;
	}

	if (!m_state->thread)
	{
		m_state->thread = lua_newthread( m_state->ls);
		for (unsigned int ii=0; ii<argc; ii++)
		{
			if (argv[ii]) lua_pushstring( m_state->ls, argv[ii]); else lua_pushnil( m_state->ls);
		}
		rt = lua_resume( m_state->thread, argc-1);
		if (rt==LUA_YIELD) rt = 0;
	}
	else
	{
		rt = lua_resume( m_state->thread, 0);
		if (rt==LUA_YIELD) rt = 0;
	}
	return rt;
}

