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
using namespace iproc;
using namespace lua;
using namespace app;

namespace luaname
{
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

	template <class Orig>
	static void push_luastack( lua_State* ls, const Orig& o)
	{
		try
		{
			(void*)new (ls) LuaObject( o);
		}
		catch (std::bad_alloc)
		{
			luaL_error( ls, "memory allocation error in lua context");
		}
	}

	static void createGlobal( lua_State* ls, const char* metatableName, const char* name, const ObjectType& instance)
	{
		create( ls, metatableName);
		(void)new (ls) LuaObject( instance);
		luaL_getmetatable( ls, metatableName);
		lua_setmetatable( ls, -2);
		lua_setglobal( ls, name);
	}
};

static int function_inputGenerator( lua_State* ls)
{
	const char* item[2];
	unsigned int itemsize[2];

	InputGeneratorClosure* closure = (InputGeneratorClosure*)lua_touserdata( ls, lua_upvalueindex( 1));

	switch (closure->fetch( item[0], itemsize[0], item[1], itemsize[1]))
	{
		case InputGeneratorClosure::DoYield:
			return lua_yield( ls, 0);

		case InputGeneratorClosure::EndOfData:
			return 0;

		case InputGeneratorClosure::Error:
			luaL_error( ls, "error in iterator");
			return 0;

		case InputGeneratorClosure::Data:
			if (item[0]) lua_pushstring( ls, item[0]); else lua_pushnil( ls);
			if (item[1]) lua_pushstring( ls, item[1]); else lua_pushnil( ls);
			return 2;
	}
	luaL_error( ls, "illegal state produced by generator");
	return 0;
}

static int function_output_print( lua_State* ls)
{
	const char* item[2] = {0,0};
	std::size_t itemsize[2] = {0,0};

	Output* output = (Output*) lua_touserdata( ls, 1);

	if (lua_gettop( ls) == 0)
	{
	}
	else if (lua_gettop( ls) == 1)
	{
		if (lua_isnil( ls, 1)) {}
		else if ((item[1]=lua_tolstring( ls, 1, &itemsize[1])) == 0)
		{
			luaL_error( ls, "invalid type of argument (convertable to string or nil expected)");
		}
	}
	else if (lua_gettop( ls) == 2)
	{
		if (lua_isnil( ls, 1)) {}
		else if ((item[0]=lua_tolstring( ls, 1, &itemsize[0])) == 0)
		{
			luaL_error( ls, "invalid type of first argument (convertable to string or nil expected)");
		}
		if (lua_isnil( ls, 2)) {}
		else if ((item[1]=lua_tolstring( ls, 2, &itemsize[1])) == 0)
		{
			luaL_error( ls, "invalid type of second argument (convertable to string or nil expected)");
		}
	}
	else
	{
		return luaL_error( ls, "too many arguments in call of format output print");
	}
	switch (output->print( item[0], itemsize[0], item[1], itemsize[1]))
	{
		case Output::DoYield:
			return lua_yield( ls, 0);

		case Output::Error:
			luaL_error( ls, "error in format output print");
			return 0;

		case Output::Data:
			return 0;
	}
	luaL_error( ls, "illegal state produced by format output print");
	return 0;
}

static int function_filter( lua_State* ls)
{
	void* ud = lua_touserdata( ls, lua_upvalueindex(1));
	System* system = (System*) ud;
	if (lua_gettop( ls) != 1) return luaL_error( ls, "invalid number of arguments (1 string as parameter expected)");
	if (!lua_isstring( ls, 1)) return luaL_error( ls, "invalid type of argument (string expected)");
	const char* name = lua_tostring( ls, 1);
	LuaObject<Filter>::push_luastack( ls, Filter( system, name));
	return 1;
}

void* toudata_udkey( lua_State* ls, int index, const char* id)
{
	lua_getmetatable( ls,index);
	const void* p1 = lua_topointer( ls,-1);
	luaL_getmetatable( ls,id);
	const void* p2 = lua_topointer( ls,-1);

	lua_pop( ls,2);
	return p1 == p2 ? lua_touserdata( ls, index) : NULL;
}

static int function_input_as( lua_State* ls)
{
	if (lua_gettop( ls) != 2)
	{
		return luaL_error( ls, "invalid number of arguments (1 filter type value as parameter of method 'as' expected)");
	}
	LuaObject<Input>* input = (LuaObject<Input>*) lua_touserdata( ls, 1);
	LuaObject<Filter>* filter = (LuaObject<Filter>*) toudata_udkey( ls, 2, luaname::Filter);
	if (!filter)
	{
		luaL_error( ls, "filter type value expected as first argument");
	}
	else if (input)
	{
		boost::shared_ptr<protocol::Generator> filtergenerator( filter->m_generator);
		*filtergenerator = *input->m_generator;
		input->m_generator = filtergenerator;
	}
	else
	{
		luaL_error( ls, "unexpected error: object of mehod 'as' is not input or corrupt");
	}
	return 0;
}

static int function_output_as( lua_State* ls)
{
	if (lua_gettop( ls) != 2)
	{
		return luaL_error( ls, "invalid number of arguments (1 filter type value as parameter of method 'as' expected)");
	}
	LuaObject<Output>* output = (LuaObject<Output>*) lua_touserdata( ls, 1);
	LuaObject<Filter>* filter = (LuaObject<Filter>*) toudata_udkey( ls, 2, luaname::Filter);
	if (!filter)
	{
		luaL_error( ls, "filter type value expected as first argument");
	}
	else if (output)
	{
		boost::shared_ptr<protocol::FormatOutput> filteroutput( filter->m_formatoutput);
		*filteroutput = *output->m_formatoutput;
		output->m_formatoutput = filteroutput;
	}
	else
	{
		luaL_error( ls, "unexpected error: object of mehod 'as' is not output or corrupt");
	}
	return 0;
}

