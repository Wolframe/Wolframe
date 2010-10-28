//
// luaHandler.cpp
//

#include "luaHandler.hpp"
#include "logger.hpp"

#include <string>
#include <cstring>
#include <stdexcept>

extern "C" {
#include <lualib.h>
#include <lauxlib.h>
}

namespace _SMERP {

	luaConnection::luaConnection( const LocalTCPendpoint& local )
	{
		LOG_TRACE << "Created connection handler for " << local.toString();
		state_ = NEW;
	}


	luaConnection::luaConnection( const LocalSSLendpoint& local )
	{
		LOG_TRACE << "Created connection handler (SSL) for " << local.toString();
		state_ = NEW;
	}

	luaConnection::~luaConnection()
	{
		LOG_TRACE << "Connection handler destroyed";
	}

	void luaConnection::setPeer( const RemoteTCPendpoint& remote )
	{
		LOG_TRACE << "Peer set to " << remote.toString();
	}

	void luaConnection::setPeer( const RemoteSSLendpoint& remote )
	{
		LOG_TRACE << "Peer set to " << remote.toString();
	}


	/// Handle a request and produce a reply.
	NetworkOperation luaConnection::nextOperation()
	{
		switch( state_ )	{
		case NEW:	{
			state_ = HELLO;
			const char *msg = "Welcome to SMERP.\n";
			return NetworkOperation( NetworkOperation::WRITE, msg, strlen( msg ));
		}

		case HELLO:
			state_ = ANSWERING;
			if ( buffer.empty() )
				return NetworkOperation( NetworkOperation::WRITE, buffer.c_str(), buffer.length() );
			else	{
				const char *msg = "BUFFER NOT EMPTY!\n";
				return NetworkOperation( NetworkOperation::WRITE, msg, strlen( msg ));
			}

		case READING:
			state_ = ANSWERING;
			if ( ! buffer.empty() )
				return NetworkOperation( NetworkOperation::WRITE, buffer.c_str(), buffer.length() );
			else	{
				const char *msg = "EMPTY BUFFER !\n";
				return NetworkOperation( NetworkOperation::WRITE, msg, strlen( msg ));
			}

		case ANSWERING:
			buffer.clear();
			state_ = READING;
			return NetworkOperation( NetworkOperation::READ, 30, 1 );

		case FINISHING:	{
			state_ = TERMINATING;
			const char *msg = "Thanks for using SMERP.\n";
			return NetworkOperation( NetworkOperation::WRITE, msg, strlen( msg ));
		}

		case TIMEOUT:	{
			state_ = TERMINATING;
			const char *msg = "Timeout. :P\n";
			return NetworkOperation( NetworkOperation::WRITE, msg, strlen( msg ));
		}

		case TERMINATING:
			buffer.clear();
			return NetworkOperation( NetworkOperation::TERMINATE );
		}
		return NetworkOperation( NetworkOperation::TERMINATE );
	}


	/// Parse incoming data. The return value indicates how much of the
	/// input has been consumed.
	void* luaConnection::parseInput( const void *begin, std::size_t bytesTransferred )
	{
		char *s = (char *)begin;
		if ( !strncmp( "QUIT", s, 4 ))
			state_ = FINISHING;
		else	{
			for ( std::size_t i = 0; i < bytesTransferred; i++ )	{
				if ( *s != '\n' )
					buffer += *s;
				else	{
					buffer += *s++;
					return( s );
				}
				s++;
			}
		}
		return( s );
	}

	void luaConnection::timeoutOccured( unsigned ID )
	{
		state_ = TIMEOUT;
		LOG_TRACE << "Timeout id: " << ID << " occured";
	}


	luaServer::luaServer( ) : ServerHandler( )
	{
		// instanitate a new VM
		l = luaL_newstate( );
		if( !l ) {
			LOG_FATAL << "Unable to create new LUA engine!";
			throw new std::runtime_error( "Can't initialize LUA processor" );			
		}

		// TODO: open standard libraries, most likely something to configure later,
		// the plain echo processor should work without any lua libraries
		//luaL_openlibs( l );
		// or open them individually, see:
		// http://stackoverflow.com/questions/966162/best-way-to-omit-lua-standard-libraries
		lua_pushcfunction( l, luaopen_base );
		lua_pushstring( l, "" );
		lua_call( l, 1, 0 );
		lua_pushcfunction( l, luaopen_io );
		lua_pushstring( l, LUA_LOADLIBNAME );
		lua_call( l, 1, 0 );

		// TODO: script location, also configurable
		int res = luaL_loadfile( l, "echo.lua" );
		if( res != 0 ) {
			LOG_FATAL << "Unable to load LUA code 'echo.lua': " << lua_tostring( l, -1 );
			lua_pop( l, 1 );
			throw new std::runtime_error( "Can't initialize LUA processor" );
		}

		// TODO: careful here with threads and how they get generated!

		// call main, we may have to initialized LUA modules there
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
	}

	luaServer::~luaServer( )
	{
		// give LUA code a chance to clean up resources or something
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
	}

	connectionHandler* luaServer::newConnection( const LocalTCPendpoint& local )
	{
		return new luaConnection( local );
	}

	connectionHandler* luaServer::newSSLconnection( const LocalSSLendpoint& local )
	{
		return new luaConnection( local );
	}

} // namespace _SMERP
