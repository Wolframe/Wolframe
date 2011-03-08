//
// luaHandler.cpp
//

#include "luaHandler.hpp"
#include "logger.hpp"
#include "SSLcertificateInfo.hpp"

#include <string>
#include <cstring>
#include <stdexcept>
#include <sstream>

extern "C" {
	#include "lua.h"
	#include "lualib.h"
	#include "lauxlib.h"
}

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

		_Wolframe::Logger( _Wolframe::LogBackend::instance() ).Get(
			_Wolframe::LogComponent::LOGCOMPONENT_LUA,
			_Wolframe::LogLevel::str2LogLevel( logLevel ) ) << os.str( );

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

		// Open lua libraries based on configuration, see
		// http://stackoverflow.com/questions/966162/best-way-to-omit-lua-standard-libraries
		for( std::list<std::string>::const_iterator it = config.preload_libs.begin( ); it != config.preload_libs.end( ); it++ ) {
			std::map<std::string, LuaModuleDefinition>::const_iterator it2 = config.knownLuaModules.find( *it );
			if( it2 != config.knownLuaModules.end( ) ) {
				LOG_TRACE << "LUA initializing library '" << *it << "'";
				lua_pushcfunction( l, it2->second.moduleInit );
				lua_pushstring( l, it2->second.moduleName.c_str( ) );
				lua_call( l, 1, 0 );
			} else {
				LOG_ERROR << "Can't load unknown LUA library '" << *it << "'!";
			}
		}

		// TODO: script location, also configurable
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
		lua_pushstring( l, "init" );
		lua_gettable( l, LUA_GLOBALSINDEX );
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
		lua_pushstring( l, "destroy" );
		lua_gettable( l, LUA_GLOBALSINDEX );
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

	luaConnection::luaConnection( const Network::LocalTCPendpoint& local, const luaConfig config_ )
		: config( config_ )
	{
		LOG_TRACE << "Created connection handler for " << local.toString();
		createVM( );
	}

#ifdef WITH_SSL
	luaConnection::luaConnection( const Network::LocalSSLendpoint& local, const luaConfig config_ )
		: config( config_ )
	{
		LOG_TRACE << "Created connection handler (SSL) for " << local.toString();
		createVM( );
	}
#endif // WITH_SSL

	luaConnection::~luaConnection()
	{
		LOG_TRACE << "Connection handler destroyed";
		destroyVM( );
	}

	void luaConnection::setPeer( const Network::RemoteTCPendpoint& remote )
	{
		LOG_TRACE << "Peer set to " << remote.toString();

		lua_pushstring( l, "new_connection" );
		lua_gettable( l, LUA_GLOBALSINDEX );
		lua_pushstring( l, remote.host( ).c_str( ) );
		unsigned short port = remote.port( );
		std::stringstream ss;
		ss << port;
		lua_pushstring( l, ss.str( ).c_str( ) );
		int res = lua_pcall( l, 2, 0, 0 );
		if( res != 0 ) {
			LOG_FATAL << "Unable to call 'new_connection' function: " << lua_tostring( l, -1 );
			lua_pop( l, 1 );
			throw new std::runtime_error( "Error in LUA processor" );
		}
	}

#ifdef WITH_SSL
	void luaConnection::setPeer( const Network::RemoteSSLendpoint& remote )
	{
		LOG_TRACE << "Peer set to " << remote.toString();
		if ( remote.SSLcertInfo() )	{
			LOG_TRACE << "Peer SSL certificate Common Name: " << remote.SSLcertInfo()->commonName();
		}

		lua_pushstring( l, "new_connection" );
		lua_gettable( l, LUA_GLOBALSINDEX );
		lua_pushstring( l, remote.host( ).c_str( ) );
		unsigned short port = remote.port( );
		std::stringstream ss;
		ss << port;
		lua_pushstring( l, ss.str( ).c_str( ) );
		if ( remote.SSLcertInfo() )	{
			lua_pushstring( l, remote.SSLcertInfo()->commonName().c_str());
		} else {
			lua_pushnil( l );
		}
		int res = lua_pcall( l, 3, 0, 0 );
		if( res != 0 ) {
			LOG_FATAL << "Unable to call 'new_connection' function: " << lua_tostring( l, -1 );
			lua_pop( l, 1 );
			throw new std::runtime_error( "Error in LUA processor" );
		}
	}
