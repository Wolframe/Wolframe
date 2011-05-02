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
#include "luaAppProcessor.hpp"
#include "logger.hpp"
#include "luaLog.hpp"
#include "appObjects.hpp"
#include "protocol/formatoutput.hpp"
#include "protocol/inputfilter.hpp"
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

static void* toudata_udkey( lua_State* ls, int index, const char* id)
{
	lua_getmetatable( ls,index);
	const void* p1 = lua_topointer( ls,-1);
	luaL_getmetatable( ls,id);
	const void* p2 = lua_topointer( ls,-1);

	lua_pop( ls,2);
	return p1 == p2 ? lua_touserdata( ls, index) : NULL;
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

	static void create( lua_State* ls, const char* metatableName, const luaL_Reg* mt=0)
	{
		// see (http://lua-users.org/wiki/BindingWithMembersAndMethods)
		// and (http://lua-users.org/wiki/BindingWithMetatableAndClosures)
		lua_pushstring( ls, metatableName);
		lua_newtable( ls);
		int methods = lua_gettop( ls);
		lua_newtable( ls);
		int metatable = lua_gettop( ls);

		lua_pushliteral( ls, "__index");	// add index event to metatable
		lua_pushvalue( ls, methods);
		lua_settable( ls, metatable);

		lua_pushliteral( ls, "__metatable");
		lua_pushvalue( ls, methods);
		lua_settable( ls, metatable);

		luaL_openlib( ls, 0, getMetamethods(), 0);
		if (mt) luaL_openlib( ls, 0, mt, 0);
		lua_settable( ls, LUA_GLOBALSINDEX);
	}

	static const luaL_Reg* getMetamethods()
	{
		static luaL_Reg ar[2] = {{"__gc", destroy},{0,0}};
		return ar;
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
	void operator delete (void *, lua_State*) {}

	template <class Orig>
	static void push_luastack( lua_State* ls, const Orig& o, const char* metatableName)
	{
		try
		{
			(void*)new (ls) LuaObject( o);
			luaL_getmetatable( ls, metatableName);
			lua_setmetatable( ls, -2);
		}
		catch (std::bad_alloc)
		{
			luaL_error( ls, "memory allocation error in lua context");
		}
	}

	static void createGlobal( lua_State* ls, const char* metatableName, const char* name, const ObjectType& instance, const luaL_Reg* mt=0)
	{
		create( ls, metatableName, mt);
		(void)new (ls) LuaObject( instance);
		luaL_getmetatable( ls, metatableName);
		lua_setmetatable( ls, -2);
		lua_setglobal( ls, name);
	}

	static bool setGlobal( lua_State* ls, const char* metatableName, const char* name, const ObjectType& instance)
	{
		lua_getglobal( ls, name);
		LuaObject* obj = (LuaObject*) toudata_udkey( ls, -1, metatableName);
		if (!obj) return false;
		*obj = instance;
		return true;
	}
};

static int function_inputFilter( lua_State* ls)
{
	const char* item[2];
	unsigned int itemsize[2];

	InputFilterClosure* closure = (InputFilterClosure*)lua_touserdata( ls, lua_upvalueindex( 1));

	switch (closure->fetch( item[0], itemsize[0], item[1], itemsize[1]))
	{
		case InputFilterClosure::DoYield:
			return lua_yield( ls, 0);

		case InputFilterClosure::EndOfData:
			return 0;

		case InputFilterClosure::Error:
			luaL_error( ls, "error in iterator");
			return 0;

		case InputFilterClosure::Data:
			if (item[0]) lua_pushstring( ls, item[0]); else lua_pushnil( ls);
			if (item[1]) lua_pushstring( ls, item[1]); else lua_pushnil( ls);
			return 2;
	}
	luaL_error( ls, "illegal state produced by input filter");
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
	LuaObject<Filter>::push_luastack( ls, Filter( system, name), luaname::Filter);
	return 1;
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
		boost::shared_ptr<protocol::InputFilter> inputfilter( filter->m_inputfilter);
		if (input->m_inputfilter.get())
		{
			*inputfilter = *input->m_inputfilter;
		}
		input->m_inputfilter = inputfilter;
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
		if (output->m_formatoutput.get())
		{
			*filteroutput = *output->m_formatoutput;
		}
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
/*[-]*/LOG_ERROR << "DEBUG: '" << lua_typename( ls, lua_type( ls, 0)) << "'";
	LuaObject<Input>* input = (LuaObject<Input>*) lua_touserdata( ls, 1);
	if (!input->m_inputfilter.get())
	{
		return luaL_error( ls, "no filter defined for input with input.as(...)");
	}
	LuaObject<InputFilterClosure>::push_luastack( ls, input->m_inputfilter, luaname::Filter);
	lua_pushcclosure( ls, &function_inputFilter, 1);
	return 1;
}

static int function_yield( lua_State* ls)
{
	int ii,nn=lua_gettop( ls);
	for (ii=0; ii<nn; ii++) lua_pushvalue( ls, ii);
	return lua_yield( ls, ii);
}

static void create_global_functions( lua_State* ls, System* system)
{
	//yield( )
	lua_pushliteral( ls, "yield");
	lua_pushcfunction( ls, &function_yield);
	lua_settable( ls, LUA_GLOBALSINDEX);

	//filter( )
	lua_pushlightuserdata( ls, system);
	lua_pushcclosure( ls, &function_filter, 1);
	lua_pushliteral( ls, "filter");
	lua_pushcfunction( ls, &function_filter);
	lua_settable( ls, LUA_GLOBALSINDEX);
	LuaObject<Filter>::create( ls, luaname::Filter);
}

struct AppProcessor::State
{
	lua_State* ls;
	lua_State* thread;
	int threadref;
	lua_State* env;

	State( const lua::Configuration& config) :ls(0),thread(0),threadref(0),env(0)
	{
		env = ls = luaL_newstate();
		if (!ls) throw std::bad_alloc();
		if (!config.load( ls)) throw std::runtime_error( "cannot load application processor from configuration");
	}
	~State()
	{
		if (ls) lua_close( ls);
	}
};

static const luaL_Reg input_methodtable[ 3] =
{
	{"as",&function_input_as},
	{"get",&function_input_get},
	{0,0}
};

static const luaL_Reg output_methodtable[ 3] =
{
	{"as",&function_output_as},
	{"get",&function_output_print},
	{0,0}
};

AppProcessor::AppProcessor( System* system, const lua::Configuration* config)
		:m_config(config),m_system(system)
{
	m_state = new State( *config);
	LuaObject<Input>::createGlobal( m_state->ls, luaname::Input, "input", m_input, input_methodtable);
	LuaObject<Output>::createGlobal( m_state->ls, luaname::Output, "output", m_output, output_methodtable);
	create_global_functions( m_state->ls, m_system);
}

AppProcessor::~AppProcessor()
{
	delete m_state;
}

static AppProcessor::CallResult getYieldState( protocol::InputFilter* in, protocol::FormatOutput* fo, const char* methodName)
{
	if (fo->getError())
	{
		LOG_ERROR << "error " << fo->getError() << ") in format output when calling '" << methodName << "'";
		return AppProcessor::Error;
	}
	protocol::InputFilter::State istate = in->state();

	switch (istate)
	{
		case protocol::InputFilter::Open:
			return AppProcessor::YieldWrite;

		case protocol::InputFilter::EndOfMessage:
			return AppProcessor::YieldRead;

		case protocol::InputFilter::Error:
		{
			int returnCode = in->getError();
			LOG_ERROR << "error " << returnCode << ") in input filter when calling '" << methodName << "'";
			return AppProcessor::Error;
		}
	}
	LOG_ERROR << "illegal state of input filter when calling '" << methodName << "'";
	return AppProcessor::Error;
}

AppProcessor::CallResult AppProcessor::call( unsigned int argc, const char** argv)
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

		// prevent garbage collecting of thread (http://permalink.gmane.org/gmane.comp.lang.lua.general/22680)
		lua_pushvalue( m_state->ls, -1);
		m_state->threadref = luaL_ref( m_state->ls, LUA_REGISTRYINDEX);

		// (http://medek.wordpress.com/page/2/)
		// create a local environment with a link to global environment via the '__index' metamethod
		lua_newtable( m_state->ls);
		lua_pushvalue( m_state->ls, -1);
		lua_setmetatable( m_state->ls, -2); //Set itself as metatable
		lua_pushvalue( m_state->ls, LUA_GLOBALSINDEX);
		lua_setfield( m_state->ls, -2, "__index");
		lua_setfenv( m_state->ls, -2);
		lua_pop( m_state->ls, 1);

		if (m_input.m_inputfilter.get())
		{
			if (!LuaObject<Input>::setGlobal( m_state->ls, luaname::Input, "input", m_input))
			{
				LOG_ERROR << "Failed to initialize input. It possibly has been redefined as a value of different type";
			}
		}
		if (m_output.m_formatoutput.get())
		{
			if (!LuaObject<Output>::setGlobal( m_state->ls, luaname::Output, "output", m_output))
			{
				LOG_ERROR << "Failed to initialize output. It possibly has been redefined as a value of different type";
			}
		}
		// call the function (for the first time)
		lua_getglobal( m_state->thread, argv[0]);
		for (unsigned int ii=1; ii<argc; ii++)
		{
			if (argv[ii]) lua_pushstring( m_state->thread, argv[ii]); else lua_pushnil( m_state->thread);
		}
		rt = lua_resume( m_state->thread, argc-1);
	}
	else
	{
		// call the function (subsequently until termination)
		rt = lua_resume( m_state->thread, 0);
	}
	if (rt == LUA_YIELD)
	{
		if (m_input.m_inputfilter.get())
		{
			return getYieldState( m_input.m_inputfilter.get(), m_output.m_formatoutput.get(), argv[0]);
		}
		else
		{
			return YieldWrite;
		}
	}
	else if (rt != 0)
	{
		const char* msg = lua_tostring( m_state->thread, -1);
		LOG_ERROR << "error calling '" << argv[0] << "':" << msg;
		luaL_unref( m_state->ls, LUA_REGISTRYINDEX, m_state->threadref);
		m_state->threadref = 0;
		m_state->thread = 0;
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

