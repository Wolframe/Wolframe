//
// connectionHandler.cpp
//

#include "echoHandler.hpp"
#include "logger.hpp"

#include <string>
#include <cstring>

namespace _SMERP {

	echoConnection::echoConnection( const connectionPeer& local )
	{
		LOG_TRACE << "Created connection handler for " << local.address << ":" << local.port;
		state_ = NEW;
	}


	echoConnection::echoConnection( const SSLconnectionPeer& local )
	{
		LOG_TRACE << "Created connection handler (SSL) for " << local.address << ":" << local.port;
		state_ = NEW;
	}

	echoConnection::~echoConnection()
	{
		LOG_TRACE << "Connection handler destroyed";
	}

	void echoConnection::setPeer( const connectionPeer& remote )
	{
		LOG_TRACE << "Peer set to " << remote.address << ":" << remote.port;
	}

	void echoConnection::setPeer( const SSLconnectionPeer& remote )
	{
		LOG_TRACE << "Peer set to " << remote.address << ":" << remote.port;
	}


	/// Handle a request and produce a reply.
	NetworkOperation echoConnection::nextOperation()
	{
		switch( state_ )	{
		case NEW:
			state_ = READING;
			return NetworkOperation( NetworkOperation::WRITE, "Welcome to SMERP.\n" );

		case READING:
			state_ = ANSWERING;
			if ( ! buffer.empty() )
				return NetworkOperation( NetworkOperation::WRITE, buffer );
			else
				return NetworkOperation( NetworkOperation::WRITE, "EMPTY BUFFER !\n" );

		case ANSWERING:
			buffer.clear();
			state_ = READING;
			return NetworkOperation( NetworkOperation::READ );

		case FINISHING:
			state_ = TERMINATING;
			return NetworkOperation( NetworkOperation::WRITE, "Thanks for using SMERP.\n" );

		case TERMINATING:
			buffer.clear();
			return NetworkOperation( NetworkOperation::TERMINATE );
		}
		return NetworkOperation( NetworkOperation::TERMINATE );
	}


	/// Parse incoming data. The return value indicates how much of the
	/// input has been consumed.
	void* echoConnection::parseInput( const void *begin, std::size_t bytesTransferred )
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


	connectionHandler* echoServer::newConnection( const connectionPeer& local )
	{
		return new echoConnection( local );
	}

	connectionHandler* echoServer::newSSLconnection( const SSLconnectionPeer& local )
	{
		return new echoConnection( local );
	}

} // namespace _SMERP
