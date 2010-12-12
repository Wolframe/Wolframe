#ifndef LUA_HELPERS_HEADER
#define LUA_HELPERS_HEADER

void lua_helper_dump_stack( lua_State *l, const char *comment );

void lua_helper_dump_table( lua_State *l, const int idx, const int level );

void lua_helper_dump_globals( lua_State *l );

void lua_helper_dump_registry( lua_State *l );

#endif /* LUA_HELPERS_HEADER */
