#include "luatypes.hpp"
#include <stdexcept>
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
	static const char* GeneratorClosure = "wolframe.GeneratorClosure";
#if 0 //not used yet
	static const char* GeneratorType = "wolframe.GeneratorType";
	static const char* FormatOutputClosure = "wolframe.FormatOutputClosure";
	static const char* FormatOutputType = "wolframe.FormatOutputType";
#endif
}

struct GeneratorClosure
{
	protocol::Generator* generator;
	protocol::Generator::ElementType type;
	char* value;
	char* buf;
	unsigned int bufsize;
	unsigned int bufpos;

	void init()
	{
		bufpos = 0;
		value = 0;
	};
};

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

bool Interpreter::passConfig( const char* , protocol::Generator* )
{
	return true;
}

int Interpreter::call( unsigned int , const char**)
{
	return 0;
}

