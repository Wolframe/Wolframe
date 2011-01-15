//
// connectionHandler.cpp
//

#include "luaHandler.hpp"
#include "logger.hpp"

#include <string>
#include <cstring>
#include <stdexcept>
#include <sstream>

extern "C" {
	#include <lualib.h>
	#include <lauxlib.h>
}

namespace _SMERP {

	void echoConnection::createVM( )
	{
		LOG_TRACE << "Creating new Lua virtual machine";
		
		// instanitate a new VM
		l = luaL_newstate( );
		if( !l ) {
			LOG_FATAL << "Unable to create new LUA engine!";
			throw new std::runtime_error( "Can't initialize LUA processor" );			
		}
		
		// TODO: open standard libraries, most likely something to configure later,
		// the plain echo processor should work without any lua libraries
		luaL_openlibs( l );
		// or open them individually, see:
		// http://stackoverflow.com/questions/966162/best-way-to-omit-lua-standard-libraries
		lua_pushcfunction( l, luaopen_base );
		lua_pushstring( l, "" );
		lua_call( l, 1, 0 );
		lua_pushcfunction( l, luaopen_io );
		lua_pushstring( l, LUA_LOADLIBNAME );
		lua_call( l, 1, 0 );
		lua_pushcfunction( l, luaopen_string );
		lua_pushstring( l, LUA_LOADLIBNAME );
		lua_call( l, 1, 0 );
		
		// TODO: script location, also configurable
		int res = luaL_loadfile( l, "echo.lua" );
		if( res != 0 ) {
			LOG_FATAL << "Unable to load LUA code 'echo.lua': " << lua_tostring( l, -1 );
			lua_pop( l, 1 );
			throw new std::runtime_error( "Can't initialize LUA processor" );
		}
				
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
	
	void echoConnection::destroyVM( )
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

	void echoConnection::printMemStats( )
	{
		int kbytes = lua_gc( l, LUA_GCCOUNT, 0 );
		int bytes = lua_gc( l, LUA_GCCOUNTB, 0 );
		LOG_TRACE << "LUA VM memory in use: " << kbytes << "." << bytes << " kBytes";
	}

	echoConnection::echoConnection( const Network::LocalTCPendpoint& local )
	{
		LOG_TRACE << "Created connection handler for " << local.toString();
		createVM( );
		printMemStats( );
	}


	echoConnection::echoConnection( const Network::LocalSSLendpoint& local )
	{
		LOG_TRACE << "Created connection handler (SSL) for " << local.toString();
		createVM( );
		printMemStats( );
	}

	echoConnection::~echoConnection()
	{
		LOG_TRACE << "Connection handler destroyed";
		destroyVM( );
	}

	void echoConnection::setPeer( const Network::RemoteTCPendpoint& remote )
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
		printMemStats( );
	}

	void echoConnection::setPeer( const Network::RemoteSSLendpoint& remote )
	{
		LOG_TRACE << "Peer set to " << remote.toString();
		LOG_TRACE << "Peer Common Name: " << remote.commonName();

		lua_pushstring( l, "new_connection" );
		lua_gettable( l, LUA_GLOBALSINDEX );
		lua_pushstring( l, remote.host( ).c_str( ) );
		unsigned short port = remote.port( );
		std::stringstream ss;
		ss << port;
		lua_pushstring( l, ss.str( ).c_str( ) );
		lua_pushstring( l, remote.commonName( ).c_str( ) );
		int res = lua_pcall( l, 3, 0, 0 );
		if( res != 0 ) {
			LOG_FATAL << "Unable to call 'new_connection' function: " << lua_tostring( l, -1 );
			lua_pop( l, 1 );
			throw new std::runtime_error( "Error in LUA processor" );
		}
		printMemStats( );
	}

	/// Handle a request and produce a reply.
	Network::NetworkOperation echoConnection::nextOperation()
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
		if( !strcmp( op, "WRITE" ) ) {
			const char *msg = lua_tostring( l, -1 );
			lua_pop( l, 2 );
			return Network::NetworkOperation( Network::NetworkOperation::WRITE,
							  msg, strlen( msg ) );
		} else if( !strcmp( op, "READ" ) ) {
			int size = lua_tointeger( l, -1 );
			lua_pop( l, 2 );
			return Network::NetworkOperation( Network::NetworkOperation::READ, size );
		} else if( !strcmp( op, "TERMINATE" ) ) {
			lua_pop( l, 2 );
			return Network::NetworkOperation( Network::NetworkOperation::TERMINATE );
		} else {
			lua_pop( l, 2 );
			LOG_FATAL << "Lua code returns '" << op << "', expecting one of 'READ', 'WRITE', 'TERMINATE'!";
			throw new std::runtime_error( "Error in LUA processor" );
		}
		printMemStats( );
	}
	
	/// Parse incoming data. The return value indicates how much of the
	/// input has been consumed.
	void* echoConnection::parseInput( const void *begin, std::size_t bytesTransferred )
	{
		char *s = (char *)begin;
		s[bytesTransferred] = '\0';
		
		lua_pushstring( l, "parse_input" );
		lua_gettable( l, LUA_GLOBALSINDEX );
		lua_pushstring( l, s );
		int res = lua_pcall( l, 1, 0, 0 );
		if( res != 0 ) {
			LOG_FATAL << "Unable to call 'parse_input' function: " << lua_tostring( l, -1 );
			lua_pop( l, 1 );
			throw new std::runtime_error( "Error in LUA processor" );
		}
		
		return s;
	}

	void echoConnection::timeoutOccured()
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

	void echoConnection::signalOccured()
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

	/// ServerHandler PIMPL
	Network::connectionHandler* ServerHandler::ServerHandlerImpl::newConnection( const Network::LocalTCPendpoint& local )
	{
		return new echoConnection( local );
	}

	Network::connectionHandler* ServerHandler::ServerHandlerImpl::newSSLconnection( const Network::LocalSSLendpoint& local )
	{
		return new echoConnection( local );
	}

	ServerHandler::ServerHandler() : impl_( new ServerHandlerImpl )	{}

	ServerHandler::~ServerHandler()	{ delete impl_; }

	Network::connectionHandler* ServerHandler::newConnection( const Network::LocalTCPendpoint& local )
	{
		return impl_->newConnection( local );
	}

	Network::connectionHandler* ServerHandler::newSSLconnection( const Network::LocalSSLendpoint& local )
	{
		return impl_->newSSLconnection( local );
	}

} // namespace _SMERP
