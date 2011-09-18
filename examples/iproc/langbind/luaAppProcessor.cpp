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
#include "luaDebug.hpp"
#include "appObjects.hpp"
#include "logger.hpp"
#include "protocol/formatoutput.hpp"
#include "protocol/inputfilter.hpp"
#include <stdexcept>
#include <cstddef>
#include <boost/lexical_cast.hpp>

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "lcoco.h"
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
	static const char* InputFilterClosure = "wolframe.InputFilterClosure";
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

namespace
{
template <class ObjectType>
const char* metaTableName()					{return 0;}
template <> const char* metaTableName<Input>()			{return luaname::Input;}
template <> const char* metaTableName<Output>()			{return luaname::Output;}
template <> const char* metaTableName<Filter>()			{return luaname::Filter;}
template <> const char* metaTableName<InputFilterClosure>()	{return luaname::InputFilterClosure;}
}//anonymous namespace

static const luaL_Reg empty_methodtable[ 1] =
{
	{0,0}
};

template <class ObjectType>
struct LuaObject :public ObjectType
{
	const char* m_name;
	LuaObject( const ObjectType& o)
		:ObjectType(o),m_name(metaTableName<ObjectType>()) {}
	LuaObject()
		:m_name(metaTableName<ObjectType>()) {}

	static int destroy( lua_State* ls)
	{
		LuaObject *THIS = (LuaObject*)lua_touserdata( ls, 1);
		if (THIS) THIS->~LuaObject();
		return 0;
	}

