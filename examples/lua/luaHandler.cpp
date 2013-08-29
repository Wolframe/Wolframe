//
// luaHandler.cpp
//

#include "luaHandler.hpp"
#include "logger-v1.hpp"
#include "system/SSLcertificateInfo.hpp"

#include <string>
#include <cstring>
#include <stdexcept>
#include <sstream>

#include "boost/date_time/posix_time/posix_time.hpp"		// to print time_t structures

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

// Aba: for snprintf, maybe better to use ostringstream?
#ifndef _WIN32
#define __STDC_FORMAT_MACROS
#include <cstdio>
#include <inttypes.h>
#else
#define snprintf _snprintf
#define PRIxPTR "%p"
#endif

namespace _Wolframe {

static int lua_log( lua_State *_l )
{
	/* first parameter maps to a log level, rest gets printed depending on
	 * whether it's a string or a number
	 */
	const char *logLevel = luaL_checkstring( _l, 1 );

	std::ostringstream os;
	int n = lua_gettop( _l ) - 1;
	int i = 2;
	for( ; n--; i++ ) {
		int type = lua_type( _l, i );

		switch( type ) {
		case LUA_TNIL:
			os << "nil";
			break;

		case LUA_TSTRING: {
			const char *v = lua_tostring( _l, i );
			os << v;
		}
		break;

		case LUA_TNUMBER: {
			lua_Number v = lua_tonumber( _l, i );
			os << v;
		}
		break;

		case LUA_TFUNCTION: {
			lua_CFunction f = lua_tocfunction( _l, i );
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

	_Wolframe::log::Logger( _Wolframe::log::LogBackend::instance() ).Get(
				_Wolframe::log::LogLevel::strToLogLevel( logLevel ) )
		<< os.str( );

	lua_pop( _l, n );

	return 0;
}

void luaConnection::createVM( )
{
	LOG_TRACE << "Creating new Lua virtual machine";

	// instanitate a new VM
	l = luaL_newstate( );
	if( !l ) {
		LOG_FATAL << "Unable to create new LUA engine!";
		throw new std::runtime_error( "Can't initialize LUA processor" );
	}

	// Open lua libraries based on configuration (from linit.c)

	// call open functions from 'loadedlibs' and set results to global table
	for( std::list<std::string>::const_iterator it = config.preload_libs.begin( ); it != config.preload_libs.end( ); it++ ) {
		std::map<std::string, LuaModuleDefinition>::const_iterator it2 = config.knownLuaModules.find( *it );
		if( it2 != config.knownLuaModules.end( ) ) {
			LOG_TRACE << "LUA initializing library '" << *it << "'";
			luaL_requiref( l, it2->second.moduleName.c_str( ), it2->second.moduleInit, 1 );
			lua_pop( l, 1 ); // remove lib
		} else {
			// we should not get here! the configuration checked should croak before
			LOG_FATAL << "Can't load unknown LUA library '" << *it << "'!";
			throw new std::runtime_error( "Can't initialize LUA processor" );
		}
	}
	// add open functions from 'preloadedlibs' into 'package.preload' table
	luaL_getsubtable( l, LUA_REGISTRYINDEX, "_PRELOAD" );
	for( std::list<std::string>::const_iterator it = config.preload_libs.begin( ); it != config.preload_libs.end( ); it++ ) {
		std::map<std::string, LuaModuleDefinition>::const_iterator it2 = config.knownLuaModules.find( *it );
		lua_pushcfunction( l, it2->second.moduleInit );
		lua_setfield( l, -2, it2->second.moduleName.c_str( ) );
	}
	lua_pop( l, 1 ); // remove _PRELOAD table

	// script location, also configurable
	int res = luaL_loadfile( l, config.script.c_str( ) );
	if( res != 0 ) {
		LOG_FATAL << "Unable to load LUA code '" << config.script << "': " << lua_tostring( l, -1 );
		lua_pop( l, 1 );
		throw new std::runtime_error( "Can't initialize LUA processor" );
	}

	// register logging function
	lua_pushcclosure( l, &lua_log, 0 );
	lua_setglobal( l, "log" );

	// call main, we may have to initialize LUA modules there
	res = lua_pcall( l, 0, LUA_MULTRET, 0 );
	if( res != 0 ) {
		LOG_FATAL << "Unable to call main entry of script: " << lua_tostring( l, -1 );
		lua_pop( l, 1 );
		throw new std::runtime_error( "Can't initialize LUA processor" );
	}

	// execute the main entry point of the script, we could initialize things there in LUA
// 5.1 -> 5.2
//	lua_pushstring( l, "init" );
//	lua_gettable( l, LUA_GLOBALSINDEX );
	lua_getglobal( l, "init" );
	res = lua_pcall( l, 0, 0, 0 );
	if( res != 0 ) {
		LOG_FATAL << "Unable to call 'init' function: " << lua_tostring( l, -1 );
		lua_pop( l, 1 );
		throw new std::runtime_error( "Can't initialize LUA processor" );
	}

	LOG_TRACE << "A new Lua virtual machine has been created";
}

void luaConnection::destroyVM( )
{
	LOG_TRACE << "Destroying the Lua virtual machine";

	// give LUA code a chance to clean up resources or something
	// usually hardly necessary, as the garbage collector should take care of it
// 5.1 -> 5.2
//	lua_pushstring( l, "destroy" );
//	lua_gettable( l, LUA_GLOBALSINDEX );
	lua_getglobal( l, "destroy" );
	int res = lua_pcall( l, 0, 0, 0 );
	if( res != 0 ) {
		LOG_FATAL << "Unable to call 'destroy' function: " << lua_tostring( l, -1 );
		lua_pop( l, 1 );
		throw new std::runtime_error( "Error in destruction of LUA processor" );
	}

	// close the VM, give away resources
	lua_close( l );

	LOG_TRACE << "Lua virtual machine has been destroyed";
}

#if 0
// maybe later with a SIGHUP?
void luaConnection::printMemStats( )
{
	int kbytes = lua_gc( l, LUA_GCCOUNT, 0 );
	if( kbytes > maxMemUsed ) {
		maxMemUsed = kbytes;
	}
	LOG_NOTICE << "LUA VM memory in use: " << kbytes << " kBytes (max. " << maxMemUsed << " kBytes)";
}
#endif

luaConnection::luaConnection( const net::LocalEndpoint& local, const luaConfig config_ )
	: config( config_ )
{
	LOG_TRACE	<< "Created connection handler "
			<< ( ( local.type( ) == _Wolframe::net::ConnectionEndpoint::SSL ) ? "(SSL) " : "" )
			<< "for " << local.toString( );

	createVM( );
}


luaConnection::~luaConnection()
{
	LOG_TRACE << "Connection handler destroyed";
	destroyVM( );
}

void luaConnection::setPeer( const net::RemoteEndpoint& remote )
{
	LOG_TRACE	<< "Peer set to " << remote.toString( )
			<< ", connected at " << boost::posix_time::from_time_t( remote.connectionTime( ) );


#ifdef WITH_SSL
	if( remote.type( ) == _Wolframe::net::ConnectionEndpoint::SSL ) {
		const _Wolframe::net::RemoteSSLendpoint& rmt = static_cast<const _Wolframe::net::RemoteSSLendpoint&>( remote );
		if ( rmt.SSLcertInfo( ) ) {
			LOG_TRACE << "Peer SSL certificate serial number " << rmt.SSLcertInfo()->serialNumber()
				  << ", issued by: " << rmt.SSLcertInfo()->issuer();
			LOG_TRACE << "Peer SSL certificate valid from " << boost::posix_time::from_time_t( rmt.SSLcertInfo()->notBefore())
				  << " to " <<  boost::posix_time::from_time_t( rmt.SSLcertInfo()->notAfter());
			LOG_TRACE << "Peer SSL certificate subject: " << rmt.SSLcertInfo()->subject();
			LOG_TRACE << "Peer SSL certificate Common Name: " << rmt.SSLcertInfo()->commonName();
		} else {
			LOG_TRACE << "No SSL information available, illegal client certificate or server is not requesting a client certificate";
		}
	}
#endif // WITH_SSL

// 5.1 -> 5.2
//	lua_pushstring( l, "new_connection" );
//	lua_gettable( l, LUA_GLOBALSINDEX );
	lua_getglobal( l, "new_connection" );
	lua_pushstring( l, remote.host( ).c_str( ) );
	lua_pushinteger( l, remote.port( ) );

#ifdef WITH_SSL
	_Wolframe::net::ConnectionEndpoint::ConnectionType type = remote.type( );
	if( type == _Wolframe::net::ConnectionEndpoint::SSL ) {
		const _Wolframe::net::RemoteSSLendpoint& rmt = static_cast<const _Wolframe::net::RemoteSSLendpoint&>( remote );
		if ( rmt.SSLcertInfo( ) ) {
			lua_newtable( l );
			int top = lua_gettop( l );

			lua_pushstring( l, "serialNumber" );
			lua_pushinteger( l, rmt.SSLcertInfo()->serialNumber( ) );
			lua_settable( l, top );

			lua_pushstring( l, "issuer" );
			lua_pushstring( l, rmt.SSLcertInfo()->issuer( ).c_str( ) );
			lua_settable( l, top );

			lua_pushstring( l, "validFrom" );
			lua_pushstring( l, boost::posix_time::to_simple_string( boost::posix_time::from_time_t( rmt.SSLcertInfo()->notBefore( ) ) ).c_str( ) );
			lua_settable( l, top );

			lua_pushstring( l, "validTo" );
			lua_pushstring( l, boost::posix_time::to_simple_string( boost::posix_time::from_time_t( rmt.SSLcertInfo()->notAfter( ) ) ).c_str( ) );
			lua_settable( l, top );

			lua_pushstring( l, "subject" );
			lua_pushstring( l, rmt.SSLcertInfo( )->subject( ).c_str( ) );
			lua_settable( l, top );

			lua_pushstring( l, "commonName" );
			lua_pushstring( l, rmt.SSLcertInfo( )->commonName( ).c_str( ) );
			lua_settable( l, top );
		} else {
			lua_pushnil( l );
		}
	} else {
		lua_pushnil( l );
	}
#else
	lua_pushnil( l );
#endif // WITH_SSL

	int res = lua_pcall( l, 3, 0, 0 );
	if( res != 0 ) {
		LOG_FATAL << "Unable to call 'new_connection' function: " << lua_tostring( l, -1 );
		lua_pop( l, 1 );
		throw new std::runtime_error( "Error in LUA processor" );
	}
}

/// Handle a request and produce a reply.
const net::NetworkOperation luaConnection::nextOperation()
{
// 5.1 -> 5.2
//	lua_pushstring( l, "next_operation" );
//	lua_gettable( l, LUA_GLOBALSINDEX );
	lua_getglobal( l, "next_operation" );
	int res = lua_pcall( l, 0, 2, 0 );
	if( res != 0 ) {
		LOG_FATAL << "Unable to call 'next_operation' function: " << lua_tostring( l, -1 );
		lua_pop( l, 1 );
		throw new std::runtime_error( "Error in LUA processor" );
	}
	const char *op = lua_tostring( l, -2 );
	if( !op ) {
		LOG_FATAL << "Lua function next_operation returned NULL as operation, illegal!!";
		throw new std::runtime_error( "Error in LUA processor" );
	}
	if( !strcmp( op, "WRITE" ) ) {
		const char *msg = lua_tostring( l, -1 );
		lua_pop( l, 2 );
		return net::NetworkOperation( net::SendString( msg ) );
	} else if( !strcmp( op, "READ" ) ) {
		unsigned int idle_timeout = lua_tointeger( l, -1 );
		lua_pop( l, 2 );
		return net::NetworkOperation( net::ReadData( buf, buf_size, idle_timeout ));
	} else if( !strcmp( op, "CLOSE" ) ) {
		lua_pop( l, 2 );
		return net::NetworkOperation( net::CloseConnection( ) );
	} else {
		lua_pop( l, 2 );
		LOG_FATAL << "Lua code returns '" << op << "', expecting one of 'READ', 'WRITE', 'CLOSE'!";
		throw new std::runtime_error( "Error in LUA processor" );
	}
}

// Parse incoming data. The data is copied from the temporary read buffer in
// the handler to the lua parameter passed by value (it's most likely not a good
// idea when using iterators later, we use too much copying around here!)
void luaConnection::networkInput( const void *begin, std::size_t bytesTransferred )
{
	LOG_DATA << "network Input: Read " << bytesTransferred << " bytes";

// 5.1 -> 5.2
//	lua_pushstring( l, "network_input" );
//	lua_gettable( l, LUA_GLOBALSINDEX );
	lua_getglobal( l, "network_input" );
	lua_pushlstring( l, (const char *)begin, bytesTransferred );
	int res = lua_pcall( l, 1, 0, 0 );
	if( res != 0 ) {
		LOG_FATAL << "Unable to call 'network_input' function: " << lua_tostring( l, -1 );
		throw new std::runtime_error( "Error in LUA processor" );
	}
}

void luaConnection::signalOccured( NetworkSignal signal )
{
	const char *signal_s = 0;

	if ( signal == TIMEOUT )	{
		LOG_TRACE << "Processor received timeout";

	// 5.1 -> 5.2
	//	lua_pushstring( l, "timeout_occured" );
	//	lua_gettable( l, LUA_GLOBALSINDEX );
		lua_getglobal( l, "timeout_occured" );
		int res = lua_pcall( l, 0, 0, 0 );
		if( res != 0 ) {
			LOG_FATAL << "Unable to call 'timeout_occured' function: " << lua_tostring( l, -1 );
			lua_pop( l, 1 );
			throw new std::runtime_error( "Error in LUA processor" );
		}
	}
	else if ( signal == TERMINATE )	{
		LOG_TRACE << "Processor received signal";

	// 5.1 -> 5.2
	//	lua_pushstring( l, "signal_occured" );
	//	lua_gettable( l, LUA_GLOBALSINDEX );
		lua_getglobal( l, "signal_occured" );
		int res = lua_pcall( l, 0, 0, 0 );
		if( res != 0 ) {
			LOG_FATAL << "Unable to call 'signal_occured' function: " << lua_tostring( l, -1 );
			lua_pop( l, 1 );
			throw new std::runtime_error( "Error in LUA processor" );
		}
	}
	else	{

		switch( signal ) {
			case END_OF_FILE:
				signal_s = "END_OF_FILE";
				break;

			case BROKEN_PIPE:
				signal_s = "BROKEN_PIPE";
				break;

			case CONNECTION_RESET:
				signal_s = "TERMINATING";
				break;

			case OPERATION_CANCELLED:
				signal_s = "OPERATION_CANCELLED";
				break;

			case TIMEOUT:
			case TERMINATE:
				// handled above, keeping compiler silent here
				break;

			case UNKNOWN_ERROR:
				signal_s = "UNKNOWN_ERROR";
				break;

			default:
				signal_s = "UNKNOWN_ERROR";
				break;
		}

		LOG_TRACE << "Got signal '" << signal_s << "'";

		// 5.1 -> 5.2
		//	lua_pushstring( l, "error_occured" );
		//	lua_gettable( l, LUA_GLOBALSINDEX );
		lua_getglobal( l, "error_occured" );
		lua_pushstring( l, signal_s );
		int res = lua_pcall( l, 1, 0, 0 );
		if( res != 0 ) {
			LOG_FATAL << "Unable to call 'error_occured' function: " << lua_tostring( l, -1 );
			lua_pop( l, 1 );
			throw new std::runtime_error( "Error in LUA processor" );
		}
	}
}


/// ServerHandler PIMPL

ServerHandler::ServerHandlerImpl::ServerHandlerImpl( const HandlerConfiguration *config ) {
	config_.script = config->luaConfig->script;
	config_.preload_libs = config->luaConfig->preload_libs;
	config_.knownLuaModules = config->luaConfig->knownLuaModules;
}

net::ConnectionHandler* ServerHandler::ServerHandlerImpl::newConnection( const net::LocalEndpoint& local )
{
	return new luaConnection( local, config_ );
}

ServerHandler::ServerHandler( const HandlerConfiguration *config,
			      const module::ModulesDirectory* /*modules*/ )
	: m_impl( new ServerHandlerImpl( config ) )	{}

ServerHandler::~ServerHandler()	{ delete m_impl; }

net::ConnectionHandler* ServerHandler::newConnection( const net::LocalEndpoint& local )
{
	return m_impl->newConnection( local );
}

} // namespace _Wolframe
