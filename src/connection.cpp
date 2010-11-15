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
	namespace Network {

connection::connection( boost::asio::io_service& IOservice,
			ConnectionList<connection_ptr>& connList,
			connectionHandler* handler ) :
	connectionBase< boost::asio::ip::tcp::socket >( IOservice, handler ),
	socket_( IOservice ),
	connList_( connList )
{
	LOG_TRACE << "New connection created";
}

connection::~connection()
{
	if ( identifier_.empty() )
		LOG_TRACE << "Connection (unconnected) destroyed";
	else
		LOG_TRACE << "Connection to " << identifier_ <<" destroyed";
}

void connection::start()
{
	identifier( std::string( socket().remote_endpoint().address().to_string())
		    + ":" + boost::lexical_cast<std::string>( socket().remote_endpoint().port() ));
	LOG_TRACE << "Starting connection to " << identifier();
	connList_.push( boost::static_pointer_cast<connection>( shared_from_this()));

	connectionHandler_->setPeer( RemoteTCPendpoint( socket().remote_endpoint().address().to_string(),
							socket().remote_endpoint().port()));
	nextOperation();
}

void connection::refuse( const std::string& reason )
{
	identifier( std::string( socket().remote_endpoint().address().to_string())
		    + ":" + boost::lexical_cast<std::string>( socket().remote_endpoint().port() ));
	LOG_TRACE << "Refusing connection to " << identifier() << ": " << reason;

	boost::system::error_code ignored_ec;
	socket().write_some( boost::asio::buffer( reason ), ignored_ec );
	socket().shutdown( boost::asio::ip::tcp::socket::shutdown_both, ignored_ec );
}

void connection::unregister()
{
	connList_.remove( boost::static_pointer_cast<connection>( shared_from_this()) );
}


#ifdef WITH_SSL

SSLconnection::SSLconnection( boost::asio::io_service& IOservice,
			      boost::asio::ssl::context& SSLcontext,
			      ConnectionList<SSLconnection_ptr>& connList,
			      connectionHandler *handler ) :
	connectionBase< ssl_socket >( IOservice, handler ),
	SSLsocket_( IOservice, SSLcontext ),
	connList_( connList )
{
	LOG_TRACE << "New SSL connection created";
}

SSLconnection::~SSLconnection()
{
	if ( identifier_.empty() )
		LOG_TRACE << "SSL connection (unconnected) destroyed";
	else
		LOG_TRACE << "Connection to " << identifier_ <<" destroyed";
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
	if ( !e )	{
		LOG_DATA << "successful SSL handshake, peer " << identifier();
		// Extract the common name from the client cert

		X509*	peerCert;
		char	buf[2048];
		int	res = -1;

		SSL*	ssl = SSLsocket_.impl()->ssl;

		memset( buf, 0, 2048 );
		peerCert = SSL_get_peer_certificate( ssl );
		if ( peerCert )
			res = X509_NAME_get_text_by_NID( X509_get_subject_name( peerCert ),
							 NID_commonName, buf, 2047 );
		connList_.push( boost::static_pointer_cast<SSLconnection>( shared_from_this()) );

		if ( res != -1 )
			connectionHandler_->setPeer( RemoteSSLendpoint( SSLsocket_.lowest_layer().remote_endpoint().address().to_string(),
									SSLsocket_.lowest_layer().remote_endpoint().port(),
									buf ));
		else
			connectionHandler_->setPeer( RemoteSSLendpoint( SSLsocket_.lowest_layer().remote_endpoint().address().to_string(),
									SSLsocket_.lowest_layer().remote_endpoint().port()));
		nextOperation();
	}
	else	{
		LOG_DEBUG << e.message() << ", SSL handshake, peer " << identifier();
//		delete this;
		boost::system::error_code ignored_ec;
		socket().lowest_layer().shutdown( boost::asio::ip::tcp::socket::shutdown_both, ignored_ec );
	}
}

void SSLconnection::unregister()
{
	connList_.remove( boost::static_pointer_cast<SSLconnection>( shared_from_this()) );
}

#endif // WITH_SSL

} // namespace Network
} // namespace _SMERP
