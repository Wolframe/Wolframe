/* C part of testmodule */

#ifndef TESTMOD_API
#ifdef _WIN32
#define TESTMOD_API __declspec( dllexport )
#else
#define TESTMOD_API extern
#endif
#endif

#include "lua.h"
#include "lauxlib.h"

static int testmod_func( lua_State *l );
TESTMOD_API int luaopen_testmod( lua_State *l );

static int testmod_func( lua_State *l )
{
	lua_pushstring( l, "Hello" );

	return 1;
}

static const luaL_Reg testmod_prv[] = {
	{ "func", testmod_func },
	{ NULL, NULL }
};

TESTMOD_API int luaopen_testmod( lua_State *l )
{
	luaL_newlib( l, testmod_prv );
	
	return 1;
}
