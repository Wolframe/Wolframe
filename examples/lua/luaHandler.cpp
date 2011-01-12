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
	}
	
	void echoConnection::destroyVM( )
	{
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
		state_ = NEW;
	}


	echoConnection::echoConnection( const Network::LocalSSLendpoint& local )
	{
		LOG_TRACE << "Created connection handler (SSL) for " << local.toString();
		createVM( );
		printMemStats( );
		state_ = NEW;
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
			throw new std::runtime_error( "Error in destruction of LUA processor" );
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
			throw new std::runtime_error( "Error in destruction of LUA processor" );
		}
		printMemStats( );
	}


	/// Handle a request and produce a reply.
	Network::NetworkOperation echoConnection::nextOperation()
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
	void* echoConnection::parseInput( const void *begin, std::size_t bytesTransferred )
	{
		char *s = (char *)begin;
		for( std::size_t i = 0; i < bytesTransferred; i++ ) {
			if( *s != '\n' ) {
				buffer_ += *s;
			} else {
				// don't pass the end of line to Lua
				s++;
				lua_pushstring( l, "got_line" );
				lua_gettable( l, LUA_GLOBALSINDEX );
				lua_pushstring( l, buffer_.c_str( ) );
				int res = lua_pcall( l, 1, 1, 0 );
				if( res != 0 ) {
					LOG_FATAL << "Unable to call 'got_line' function: " << lua_tostring( l, -1 );
					lua_pop( l, 1 );
					throw new std::runtime_error( "Error in destruction of LUA processor" );
				}
				bool cont = lua_toboolean( l, lua_gettop( l ) );
				if( !cont ) {
					state_ = FINISHING;
					return s;
				} else {
					buffer_ += '\n';
					return s;
				}
			}
			s++;
		}
		return s;
	}

	void echoConnection::timeoutOccured()
	{
		state_ = TIMEOUT;
		LOG_TRACE << "Processor received timeout";
	}

	void echoConnection::signalOccured()
	{
		state_ = SIGNALLED;
		LOG_TRACE << "Processor received signal";
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
