//
// baseConnection.cpp
//

#include "baseConnection.hpp"
#include "logger.hpp"
#include "requestHandler.hpp"

#include <boost/bind.hpp>

namespace _SMERP {

baseConnection::baseConnection( boost::asio::io_service& IOservice,
				requestHandler& handler,
				unsigned long idleTimeout, unsigned long requestTimeout,
				unsigned long processTimeout, unsigned long answerTimeout ) :
	strand_( IOservice ),
	requestHandler_( handler ),
	timer_( IOservice )
{
	idleTimeout_ = idleTimeout;
	requestTimeout_ = requestTimeout;
	processTimeout_ = processTimeout;
	answerTimeout_ = answerTimeout;
	timerType_ = TIMEOUT_NONE;
	LOG_TRACE << "New base connection created";
}


void baseConnection::handleRead( const boost::system::error_code& e, std::size_t bytesTransferred )
{
	if ( !e )	{
		LOG_TRACE << "Read " << bytesTransferred << " bytes from " << identifier();

		request_.parseInput( buffer_.data(), bytesTransferred );

		switch ( request_.status())	{
		case request::READY:
			setTimeout( TIMEOUT_PROCESSING );
			requestHandler_.handleRequest( request_, reply_ );

			boost::asio::async_write( socket(), reply_.toBuffers(),
						  strand_.wrap( boost::bind( &baseConnection::handleWrite,
									     shared_from_this(),
									     boost::asio::placeholders::error )));
		case request::EMPTY:
		case request::PARSING:
			socket().async_read_some( boost::asio::buffer( buffer_ ),
						  strand_.wrap( boost::bind( &baseConnection::handleRead,
									     shared_from_this(),
									     boost::asio::placeholders::error,
									     boost::asio::placeholders::bytes_transferred )));
		}
	}

	// If an error occurs then no new asynchronous operations are started. This
	// means that all shared_ptr references to the connection object will
	// disappear and the object will be destroyed automatically after this
	// handler returns. The connection class's destructor closes the socket.
}


void baseConnection::handleWrite( const boost::system::error_code& e )
{
	if ( !e )	{
		boost::system::error_code ignored_ec;

		// Cancel timer.
		setTimeout( TIMEOUT_NONE );

		boost::asio::write( socket(), boost::asio::buffer( "Bye.\n" ));
		// Initiate graceful connection closure.
		socket().shutdown( boost::asio::ip::tcp::socket::shutdown_both, ignored_ec );
	}

	// No new asynchronous operations are started. This means that all shared_ptr
	// references to the connection object will disappear and the object will be
	// destroyed automatically after this handler returns. The connection class's
	// destructor closes the socket.
}


void baseConnection::handleTimeout( const boost::system::error_code& e )
{
	if ( !e )	{
		boost::system::error_code ignored_ec;

		boost::asio::write( socket(), boost::asio::buffer( "Timeout :P\n" ));
		socket().shutdown( boost::asio::ip::tcp::socket::shutdown_both, ignored_ec );
		LOG_INFO << "Timeout, client " << identifier();
	}
}


void baseConnection::setTimeout( const TimeOutType type )
{
	const char	*typeName;

	if ( timerType_ != type )	{
		unsigned long timeout;
		switch( type )	{
		case TIMEOUT_IDLE:
			timeout = idleTimeout_;
			typeName = "IDLE";
			break;
		case TIMEOUT_REQUEST:
			timeout = requestTimeout_;
			typeName = "REQUEST";
			break;
		case TIMEOUT_PROCESSING:
			timeout = processTimeout_;
			typeName = "PROCESSING";
			break;
		case TIMEOUT_ANSWER:
			timeout = answerTimeout_;
			typeName = "ANSWER";
			break;
		case TIMEOUT_NONE:
		default:
			timeout = 0;
			typeName = "NONE";
		}

		timer_.cancel();

		if ( timeout > 0 )	{
			timer_.expires_from_now( boost::posix_time::seconds( timeout ));
			timer_.async_wait( strand_.wrap( boost::bind( &baseConnection::handleTimeout,
								      shared_from_this(),
								      boost::asio::placeholders::error )));
			timerType_ = type;
			LOG_TRACE << typeName << " timer for " << identifier() << " set to " << timeout << " s";
		}
		else	{
			LOG_TRACE << typeName << " timer for " << identifier() << " disabled";
			timerType_ = TIMEOUT_NONE;
		}
	}
}

} // namespace _SMERP
