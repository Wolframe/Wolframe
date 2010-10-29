//
// connectionHandler.cpp
//

#include "echoHandler.hpp"
#include "logger.hpp"

#include <string>
#include <cstring>

namespace _SMERP {

	echoConnection::echoConnection( const LocalTCPendpoint& local )
	{
		LOG_TRACE << "Created connection handler for " << local.toString();
		state_ = NEW;
	}


	echoConnection::echoConnection( const LocalSSLendpoint& local )
	{
		LOG_TRACE << "Created connection handler (SSL) for " << local.toString();
		state_ = NEW;
	}

	echoConnection::~echoConnection()
	{
		LOG_TRACE << "Connection handler destroyed";
	}

	void echoConnection::setPeer( const RemoteTCPendpoint& remote )
	{
		LOG_TRACE << "Peer set to " << remote.toString();
	}

	void echoConnection::setPeer( const RemoteSSLendpoint& remote )
	{
		LOG_TRACE << "Peer set to " << remote.toString();
		LOG_TRACE << "Peer Common Name: " << remote.commonName();
	}


	/// Handle a request and produce a reply.
	NetworkOperation echoConnection::nextOperation()
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

	void echoConnection::timeoutOccured( unsigned ID )
	{
		state_ = TIMEOUT;
		LOG_TRACE << "Processor received timeout, id: " << ID;
	}


	connectionHandler* echoServer::newConnection( const LocalTCPendpoint& local )
	{
		return new echoConnection( local );
	}

	connectionHandler* echoServer::newSSLconnection( const LocalSSLendpoint& local )
	{
		return new echoConnection( local );
	}

} // namespace _SMERP
