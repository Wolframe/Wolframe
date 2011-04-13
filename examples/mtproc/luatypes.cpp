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
	static const char* Generator = "wolframe.InputGenerator";
}

struct GeneratorClosure
{
	boost::shared_ptr<protocol::Generator> generator;
	protocol::Generator::ElementType type;
	char* value;
	char* buf;
	std::size_t bufsize;
	std::size_t bufpos;

	void init()
	{
		bufpos = 0;
		value = 0;
	};
};

struct LuaInput
{
	boost::shared_ptr<protocol::Generator> generator;
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

static int generatorNextStringTuple( lua_State* ls)
{
	GeneratorClosure* closure = (GeneratorClosure*)luaL_checkudata( ls, lua_upvalueindex( 1), luaname::GeneratorClosure);
	if (closure == 0 || closure->bufsize == 0)
	{
		luaL_error( ls, "calling undefined iterator");
		return 0;
	}
	if (closure->generator->getNext( &closure->type, closure->buf, closure->bufsize-1, &closure->bufpos))
	{
		switch (closure->generator->state())
		{
			case protocol::Generator::EndOfMessage:
				return lua_yield( ls, 0);

			case protocol::Generator::Error:
				luaL_error( ls, "error in iterator (%d)", closure->generator->getError());
				return 0;

			case protocol::Generator::Open:
				lua_pushnil( ls);
				lua_pushnil( ls);
				return 2;
		}
	}
	else
	{
		switch (closure->type)
		{
			case protocol::Generator::OpenTag:
				closure->buf[ closure->bufpos] = 0;
				lua_pushstring( ls, closure->buf);
				lua_pushnil( ls);
				closure->init();
				return 2;

			case protocol::Generator::Value:
				closure->buf[ closure->bufpos] = 0;
				if (closure->value)
				{
					lua_pushstring( ls, closure->buf);
					lua_pushstring( ls, closure->value);
				}
				else
								closure->init();
				return 2;

			 case protocol::Generator::Attribute:
				closure->buf[ closure->bufpos++] = 0;
				if (closure->value)
				{
					luaL_error( ls, "illegal state produced by generator");
					closure->init();
					return 0;
				}
				else
				{
					closure->value = closure->buf+closure->bufpos;
					return generatorNextStringTuple( ls);
				}
			 case protocol::Generator::CloseTag:
				lua_pushnil( ls);
				lua_pushnil( ls);
				closure->init();
				return 2;
		}
	}
	luaL_error( ls, "illegal state produced by generator");
	return 0;
}

struct Interpreter::State
{
	lua_State* lua;

	State()
	{
		lua = luaL_newstate();
		if (!lua) throw std::bad_alloc();
	}
	~State()
	{
		lua_close( lua);
	}
};

Interpreter::Interpreter()
{
	state = new State();
}

Interpreter::~Interpreter()
{
	delete state;
}

int Interpreter::call( unsigned int , const char**)
{
	return 0;
}

