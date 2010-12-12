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

	luaConnection::luaConnection( const Network::LocalTCPendpoint& local )
	{
		LOG_TRACE << "Created connection handler for " << local.toString();
		state_ = NEW;
	}


	luaConnection::luaConnection( const Network::LocalSSLendpoint& local )
	{
		LOG_TRACE << "Created connection handler (SSL) for " << local.toString();
		state_ = NEW;
	}

	luaConnection::~luaConnection()
	{
		LOG_TRACE << "Connection handler destroyed";
	}

	void luaConnection::setPeer( const Network::RemoteTCPendpoint& remote )
	{
		LOG_TRACE << "Peer set to " << remote.toString();
	}

	void luaConnection::setPeer( const Network::RemoteSSLendpoint& remote )
	{
		LOG_TRACE << "Peer set to " << remote.toString();
		LOG_TRACE << "Peer Common Name: " << remote.commonName();
	}


	/// Handle a request and produce a reply.
	Network::NetworkOperation luaConnection::nextOperation()
	{
		switch( state_ )	{
		case NEW:	{
			state_ = HELLO;
			const char *msg = "Welcome to SMERP.\n";
			return Network::NetworkOperation( Network::NetworkOperation::WRITE,
							  msg, strlen( msg ));
		}

		case HELLO:
			state_ = ANSWERING;
			if ( buffer_.empty() )
				return Network::NetworkOperation( Network::NetworkOperation::WRITE,
								  buffer_.c_str(), buffer_.length() );
			else	{
				const char *msg = "BUFFER NOT EMPTY!\n";
				return Network::NetworkOperation( Network::NetworkOperation::WRITE,
								  msg, strlen( msg ));
			}

		case READING:
			state_ = ANSWERING;
			if ( ! buffer_.empty() )
				return Network::NetworkOperation( Network::NetworkOperation::WRITE,
								  buffer_.c_str(), buffer_.length() );
			else	{
				const char *msg = "EMPTY BUFFER !\n";
				return Network::NetworkOperation( Network::NetworkOperation::WRITE,
								  msg, strlen( msg ));
			}

		case ANSWERING:
			buffer_.clear();
			state_ = READING;
			return Network::NetworkOperation( Network::NetworkOperation::READ, 30 );

		case FINISHING:	{
			state_ = TERMINATING;
			const char *msg = "Thanks for using SMERP.\n";
			return Network::NetworkOperation( Network::NetworkOperation::WRITE,
							  msg, strlen( msg ));
		}

		case TIMEOUT:	{
			state_ = TERMINATING;
			const char *msg = "Timeout. :P\n";
			return Network::NetworkOperation( Network::NetworkOperation::WRITE,
							  msg, strlen( msg ));
		}

		case SIGNALLED:	{
			state_ = TERMINATING;
			const char *msg = "Server is shutting down. :P\n";
			return Network::NetworkOperation( Network::NetworkOperation::WRITE,
							  msg, strlen( msg ));
		}

		case TERMINATING:
			return Network::NetworkOperation( Network::NetworkOperation::TERMINATE );
		}
		return Network::NetworkOperation( Network::NetworkOperation::TERMINATE );
	}


	/// Parse incoming data. The return value indicates how much of the
	/// input has been consumed.
	void* luaConnection::parseInput( const void *begin, std::size_t bytesTransferred )
	{
		char *s = (char *)begin;
		if ( !strncmp( "quit", s, 4 ))
			state_ = FINISHING;
		else	{
			for ( std::size_t i = 0; i < bytesTransferred; i++ )	{
				if ( *s != '\n' )
					buffer_ += *s;
				else	{
					buffer_ += *s++;
					return( s );
				}
				s++;
			}
		}
		return( s );
	}

	void luaConnection::timeoutOccured()
	{
		state_ = TIMEOUT;
		LOG_TRACE << "Processor received timeout";
	}

	void luaConnection::signalOccured()
	{
		state_ = SIGNALLED;
		LOG_TRACE << "Processor received signal";
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

	Network::connectionHandler* luaServer::newConnection( const Network::LocalTCPendpoint& local )
	{
		return new luaConnection( local );
	}

	Network::connectionHandler* luaServer::newSSLconnection( const Network::LocalSSLendpoint& local )
	{
		return new luaConnection( local );
	}

} // namespace _SMERP
