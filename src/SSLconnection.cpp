//
// connection.cpp
//

#include "SSLconnection.hpp"
#include "logger.hpp"
#include "requestHandler.hpp"

#include <boost/bind.hpp>

namespace _SMERP {

SSLconnection::SSLconnection( boost::asio::io_service& IOservice, requestHandler& handler, long timeoutDuration,
			boost::asio::ssl::context& SSLcontext ) :
	strand_( IOservice ),
	requestHandler_( handler ),
	timer_( IOservice ),
	timeoutDuration_( timeoutDuration )
{
	SSLsocket_ = new ssl_socket( IOservice, SSLcontext );
	LOG_TRACE << "New SSL socket created";
}


void SSLconnection::start()
{
	LOG_TRACE << "Starting SSL handshake, client " << socket().remote_endpoint().address().to_string()
		  << ":" << socket().remote_endpoint().port();
	SSLsocket_->async_handshake( boost::asio::ssl::stream_base::server,
				     strand_.wrap( boost::bind( &connection::handleHandshake,
								shared_from_this(),
								boost::asio::placeholders::error )));
}


void SSLconnection::handleHandshake( const boost::system::error_code& e )
{
	if ( !e )	{
		boost::asio::write( socket(), boost::asio::buffer( "Welcome to SSL bla bla bla\n" ));
		LOG_TRACE << "Wrote welcome message to " << socket().remote_endpoint().address().to_string()
			  << ":" << socket().remote_endpoint().port() << " (SSL)";

		socket().async_read_some( boost::asio::buffer( buffer_ ),
					     strand_.wrap( boost::bind( &connection::handleRead, this,
									boost::asio::placeholders::error,
									boost::asio::placeholders::bytes_transferred )));

		timer_.expires_from_now( boost::posix_time::seconds( timeoutDuration_ ));
		timer_.async_wait( strand_.wrap( boost::bind( &connection::handleTimeout, shared_from_this(),
							      boost::asio::placeholders::error )));
		LOG_TRACE << "Expiring timer for " << socket().remote_endpoint().address().to_string()
			  << ":" << socket().remote_endpoint().port() << " (SSL) set to " << timeoutDuration_ << " s";
	}
	else	{
		LOG_DEBUG << "ERROR handling SSL handshake from " << socket().remote_endpoint().address().to_string()
			  << ":" << socket().remote_endpoint().port();
//		delete this;
		boost::system::error_code ignored_ec;
		socket().shutdown( boost::asio::ip::tcp::socket::shutdown_both, ignored_ec );
	}
}

} // namespace _SMERP