#endif // WITH_SSL

	/// Handle a request and produce a reply.
	const Network::NetworkOperation luaConnection::nextOperation()
	{
		lua_pushstring( l, "next_operation" );
		lua_gettable( l, LUA_GLOBALSINDEX );
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
			return Network::NetworkOperation( Network::SendString( msg ) );
		} else if( !strcmp( op, "READ" ) ) {
			unsigned int idle_timeout = lua_tointeger( l, -1 );
			lua_pop( l, 2 );
			return Network::NetworkOperation( Network::ReadData( buf, buf_size, idle_timeout ));
		} else if( !strcmp( op, "CLOSE" ) ) {
			lua_pop( l, 2 );
			return Network::NetworkOperation( Network::CloseConnection( ) );
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

		lua_pushstring( l, "network_input" );
		lua_gettable( l, LUA_GLOBALSINDEX );
		lua_pushlstring( l, (const char *)begin, bytesTransferred );
		int res = lua_pcall( l, 1, 0, 0 );
		if( res != 0 ) {
			LOG_FATAL << "Unable to call 'network_input' function: " << lua_tostring( l, -1 );
			throw new std::runtime_error( "Error in LUA processor" );
		}
	}

	void luaConnection::timeoutOccured()
	{
		LOG_TRACE << "Processor received timeout";

		lua_pushstring( l, "timeout_occured" );
		lua_gettable( l, LUA_GLOBALSINDEX );
		int res = lua_pcall( l, 0, 0, 0 );
		if( res != 0 ) {
			LOG_FATAL << "Unable to call 'timeout_occured' function: " << lua_tostring( l, -1 );
			lua_pop( l, 1 );
			throw new std::runtime_error( "Error in LUA processor" );
		}
	}

	void luaConnection::signalOccured()
	{
		LOG_TRACE << "Processor received signal";

		lua_pushstring( l, "signal_occured" );
		lua_gettable( l, LUA_GLOBALSINDEX );
		int res = lua_pcall( l, 0, 0, 0 );
		if( res != 0 ) {
			LOG_FATAL << "Unable to call 'signal_occured' function: " << lua_tostring( l, -1 );
			lua_pop( l, 1 );
			throw new std::runtime_error( "Error in LUA processor" );
		}
	}

	void luaConnection::errorOccured( NetworkSignal signal )
	{
		const char *signal_s;

		switch( signal ) {
			case END_OF_FILE:
				signal_s = "END_OF_FILE";
				break;

			case BROKEN_PIPE:
				signal_s = "BROKEN_PIPE";
				break;

			case OPERATION_CANCELLED:
				signal_s = "OPERATION_CANCELLED";
				break;

			case UNKNOWN_ERROR:
				signal_s = "UNKNOWN_ERROR";
				break;
		}

		LOG_TRACE << "Got error '" << signal_s << "'";

		lua_pushstring( l, "error_occured" );
		lua_gettable( l, LUA_GLOBALSINDEX );
		lua_pushstring( l, signal_s );
		int res = lua_pcall( l, 1, 0, 0 );
		if( res != 0 ) {
			LOG_FATAL << "Unable to call 'error_occured' function: " << lua_tostring( l, -1 );
			lua_pop( l, 1 );
			throw new std::runtime_error( "Error in LUA processor" );
		}
	}


	/// ServerHandler PIMPL

	ServerHandler::ServerHandlerImpl::ServerHandlerImpl( const HandlerConfiguration *config ) {
		config_.script = config->luaConfig->script;
		config_.preload_libs = config->luaConfig->preload_libs;
		
		LuaModuleDefinition x;
		x.moduleName = "";
		x.moduleInit = luaopen_base;
		config_.knownLuaModules["base"] = x;
		x.moduleName = LUA_TABLIBNAME;
		x.moduleInit = luaopen_table;
		config_.knownLuaModules[LUA_TABLIBNAME] = x;
		x.moduleName = LUA_IOLIBNAME;
		x.moduleInit = luaopen_io;
		config_.knownLuaModules[LUA_IOLIBNAME] = x;
		x.moduleName = LUA_OSLIBNAME;
		x.moduleInit = luaopen_os;
		config_.knownLuaModules[LUA_OSLIBNAME] = x;
		x.moduleName = LUA_STRLIBNAME;
		x.moduleInit = luaopen_string;
		config_.knownLuaModules[LUA_STRLIBNAME] = x;
		x.moduleName = LUA_MATHLIBNAME;
		x.moduleInit = luaopen_math;
		config_.knownLuaModules[LUA_MATHLIBNAME] = x;
		x.moduleName = LUA_DBLIBNAME;
		x.moduleInit = luaopen_debug;
		config_.knownLuaModules[LUA_DBLIBNAME] = x;
		x.moduleName = LUA_LOADLIBNAME;
		x.moduleInit = luaopen_package;
		config_.knownLuaModules[LUA_LOADLIBNAME] = x;
	}

	Network::connectionHandler* ServerHandler::ServerHandlerImpl::newConnection( const Network::LocalTCPendpoint& local )
	{
		return new luaConnection( local, config_ );
	}
#ifdef WITH_SSL
	Network::connectionHandler* ServerHandler::ServerHandlerImpl::newSSLconnection( const Network::LocalSSLendpoint& local )
	{
		return new luaConnection( local, config_ );
	}
#endif // WITH_SSL
	ServerHandler::ServerHandler( const HandlerConfiguration *config ) : impl_( new ServerHandlerImpl( config ) )	{}

	ServerHandler::~ServerHandler()	{ delete impl_; }

	Network::connectionHandler* ServerHandler::newConnection( const Network::LocalTCPendpoint& local )
	{
		return impl_->newConnection( local );
	}
#ifdef WITH_SSL
	Network::connectionHandler* ServerHandler::newSSLconnection( const Network::LocalSSLendpoint& local )
	{
		return impl_->newSSLconnection( local );
	}
#endif // WITH_SSL
} // namespace _Wolframe
