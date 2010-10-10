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
			state_ = READING;
			std::string msg = "Welcome to SMERP.\n";
			return NetworkOperation( NetworkOperation::WRITE, msg.c_str(), msg.length());
		}

		case READING:
			state_ = ANSWERING;
			if ( ! buffer.empty() )
				return NetworkOperation( NetworkOperation::WRITE, buffer.c_str(), buffer.length() );
			else	{
				std::string msg = "EMPTY BUFFER !\n";
				return NetworkOperation( NetworkOperation::WRITE, msg.c_str(), msg.length());
			}

		case ANSWERING:
			buffer.clear();
			state_ = READING;
			return NetworkOperation( NetworkOperation::READ );

		case FINISHING:	{
			state_ = TERMINATING;
			std::string msg = "Thanks for using SMERP.\n";
			return NetworkOperation( NetworkOperation::WRITE, msg.c_str(), msg.length());
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


	luaServer::luaServer( ) : ServerHandler( )
	{
		l = luaL_newstate( );

		// TODO: open standard libraries, most likely something to configure later
		luaL_openlibs( l );

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
