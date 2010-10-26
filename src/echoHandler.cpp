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
	}


	/// Handle a request and produce a reply.
	NetworkOperation echoConnection::nextOperation()
	{
		switch( state_ )	{
		case NEW:	{
			state_ = HELLO;
			std::string msg = "Welcome to SMERP.\n";
			return NetworkOperation( NetworkOperation::WRITE, msg.c_str(), msg.length());
		}

		case HELLO:
			state_ = ANSWERING;
			if ( buffer.empty() )
				return NetworkOperation( NetworkOperation::WRITE, buffer.c_str(), buffer.length() );
			else	{
				std::string msg = "BUFFER NOT EMPTY!\n";
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

		case TIMEOUT:	{
			state_ = TERMINATING;
			std::string msg = "Timeout. :P\n";
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
		LOG_TRACE << "Timeout id: " << ID << " occured";
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
