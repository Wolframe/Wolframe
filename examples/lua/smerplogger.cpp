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

#include "logger.hpp"

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
	const char *msg = luaL_checkstring( l, 2 );
	// TODO: this is NOT the singleton instance of the daemon!?
	// the first level logged is thus ERROR
	_SMERP::Logger( _SMERP::LogBackend::instance() ).Get(
		_SMERP::LogLevel::str2LogLevel( logLevel ) ) << msg;

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
