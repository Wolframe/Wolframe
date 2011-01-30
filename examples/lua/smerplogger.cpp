//
// smerplogger.cpp
//

extern "C" {
        #include "lauxlib.h"
}

#ifdef _WIN32
#define MYLUA_API __declspec(dllexport)
#else
#define MYLUA_API
#endif

#ifndef _WIN32
#define __STDC_FORMAT_MACROS
#include <cstdio>
#include <inttypes.h>
#else
#define PRIxPTR "%p"
#endif

#include "logger.hpp"

#include <sstream>

/* prototypes */
static int l_write( lua_State *l );

extern "C" {
	LUALIB_API int MYLUA_API luaopen_smerplogger( lua_State *l );
}

static int l_write( lua_State *l )
{
	/* first parameter maps to a log level, rest gets printed depending on
	 * whether it's a string or a number
	 */
	const char *logLevel = luaL_checkstring( l, 1 );

	std::ostringstream os;
	int n = lua_gettop( l ) - 1;
	int i = 2;
	for( ; n--; i++ ) {
		int type = lua_type( l, i );

		switch( type ) {
			case LUA_TNIL:
				os << "nil";
				break;

			case LUA_TSTRING: {
				const char *v = lua_tostring( l, i );
				os << v;
				}
				break;
				
			case LUA_TNUMBER: {
				lua_Number v = lua_tonumber( l, i );
				os << v;
				}
				break;

			case LUA_TFUNCTION: {
				lua_CFunction f = lua_tocfunction( l, i );
				char buf[33];
				snprintf( buf, 32, "function[%016" PRIxPTR "]", (uintptr_t)f );
				os << buf;
				}
				break;

			case LUA_TTABLE:
				// TODO

			default:
				os << "<unknown>";
		}
	}

	// TODO: this is NOT the singleton instance of the daemon!?
	// the first level logged is thus ERROR
	_SMERP::Logger( _SMERP::LogBackend::instance() ).Get(
		_SMERP::LogLevel::str2LogLevel( logLevel ) ) << os.str( );

	lua_pop( l, n );

	return 0;
}

/* registry */
static const luaL_Reg smerplogger_registry[] = {
	{ "write", l_write },
	{ NULL, NULL }
};

extern "C" {

LUALIB_API int MYLUA_API luaopen_smerplogger( lua_State *l )
{
	luaL_register( l, "smerplogger", smerplogger_registry );

	return 0;
}

} // extern "C"
