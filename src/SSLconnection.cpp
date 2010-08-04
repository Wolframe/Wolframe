//
// SSLconnection.cpp
//

#include "connectionBase.hpp"
#include "SSLconnection.hpp"
#include "logger.hpp"
#include "requestHandler.hpp"
#include "connContext.hpp"

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>


namespace _SMERP {

SSLconnection::SSLconnection( boost::asio::io_service& IOservice,
			      boost::asio::ssl::context& SSLcontext,
			      connectionTimeout& timeouts,
			      requestHandler& handler ) :
	connectionBase< ssl_socket >( IOservice, timeouts, handler ),
	SSLsocket_( IOservice, SSLcontext )
{
	LOG_TRACE << "New SSL connection created";
}


void SSLconnection::start()
{
	identifier( std::string( SSLsocket_.lowest_layer().remote_endpoint().address().to_string())
		    + ":" + boost::lexical_cast<std::string>( SSLsocket_.lowest_layer().remote_endpoint().port() )
		    + " (SSL)");
	LOG_TRACE << "Starting connection to " << identifier();
	SSLsocket_.async_handshake( boost::asio::ssl::stream_base::server,
				    strand_.wrap( boost::bind( &SSLconnection::handleHandshake,
							       boost::static_pointer_cast<SSLconnection>( shared_from_this() ),
							       boost::asio::placeholders::error )));
}


void SSLconnection::handleHandshake( const boost::system::error_code& e )
{
	LOG_DATA << "SSL handshake to " << identifier();
	if ( !e )	{
		boost::asio::write( socket(), boost::asio::buffer( "Welcome to SSL bla bla bla\n" ));
		LOG_TRACE << "Wrote welcome message to " << identifier();

		socket().async_read_some( boost::asio::buffer( buffer_ ),
					    strand_.wrap( boost::bind( &SSLconnection::handleRead,
								       boost::static_pointer_cast<SSLconnection>( shared_from_this() ),
								       boost::asio::placeholders::error,
								       boost::asio::placeholders::bytes_transferred )));
		setTimeout( connectionTimeout::TIMEOUT_IDLE );
	}
	else	{
		LOG_DEBUG << "ERROR handling SSL handshake from " << identifier();
//		delete this;
		boost::system::error_code ignored_ec;
		socket().lowest_layer().shutdown( boost::asio::ip::tcp::socket::shutdown_both, ignored_ec );
	}
}

} // namespace _SMERP
