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
	networkOperation echoConnection::nextOperation()
	{
		networkOperation	op;

		switch( state_ )	{
		case NEW:
			op.msg = NetworkMessage( "Welcome to SMERP.\n" );
			op.operation = networkOperation::WRITE;
			state_ = READING;
			break;

		case READING:
			if ( ! buffer.empty() )
				op.msg = NetworkMessage( buffer );
			state_ = ANSWERING;
			op.operation = networkOperation::WRITE;
			break;

		case ANSWERING:
			buffer.clear();
			state_ = READING;
			op.operation = networkOperation::READ;
			break;

		case FINISHING:
			op.msg = NetworkMessage( "Thanks for using SMERP.\n" );
			op.operation = networkOperation::WRITE;
			state_ = TERMINATING;
			break;

		case TERMINATING:
			buffer.clear();
			op.operation = networkOperation::TERMINATE;
			break;
		}
		return op;
	}


	/// Parse incoming data. The return value indicates how much of the
	/// input has been consumed.
	char *echoConnection::parseInput( char *begin, std::size_t bytesTransferred )
	{
		if ( !strncmp( "QUIT", begin, 4 ))
			state_ = FINISHING;
		else	{
			for ( std::size_t i = 0; i < bytesTransferred; i++ )	{
				if ( *begin != '\n' )
					buffer += *begin;
				else	{
					buffer += *begin++;
					return( begin );
				}
				begin++;
			}
		}
		return( begin );
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