	static void create( lua_State* ls, const luaL_Reg* mt=0)
	{
		luaL_openlib( ls, metaTableName<ObjectType>(), mt?mt:empty_methodtable, 0);
		luaL_newmetatable( ls, metaTableName<ObjectType>());
		luaL_openlib( ls, 0, getMetamethods(), 0);

		lua_pushliteral( ls, "__index");
		lua_pushvalue( ls, -3);		// dup methods table
		lua_rawset( ls, -3);		// metatable.__index = methods
		lua_pushliteral( ls, "__metatable");
		lua_pushvalue( ls, -3);		// dup methods table
		lua_rawset( ls, -3);		//hide metatable: metatable.__metatable = methods
		lua_pop( ls, 1);
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

	void* operator new (std::size_t num_bytes, lua_State* ls, const char* mt) throw (std::bad_alloc)
	{
		void* rt = lua_newuserdata( ls, num_bytes);
		if (rt == 0) throw std::bad_alloc();
		luaL_getmetatable( ls, mt);
		lua_setmetatable( ls, -2);
		return rt;
	}

	/// \brief does nothing because the LUA garbage collector does the job.
	/// \warning CAUTION: DO NOT CALL THIS FUNCTION ! DOES NOT WORK ON MSVC 9.0. (The compiler links with the std delete)
	/// (just avoids C4291 warning)
	void operator delete (void *, lua_State*) {}
	/// ABa: got a C4291 warning here again, not sure about the whole construct here..
	/// an indicator could be that started the 'iprocd.exe' results in an error when
	/// opening the processor!
	void operator delete (void *, lua_State*, const char*) {}

	template <class Orig>
	static void push_luastack( lua_State* ls, const Orig& o)
	{
		try
		{
			const char* mt = metaTableName<ObjectType>();
			(void*)new (ls,mt) LuaObject( o);
		}
		catch (std::bad_alloc)
		{
			luaL_error( ls, "memory allocation error in lua context");
		}
	}

	static void createGlobal( lua_State* ls, const char* name, const ObjectType& instance, const luaL_Reg* mt=0)
	{
		create( ls, mt);
		new (ls) LuaObject( instance);
		luaL_getmetatable( ls, metaTableName<ObjectType>());
		lua_setmetatable( ls, -2);
		lua_setglobal( ls, name);
	}

	static bool setGlobal( lua_State* ls, const char* name, const ObjectType& instance)
	{
		lua_getglobal( ls, name);
		LuaObject* obj = (LuaObject*) toudata_udkey( ls, -1, metaTableName<ObjectType>());
		if (!obj) return false;
		*obj = instance;
		return true;
	}

	static ObjectType* getGlobal( lua_State* ls, const char* name)
	{
		lua_getglobal( ls, name);
		LuaObject* obj = (LuaObject*) toudata_udkey( ls, -1, metaTableName<ObjectType>());
		if (!obj)
		{
			luaL_error( ls, "reserved global variable '%s' has been changed", name);
		}
		return obj;
	}

	static LuaObject* getSelf( lua_State* ls, const char* name, const char* method)
	{
		LuaObject* self;
		if (lua_gettop( ls) == 0 || (self=(LuaObject*) toudata_udkey( ls, 1, metaTableName<ObjectType>())) == 0)
		{
			luaL_error( ls, "'%s' needs self parameter (%s:%s() instead of %s.%s())", name, name, method, name, method);
		}
		return self;
	}

	static LuaObject* get( lua_State* ls, int index)
	{
		LuaObject* rt = (LuaObject*) toudata_udkey( ls, index, metaTableName<ObjectType>());
		return rt;
	}
};

static void pushItem( lua_State* ls, const char* item, unsigned int itemsize)
{
	if (item)
	{
		lua_pushlstring( ls, item, (std::size_t)itemsize);
		lua_tostring( ls, -1); //PF:BUGFIX lua 5.1.4 needs this one
	}
	else
	{
		lua_pushboolean( ls, 0);
	}
}

static int function_inputFilter( lua_State* ls)
{
	for (;;)
	{
		const char* item[2];
		unsigned int itemsize[2];

		InputFilterClosure* closure = (InputFilterClosure*)lua_touserdata( ls, lua_upvalueindex( 1));

		switch (closure->fetch( item[1]/*tag*/, itemsize[1], item[0]/*val*/, itemsize[0]))
		{
			case InputFilterClosure::DoYield:
				lua_yield( ls, 0);
				continue;

			case InputFilterClosure::EndOfData:
				return 0;

			case InputFilterClosure::Error:
				luaL_error( ls, "error in iterator");
				return 0;

			case InputFilterClosure::Data:
				pushItem( ls, item[0], itemsize[0]);
				pushItem( ls, item[1], itemsize[1]);
				return 2;
		}
		luaL_error( ls, "illegal state produced by input filter");
		return 0;
	}
}

static const char* get_printop( lua_State* ls, int index, std::size_t& size)
{
	const char* rt = 0;
	if (lua_isnil( ls, index) || (lua_isboolean( ls, index) && !lua_toboolean( ls, index))) {}
	else if ((rt=lua_tolstring( ls, index, &size)) == 0)
	{
		const char* tn = lua_typename( ls, lua_type( ls, index));
		luaL_error( ls, "invalid type (%s) of argument %d (convertable to string or nil or false expected)", tn?tn:"unknown", index-1);
	}
	return rt;
}

static int function_output_print( lua_State* ls)
{
	for (;;)
	{
		const char* item[2] = {0,0};
		std::size_t itemsize[2] = {0,0};

		Output* output = LuaObject<Output>::getSelf( ls, "output", "print");
		if (lua_gettop( ls) == 1)
		{
			return 0;
		}
		if (lua_gettop( ls) == 2)
		{
			item[0] = get_printop( ls, 2, itemsize[0]);
		}
		else if (lua_gettop( ls) == 3)
		{
			item[0] = get_printop( ls, 2, itemsize[0]);
			item[1] = get_printop( ls, 3, itemsize[1]);
		}
		else
		{
			return luaL_error( ls, "too many arguments in call of format output print");
		}
		switch (output->print( item[1]/*tag*/, itemsize[1], item[0]/*val*/, itemsize[0]))
		{
			case Output::DoYield:
				lua_yield( ls, 0);
				continue;

			case Output::Error:
				luaL_error( ls, "error in format output print (%d)", output->m_formatoutput->getError());
				return 0;

			case Output::Data:
				return 0;
		}
		luaL_error( ls, "illegal state produced by format output print");
		return 0;
	}
}

static int function_filter( lua_State* ls)
{
	unsigned int nn = lua_gettop( ls);
	unsigned int buffersize = 0;
	if (nn == 0) return luaL_error( ls, "too few arguments for filter");
	if (nn > 2) return luaL_error( ls, "too many arguments for filter");
	if (nn == 2)
	{
		if (lua_type( ls, 2) != LUA_TNUMBER)
		{
			return luaL_error( ls, "invalid 2nd argument for filter (number expected)");
		}
		try
		{
			buffersize = boost::lexical_cast<unsigned int>( lua_tonumber( ls, 2));
		}
		catch (...)
		{
			return luaL_error( ls, "invalid 2nd argument for filter (positive non fraction number expected)");
		}
	}
	if (!lua_isstring( ls, 1))
	{
		return luaL_error( ls, "invalid type of argument (string expected)");
	}
	const char* name = lua_tostring( ls, 1);

	Filter ft( name, buffersize, buffersize);
	LuaObject<Filter>::push_luastack( ls, ft);
	return 1;
}

static int function_input_as( lua_State* ls)
{
	Input* input = LuaObject<Input>::getSelf( ls, "input", "as");
	if (lua_gettop( ls) != 2)
	{
		luaL_error( ls, "filter type value as parameter of method 'input:as' expected");
	}
	LuaObject<Filter>* filter = LuaObject<Filter>::get( ls, 2);

	if (!filter)
	{
		const char* tn = lua_typename( ls, lua_type( ls, 2));
		luaL_error( ls, "filter type value expected as first argument of input:as instead of %s", tn?tn:"UNKNOWN");
	}
	protocol::InputFilter* ff = 0;
	if (filter->m_inputfilter.get())
	{
		ff = filter->m_inputfilter->copy();
		if (input->m_inputfilter.get())
		{
			*ff = *input->m_inputfilter;
		}
	}
	else
	{
		luaL_error( ls, "input:as called with a filter with undefined input");
	}
	input->m_inputfilter.reset( ff);

	return 0;
}

static int function_output_as( lua_State* ls)
{
	Output* output = LuaObject<Output>::getSelf( ls, "output", "as");
	if (lua_gettop( ls) != 2)
	{
		luaL_error( ls, "filter type value as parameter of method 'output:as' expected");
	}
	Filter* filter = LuaObject<Filter>::get( ls, 2);
	if (!filter)
	{
		const char* tn = lua_typename( ls, lua_type( ls, 2));
		luaL_error( ls, "filter type value expected as first argument of output:as instead of %s", tn?tn:"UNKNOWN");
	}
	protocol::FormatOutput* ff = 0;
	if (filter->m_formatoutput.get())
	{
		ff = filter->m_formatoutput->copy();
		if (output->m_formatoutput.get())
		{
			*ff = *output->m_formatoutput;
		}
	}
	else
	{
		luaL_error( ls, "output:as called with a filter with undefined output");
	}
	output->m_formatoutput.reset( ff);
	return 0;
}

static int function_input_get( lua_State* ls)
{
	Input* input = LuaObject<Input>::getSelf( ls, "input", "get");
	if (lua_gettop( ls) != 1)
	{
		luaL_error( ls, "no arguments expected for input:get");
	}
	if (!input->m_inputfilter.get())
	{
		return luaL_error( ls, "no filter defined for input with input:as");
	}
	LuaObject<InputFilterClosure>::push_luastack( ls, input->m_inputfilter);
	lua_pushcclosure( ls, function_inputFilter, 1);
	return 1;
}

static int function_yield( lua_State* ls)
{
	int ii,nn=lua_gettop( ls);
	for (ii=0; ii<nn; ii++) lua_pushvalue( ls, ii);
	return lua_yield( ls, ii);
}

static const luaL_Reg input_methodtable[ 3] =
{
	{"as",&function_input_as},
	{"get",&function_input_get},
	{0,0}
};

static const luaL_Reg output_methodtable[ 4] =
{
	{"as",&function_output_as},
	{"print",&function_output_print},
	{0,0}
};

static void create_global_functions( lua_State* ls)
{
	//yield( )
	lua_pushliteral( ls, "yield");
	lua_pushcfunction( ls, &function_yield);
	lua_settable( ls, LUA_GLOBALSINDEX);

	//filter( )
	lua_pushliteral( ls, "filter");
	lua_pushcfunction( ls, &function_filter);
	lua_settable( ls, LUA_GLOBALSINDEX);
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

AppProcessor::AppProcessor( const lua::Configuration* config)
		:m_config(config)
{
	m_state = new State( *config);
	LuaObject<Input>::createGlobal( m_state->ls, "input", m_input, input_methodtable);
	LuaObject<Output>::createGlobal( m_state->ls, "output", m_output, output_methodtable);
	LuaObject<Filter>::create( m_state->ls);
	LuaObject<InputFilterClosure>::create( m_state->ls);
	create_global_functions( m_state->ls);
}

AppProcessor::~AppProcessor()
{
	delete m_state;
}

static AppProcessor::CallResult getYieldState( protocol::InputFilter* in, protocol::FormatOutput* fo, const char* methodName)
{
	if (!fo)
	{
		LOG_ERROR << "no output specified";
		return AppProcessor::Error;
	}
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
		m_state->thread = lua_newcthread( m_state->ls, m_config->cthread_stacksize());

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
			if (!LuaObject<Input>::setGlobal( m_state->ls, "input", m_input))
			{
				LOG_ERROR << "Failed to initialize input. It possibly has been redefined as a value of different type";
			}
		}
		if (m_output.m_formatoutput.get())
		{
			if (!LuaObject<Output>::setGlobal( m_state->ls, "output", m_output))
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

