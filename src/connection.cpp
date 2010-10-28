//
// connection.cpp
//

#include "connectionBase.hpp"
#include "connection.hpp"
#include "logger.hpp"
#include "connectionHandler.hpp"

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

namespace _SMERP {

connection::connection( boost::asio::io_service& IOservice,
			connectionHandler* handler ) :
	connectionBase< boost::asio::ip::tcp::socket >( IOservice, handler ),
	socket_( IOservice )
{
	LOG_TRACE << "New connection created";
}

connection::~connection()
{
	LOG_TRACE << "Connection destroyed";
}


void connection::start()
{
	identifier( std::string( socket().remote_endpoint().address().to_string())
		    + ":" + boost::lexical_cast<std::string>( socket().remote_endpoint().port() ));
	LOG_TRACE << "Starting connection to " << identifier();

	connectionHandler_->setPeer( RemoteTCPendpoint( socket().remote_endpoint().address().to_string(),
							socket().remote_endpoint().port()));

	nextOperation();
}


#ifdef WITH_SSL

SSLconnection::SSLconnection( boost::asio::io_service& IOservice,
			      boost::asio::ssl::context& SSLcontext,
			      connectionHandler *handler ) :
	connectionBase< ssl_socket >( IOservice, handler ),
	SSLsocket_( IOservice, SSLcontext )
{
	LOG_TRACE << "New SSL connection created";
}

SSLconnection::~SSLconnection()
{
	LOG_TRACE << "SSL connection destroyed";
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
		// Extract the common name from the client cert
		SSL* ssl = SSLsocket_.impl()->ssl;
		X509* peer_cert;
		char buf[2048];
		memset( buf, 0, 2048 );
		peer_cert = SSL_get_peer_certificate( ssl );
		int x = X509_NAME_get_text_by_NID( X509_get_subject_name( peer_cert ),
						   NID_commonName, buf, 2047 );
		if ( x != -1 )
			connectionHandler_->setPeer( RemoteSSLendpoint( SSLsocket_.lowest_layer().remote_endpoint().address().to_string(),
									SSLsocket_.lowest_layer().remote_endpoint().port(),
									buf ));
		else
			connectionHandler_->setPeer( RemoteSSLendpoint( SSLsocket_.lowest_layer().remote_endpoint().address().to_string(),
									SSLsocket_.lowest_layer().remote_endpoint().port()));
		nextOperation();
	}
	else	{
		LOG_DEBUG << "ERROR handling SSL handshake from " << identifier();
//		delete this;
		boost::system::error_code ignored_ec;
		socket().lowest_layer().shutdown( boost::asio::ip::tcp::socket::shutdown_both, ignored_ec );
	}
}

#endif // WITH_SSL

} // namespace _SMERP
