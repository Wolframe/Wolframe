//
// connectionHandler.cpp
//

#include "smerpHandler.hpp"
#include "logger.hpp"

#include <string>
#include <cstring>

namespace _SMERP {

	echoConnection::echoConnection( const Network::LocalTCPendpoint& local )
	{
		LOG_TRACE << "Created connection handler for " << local.toString();
		state_ = NEW;
	}


	echoConnection::echoConnection( const Network::LocalSSLendpoint& local )
	{
		LOG_TRACE << "Created connection handler (SSL) for " << local.toString();
		state_ = NEW;
	}

	echoConnection::~echoConnection()
	{
		LOG_TRACE << "Connection handler destroyed";
	}

	void echoConnection::setPeer( const Network::RemoteTCPendpoint& remote )
	{
		LOG_TRACE << "Peer set to " << remote.toString();
	}

	void echoConnection::setPeer( const Network::RemoteSSLendpoint& remote )
	{
		LOG_TRACE << "Peer set to " << remote.toString();
		LOG_TRACE << "Peer Common Name: " << remote.commonName();
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
