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

/* Is there because MSVC croaks:
   luatypes.cpp(130) : warning C4291: 'void *LuaObject<ObjectType>::operator new(size_t,lua_State *)' :
   no matching operator delete found; memory will not be freed if initialization throws an exception
*/
#pragma warning(disable : 4291)

namespace luaname
{
	static const char* GeneratorClosure = "wolframe.InputGeneratorClosure";
	static const char* Input = "wolframe.Input";
	static const char* Output = "wolframe.Output";
	static const char* System = "wolframe.System";
	static const char* Generator = "wolframe.InputGenerator";
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
	
	void* operator new (std::size_t num_bytes, lua_State* ls) throw( )
	{
		return lua_newuserdata( ls, num_bytes);
	}
};

// Design
/// input is an object LuaInput with a shared_ptr reference to the application processor contexts input generator reference it is predefined at
/// application processor startup. It has a function at(f) defining the filter and a function get() returning the input iterator function with closure.
///
/// A lua filter is a lua function. input.as(f) expects a function f that returns a boost::shared_ptr<protocol::Generator*>.
/// This is then assigned to the input generator after getting its buffer. Why shared_ptr<>, because we want to be able to free it, in any context
/// also when it is created in a dll.
///
///
// A module luafilters exists that defines a global table "filter" with all create filter functions defined as member variables. referencing such an element
// does calling the function returning a new filter.
// A module luasystem exists that defines a global table "system" with reference to all system interfaces from the processing context as objects
// defined. (for example "input" and "output")  input has a function input.as(f) with a filter f as argument and a function get.

static int generatorGetNext( lua_State* ls)
{
	const char* item[2];
	GeneratorClosure* closure = (GeneratorClosure*)luaL_checkudata( ls, lua_upvalueindex( 1), luaname::GeneratorClosure);
	if (closure == 0)
	{
		luaL_error( ls, "calling undefined iterator");
		return 0;
	}
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

static void createInput( lua_State* ls, Input& input)
{
	LuaObject<Input>::create( ls, luaname::Input);
	(void)new (ls) LuaObject<Input>( input);
	luaL_getmetatable( ls, luaname::Input);
	lua_setmetatable( ls, -2);
	lua_setglobal( ls, "input");
}

static void createOutput( lua_State* ls, Output& output)
{
	LuaObject<Output>::create( ls, luaname::Output);
	(void)new (ls) LuaObject<Output>( output);
	luaL_getmetatable( ls, luaname::Output);
	lua_setmetatable( ls, -2);
	lua_setglobal( ls, "output");
}

static void createSystem( lua_State* ls, System* system)
{
	luaL_newmetatable( ls, luaname::System);
	lua_settable( ls, -1);
	System** ref = (System**)lua_newuserdata( ls, sizeof(System*));
	luaL_getmetatable( ls, luaname::System);
	lua_setmetatable( ls, -2);
	*ref = system;
	lua_setglobal( ls, "system");
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
	createSystem( m_state->ls, m_system);
	createInput( m_state->ls, m_input);
	createOutput( m_state->ls, m_output);
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

