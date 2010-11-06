#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <stdio.h>
#include <assert.h>
#ifndef _WIN32
#include <inttypes.h>
#endif

#include "luahelpers.h"

void lua_helper_dump_stack( lua_State *l, const char *comment )
{
	int i;

	int top = lua_gettop( l );
	if( top == 0 ) {
		printf( "STACK %s -- <empty stack>\n", comment );
		return;
	}

	printf( "STACK %s -- BEGIN\n", comment );
	for( i = 1; i <= top; i++ ) {
		int t = lua_type( l, i );

		switch( t ) {
			case LUA_TNIL:
				printf( "STACK %s -- %d (-%d) : nil\n", comment, i, top - i + 1 );
				break;

			case LUA_TSTRING:
				printf( "STACK %s -- %d (-%d): \"%s\"\n", comment, i, top - i + 1, lua_tostring( l, i ) );
				break;

			case LUA_TNUMBER:
				printf( "STACK %s -- %d (-%d): %g\n", comment, i, top - i + 1, lua_tonumber( l, i ) );
				break;

			case LUA_TFUNCTION: {
				lua_CFunction f = lua_tocfunction( l, i );
#ifndef _WIN32
				printf( "STACK %s -- %d (-%d): function[%016"PRIxPTR"]\n", comment, i, top - i + 1, (uintptr_t)f );
#else
				printf( "STACK %s -- %d (-%d): function[%016]\n", comment, i, top - i + 1, f );
#endif
				break;
				}

			case LUA_TTABLE:
				printf( "STACK %s -- %d (-%d): table\n", comment, i, top - i + 1 );
				break;				

			default:
				printf( "STACK %s -- %d (-%d): [unknown(%s)]\n", comment, i, top - i + 1, lua_typename( l, t ) );
		}
	}
	printf( "STACK %s -- END\n", comment );
}

void lua_helper_dump_table( lua_State *l, const int idx, const int level )
{
	/* push first key 'nil' to start the iteration */
	lua_pushnil( l );
	//dump_stack( l, "beforefirstnext" );

	while( lua_next( l, idx ) != 0 ) {
		/* uses 'key' (at index -2) and 'value' (at index -1) */
		//dump_stack( l, "afternext" );

		printf( "(%d) %*s", level, level * 3, " " );
		switch( lua_type( l, -2 ) ) {
			case LUA_TSTRING:
				printf( "%s = ", lua_tostring( l, -2 ) );
				break;

			case LUA_TNUMBER:
				printf( "%g = ", lua_tonumber( l, -2 ) );
				break;

			default:
				printf( "[unknown(%s)] = ", lua_typename( l, lua_type( l, -2 ) ) );
		}

		switch( lua_type( l, -1 ) ) {
			case LUA_TSTRING:
				printf( "\"%s\"\n", lua_tostring( l, -1 ) );
				break;

			case LUA_TNUMBER:
				printf( "%g\n", lua_tonumber( l, -1 ) );
				break;

			case LUA_TFUNCTION: {
				lua_CFunction f = lua_tocfunction( l, -1 );
#ifdef _WIN32
				printf( "function[%p]\n", f );
#else
				printf( "function[%016"PRIxPTR"]\n", (uintptr_t)f );
#endif
				break;
				}

			case LUA_TTABLE:
				puts( "{" );
				/* keep stack intact, -1 is the table itself,
				   we will push another nil to start the iteration
				   so the index to the table is -3
				 */
				//dump_stack( l, "starttable" );
				lua_helper_dump_table( l, -2, level + 1 );
				printf( "(%d) %*s}\n", level, level * 3, " " );
				//dump_stack( l, "endtable" );
				break;

			default:
				printf( "[unknown(%s)]\n", lua_typename( l, lua_type( l, -1 ) ) );
		}
		/* pop away value of iteration, keep the key */
		lua_pop( l, 1 );

		//dump_stack( l, "beforenext" );
	}
}

void lua_helper_dump_globals( lua_State *l )
{
	puts( "-- GLOBALS:" );
	lua_helper_dump_table( l, LUA_GLOBALSINDEX, 0 );
}

/* segfaults?
void dump_environment( lua_State *l )
{
	puts( "-- ENVORINMENT:" );
	dump_table( l, LUA_ENVIRONINDEX );
}
*/

void lua_helper_dump_registry( lua_State *l )
{
	puts( "-- REGISTRY:" );
	lua_helper_dump_table( l, LUA_REGISTRYINDEX, 0 );
}
