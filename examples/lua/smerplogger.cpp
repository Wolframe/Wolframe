//
// smerplogger.cpp
//

extern "C" {
        #include "lauxlib.h"
}

#include "logger.hpp"

/* prototypes */
static int l_write( lua_State *l );

extern "C" {
	LUALIB_API int luaopen_smerplogger( lua_State *l );
}

static int l_write( lua_State *l )
{
	/* first parameter maps to a log level, rest gets printed depending on
	 * whether it's a string or a number
	 */
	const char *logLevel = luaL_checkstring( l, 1 );
	const char *msg = luaL_checkstring( l, 2 );
	// TODO: this is NOT the singleton instance of the daemon!?
	// the first level logged is thus ERROR
	_SMERP::Logger( _SMERP::LogBackend::instance() ).Get(
		_SMERP::LogLevel::str2LogLevel( logLevel ) ) << msg;

	return 0;
}

/* registry */
static const luaL_Reg smerplogger[] = {
	{ "write", l_write },
	{ NULL, NULL }
};

extern "C" {

LUALIB_API int luaopen_smerplogger( lua_State *l )
{
	luaL_register( l, "smerplogger", smerplogger );

	return 0;
}

}
