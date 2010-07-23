//
// connection.cpp
//

#include "connection.hpp"
#include "logger.hpp"
#include "requestHandler.hpp"
#include "connContext.hpp"

#include <vector>
#include <boost/bind.hpp>

namespace _SMERP {

connection::connection( boost::asio::io_service& IOservice, requestHandler& handler, long timeoutDuration ) :
	strand_( IOservice ),
	socket_( IOservice ),
	requestHandler_( handler ),
	timer_( IOservice )
{
	timeoutDuration_ = timeoutDuration;
	LOG_TRACE << "New socket created";
}


void connection::start()
{
	LOG_TRACE << "Starting unencrypted connection to " << socket().remote_endpoint().address().to_string()
			<< ":" << socket().remote_endpoint().port();

	boost::asio::write( socket(), boost::asio::buffer( "Welcome to bla bla bla\n" ));
	LOG_TRACE << "Wrote welcome message to " << socket().remote_endpoint().address().to_string()
			<< ":" << socket().remote_endpoint().port();

	socket_.async_read_some( boost::asio::buffer( buffer_ ),
				  strand_.wrap( boost::bind( &connection::handleRead, shared_from_this(),
							     boost::asio::placeholders::error,
							     boost::asio::placeholders::bytes_transferred )));

	timer_.expires_from_now( boost::posix_time::seconds( timeoutDuration_ ));
	timer_.async_wait( strand_.wrap( boost::bind( &connection::handleTimeout, shared_from_this(),
						      boost::asio::placeholders::error )));
	LOG_TRACE << "Expiring timer for " << socket().remote_endpoint().address().to_string()
			<< ":" << socket().remote_endpoint().port() << " set to " << timeoutDuration_ << " s";
}


void connection::handleRead( const boost::system::error_code& e, std::size_t bytes_transferred )
{
	if ( !e )	{
		LOG_TRACE << "Read " << bytes_transferred << " bytes from "
				<< socket().remote_endpoint().address().to_string()
				<< ":" << socket().remote_endpoint().port();

		request_.parse( buffer_.data(), buffer_.data() + bytes_transferred );

		switch ( request_.status())	{
		case request::READY:
			timer_.cancel();
			timer_.expires_from_now( boost::posix_time::seconds( timeoutDuration_ ));
			timer_.async_wait( strand_.wrap( boost::bind( &connection::handleTimeout,
								      shared_from_this(),
								      boost::asio::placeholders::error )));

			requestHandler_.handleRequest( request_, reply_ );

			boost::asio::async_write( socket(), reply_.toBuffers(),
						  strand_.wrap( boost::bind( &connection::handleWrite,
									     shared_from_this(),
									     boost::asio::placeholders::error )));
		case request::EMPTY:
		case request::PARSING:

			socket().async_read_some( boost::asio::buffer( buffer_ ),
						  strand_.wrap( boost::bind( &connection::handleRead,
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

void connection::handleWrite( const boost::system::error_code& e )
{
	if ( !e )	{
		boost::system::error_code ignored_ec;

		// Cancel timer.
		timer_.cancel();

		boost::asio::write( socket(), boost::asio::buffer( "Bye.\n" ));
		// Initiate graceful connection closure.
		socket().shutdown( boost::asio::ip::tcp::socket::shutdown_both, ignored_ec );
	}

	// No new asynchronous operations are started. This means that all shared_ptr
	// references to the connection object will disappear and the object will be
	// destroyed automatically after this handler returns. The connection class's
	// destructor closes the socket.
}


// Initiate graceful connection closure. This will cause all outstanding
// asynchronous read or write operations to be canceled.
void connection::handleTimeout( const boost::system::error_code& e )
{
	if ( !e )	{
		boost::system::error_code ignored_ec;

		boost::asio::write( socket(), boost::asio::buffer( "Timeout :P\n" ));
		socket().shutdown( boost::asio::ip::tcp::socket::shutdown_both, ignored_ec );
		LOG_INFO << "Timeout, client " << socket().remote_endpoint().address().to_string()
				<< ":" << socket().remote_endpoint().port();
	}
}


} // namespace _SMERP
