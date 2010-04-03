//
// connection.cpp
//

#include "connection.hpp"
#include "logger.hpp"
#include "requestHandler.hpp"

#include <vector>
#include <boost/bind.hpp>

namespace _SMERP {

connection::connection( boost::asio::io_service& io_service, requestHandler& handler, long timeoutDuration,
			boost::asio::ssl::context *SSLcontext ) :
	strand_( io_service ),
	requestHandler_( handler ),
	timer_( io_service ),
	timeoutDuration_( timeoutDuration )
{
	if ( SSLcontext )	{
		isSSL_ = true;
		SSLsocket_ = new ssl_socket( io_service, *SSLcontext );
		LOG_TRACE << "New SSL socket created";
	}
	else	{
		isSSL_ = false;
		socket_ = new boost::asio::ip::tcp::socket( io_service );
		LOG_TRACE << "New socket created";
	}
}


boost::asio::ip::tcp::socket& connection::socket()
{
	return *socket_;
}


ssl_socket::lowest_layer_type& connection::SSLsocket()
{
	return SSLsocket_->lowest_layer();
}



void connection::start()
{
	if ( isSSL_ )	{
		LOG_TRACE << "Starting SSL handshake with " << SSLsocket().remote_endpoint().address().to_string()
			  << ":" << SSLsocket().remote_endpoint().port();
		SSLsocket_->async_handshake( boost::asio::ssl::stream_base::server,
					     boost::bind( &connection::handleHandshake, this,
							  boost::asio::placeholders::error ));
	}
	else	{
		LOG_TRACE << "Writing welcome message to " << socket_->remote_endpoint().address().to_string()
			  << ":" << SSLsocket().remote_endpoint().port();
		boost::asio::write( *socket_, boost::asio::buffer( "Welcome to bla bla bla\n" ));

		LOG_TRACE << "Reading data from " << SSLsocket().remote_endpoint().address().to_string()
			  << ":" << SSLsocket().remote_endpoint().port();
		socket_->async_read_some( boost::asio::buffer( buffer_),
					  strand_.wrap( boost::bind( &connection::handle_read, shared_from_this(),
								     boost::asio::placeholders::error,
								     boost::asio::placeholders::bytes_transferred )));

		timer_.expires_from_now( boost::posix_time::milliseconds( timeoutDuration_ ));
		timer_.async_wait( strand_.wrap( boost::bind( &connection::handleTimeout, shared_from_this(),
							      boost::asio::placeholders::error )));
		LOG_TRACE << "Expiring timer for " << SSLsocket().remote_endpoint().address().to_string()
			  << ":" << SSLsocket().remote_endpoint().port() << " set up to " << timeoutDuration_ << " ms";
	}
}


void connection::handleHandshake( const boost::system::error_code& e )
{
	if ( !e )	{
		LOG_TRACE << "Writing welcome message to " << SSLsocket().remote_endpoint().address().to_string()
			  << ":" << SSLsocket().remote_endpoint().port() << "(SSL)";
		boost::asio::write( *SSLsocket_, boost::asio::buffer( "Welcome to SSL bla bla bla\n" ));

		LOG_TRACE << "Reading data from " << SSLsocket().remote_endpoint().address().to_string()
			  << ":" << SSLsocket().remote_endpoint().port() << "(SSL)";
		SSLsocket_->async_read_some( boost::asio::buffer( buffer_ ),
					     strand_.wrap( boost::bind( &connection::handle_read, this,
									boost::asio::placeholders::error,
									boost::asio::placeholders::bytes_transferred )));
		timer_.expires_from_now( boost::posix_time::milliseconds( timeoutDuration_ ));
		timer_.async_wait( strand_.wrap( boost::bind( &connection::handleTimeout, shared_from_this(),
							      boost::asio::placeholders::error )));
		LOG_TRACE << "Expiring timer for " << SSLsocket().remote_endpoint().address().to_string()
			  << ":" << SSLsocket().remote_endpoint().port() << "(SSL) set up to " << timeoutDuration_ << " ms";
	}
	else	{
		LOG_DEBUG << "ERROR handling SSL handshake from " << SSLsocket().remote_endpoint().address().to_string()
			  << ":" << SSLsocket().remote_endpoint().port();
		delete this;
	}
}


void connection::handle_read( const boost::system::error_code& e, std::size_t bytes_transferred )
{
	if ( !e )	{
		request_.parse( buffer_.data(), buffer_.data() + bytes_transferred );

		switch ( request_.status())	{
			case request::READY:
				timer_.cancel();
				timer_.expires_from_now( boost::posix_time::milliseconds( timeoutDuration_ ));
				timer_.async_wait( strand_.wrap( boost::bind( &connection::handleTimeout, shared_from_this(),
									boost::asio::placeholders::error )));

				requestHandler_.handleRequest( request_, reply_ );
				if ( isSSL_ )
					boost::asio::async_write( *SSLsocket_, reply_.toBuffers(),
							strand_.wrap( boost::bind( &connection::handle_write, shared_from_this(),
										boost::asio::placeholders::error )));
				else
					boost::asio::async_write( *socket_, reply_.toBuffers(),
							strand_.wrap( boost::bind( &connection::handle_write, shared_from_this(),
										boost::asio::placeholders::error )));
			case request::EMPTY:
			case request::PARSING:
				if ( isSSL_ )
					SSLsocket_->async_read_some( boost::asio::buffer( buffer_ ),
							strand_.wrap( boost::bind( &connection::handle_read, shared_from_this(),
										boost::asio::placeholders::error,
										boost::asio::placeholders::bytes_transferred )));
				else
					socket_->async_read_some( boost::asio::buffer( buffer_ ),
							strand_.wrap( boost::bind( &connection::handle_read, shared_from_this(),
										boost::asio::placeholders::error,
										boost::asio::placeholders::bytes_transferred )));
			}
	}

// If an error occurs then no new asynchronous operations are started. This
// means that all shared_ptr references to the connection object will
// disappear and the object will be destroyed automatically after this
// handler returns. The connection class's destructor closes the socket.
}

void connection::handle_write( const boost::system::error_code& e )
{
	if ( !e )	{
		boost::system::error_code ignored_ec;

		// Cancel timer.
		timer_.cancel();

		if ( isSSL_ )	{
			boost::asio::write( *SSLsocket_, boost::asio::buffer( "SSL Bye.\n" ));
			// Initiate graceful connection closure.
			SSLsocket_->lowest_layer().shutdown( boost::asio::ip::tcp::socket::shutdown_both, ignored_ec );
		}
		else	{
			boost::asio::write( *socket_, boost::asio::buffer( "Bye.\n" ));
			// Initiate graceful connection closure.
			socket_->shutdown( boost::asio::ip::tcp::socket::shutdown_both, ignored_ec );
		}
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

		if ( isSSL_ )	{
			boost::asio::write( *SSLsocket_, boost::asio::buffer( "Timeout :P\n" ));
			SSLsocket_->lowest_layer().shutdown( boost::asio::ip::tcp::socket::shutdown_both, ignored_ec );
		}
		else	{
			boost::asio::write( *socket_, boost::asio::buffer( "Timeout :P\n" ));
			socket_->shutdown( boost::asio::ip::tcp::socket::shutdown_both, ignored_ec );
		}
	}
}


} // namespace _SMERP