static int function_input_get( lua_State* ls)
{
	if (lua_gettop( ls) != 1)
	{
		return luaL_error( ls, "invalid number of arguments (no arguments for method 'get' expected)");
	}
	LuaObject<Input>* input = (LuaObject<Input>*) lua_touserdata( ls, 1);
	if (!input->m_generator.get())
	{
		return luaL_error( ls, "no filter defined for input with input.as(...)");
	}
	LuaObject<InputGeneratorClosure>::push_luastack( ls, input->m_generator);
	lua_pushcclosure( ls, &function_inputGenerator, 1);
	return 1;
}

static void create_function_filter( lua_State* ls, System* system)
{
	lua_pushlightuserdata( ls, system);
	lua_pushcclosure( ls, &function_filter, 1);

	lua_pushstring( ls, "filter");
	lua_pushcfunction( ls, &function_filter);
	lua_settable( ls, LUA_REGISTRYINDEX);

	LuaObject<Filter>::create( ls, luaname::Filter);
}

struct AppProcessor::State
{
	lua_State* ls;
	lua_State* thread;
	int threadref;

	State( const lua::Configuration& config) :ls(0),thread(0),threadref(0)
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

AppProcessor::AppProcessor( System* system, const lua::Configuration* config, Input& input, Output& output)
		:m_config(config),m_input(input),m_output(output),m_system(system)
{
	m_state = new State( *config);

	LuaObject<Input>::createGlobal( m_state->ls, luaname::Input, "input", m_input);
	LuaObject<Input>::defineMethod( m_state->ls, luaname::Input, "as", &function_input_as);
	LuaObject<Input>::defineMethod( m_state->ls, luaname::Input, "get", &function_input_get);

	LuaObject<Output>::createGlobal( m_state->ls, luaname::Output, "output", m_output);
	LuaObject<Output>::defineMethod( m_state->ls, luaname::Output, "as", &function_output_as);
	LuaObject<Output>::defineMethod( m_state->ls, luaname::Output, "print", &function_output_print);

	create_function_filter( m_state->ls, m_system);
}

AppProcessor::~AppProcessor()
{
	delete m_state;
}

static AppProcessor::CallResult getYieldState( protocol::Generator* in, protocol::FormatOutput* fo, const char* methodName, bool commandHasIO)
{
	if (fo->getError())
	{
		LOG_ERROR << "error " << fo->getError() << ") in format output when calling '" << methodName << "'";
		return AppProcessor::Error;
	}
	protocol::Generator::State istate = in->state();

	switch (istate)
	{
		case protocol::Generator::Open:
			if (fo->size() == 0)
			{
				LOG_ERROR << "error printed in method '" << methodName << "' declared to have no output";
				return AppProcessor::Error;
			}
			return AppProcessor::YieldWrite;

		case protocol::Generator::EndOfMessage:
			if (commandHasIO)
			{
				return AppProcessor::YieldRead;
			}
			else
			{
				in->protocolInput( 0, 0, true);
				return AppProcessor::Ok;
			}

		case protocol::Generator::Error:
		{
			int returnCode = in->getError();
			LOG_ERROR << "error " << returnCode << ") in input generator when calling '" << methodName << "'";
			return AppProcessor::Error;
		}
	}
	LOG_ERROR << "illegal state of input generator when calling '" << methodName << "'";
	return AppProcessor::Error;
}

AppProcessor::CallResult AppProcessor::call( unsigned int argc, const char** argv, bool commandHasIO)
{
	if (argc == 0)
	{
		LOG_ERROR << "lua interpreter called with no arguments (first argument funtion name missing)";
		return Error;
	}
	int rt = 0;

	if (!m_state->thread)
	{
		// create thread for the call execution
		m_state->thread = lua_newthread( m_state->ls);
		m_state->threadref = luaL_ref( m_state->ls, LUA_REGISTRYINDEX);

		// initialize input:
		boost::shared_ptr<protocol::Generator> filtergenerator( m_system->createGenerator());
		*filtergenerator = *m_input.m_generator;
		m_input.m_generator = filtergenerator;

		// initialize output:
		boost::shared_ptr<protocol::FormatOutput> filteroutput( m_system->createFormatOutput());
		*filteroutput = *m_output.m_formatoutput;
		m_output.m_formatoutput = filteroutput;

		for (unsigned int ii=0; ii<argc; ii++)
		{
			if (argv[ii]) lua_pushstring( m_state->ls, argv[ii]); else lua_pushnil( m_state->ls);
		}
		rt = lua_resume( m_state->thread, argc-1);
	}
	else
	{
		rt = lua_resume( m_state->thread, 0);
	}
	if (rt == LUA_YIELD)
	{
		return getYieldState( m_input.m_generator.get(), m_output.m_formatoutput.get(), argv[0], commandHasIO);
	}
	else if (rt != 0)
	{
		luaL_unref( m_state->ls, LUA_REGISTRYINDEX, m_state->threadref);
		m_state->threadref = 0;
		m_state->thread = 0;
		LOG_ERROR << "error " << rt << ")  calling '" << argv[0] << "'";
		return Error;
	}
	else
	{
		luaL_unref( m_state->ls, LUA_REGISTRYINDEX, m_state->threadref);
		m_state->threadref = 0;
		m_state->thread = 0;
	}
	return Ok;
}

