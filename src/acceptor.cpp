//
// acceptor.cpp
//

#include "acceptor.hpp"
#include "connection.hpp"
#include "logger.hpp"

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>


namespace _SMERP {
namespace Network {

acceptor::acceptor( boost::asio::io_service& IOservice,
		    const std::string& host, unsigned short port, unsigned maxConnections,
		    ServerHandler& srvHandler ) :
	IOservice_( IOservice ),
	strand_( IOservice_ ),
	acceptor_( IOservice_ ),
	connList_ ( maxConnections ),
	srvHandler_( srvHandler )
{
	// Open the acceptor(s) with the option to reuse the address (i.e. SO_REUSEADDR).
	boost::asio::ip::tcp::resolver resolver( IOservice_ );
	boost::asio::ip::tcp::resolver::query query( host, "" );
	boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve( query );
	endpoint.port( port );

	connectionHandler *handler = srvHandler_.newConnection( LocalTCPendpoint( host, port ));
	newConnection_ = connection_ptr( new connection( IOservice_, connList_, handler ));

	acceptor_.open( endpoint.protocol() );
	acceptor_.set_option( boost::asio::ip::tcp::acceptor::reuse_address( true ));
	acceptor_.bind( endpoint );
	acceptor_.listen();
	acceptor_.async_accept( newConnection_->socket(),
				strand_.wrap( boost::bind( &acceptor::handleAccept,
							   this,
							   boost::asio::placeholders::error )));
	identifier_ = acceptor_.local_endpoint().address().to_string()
		      + ":" + boost::lexical_cast<std::string>( acceptor_.local_endpoint().port() );
	LOG_INFO << "Accepting connections on " << identifier_;
}

/// acceptor destructor
acceptor::~acceptor()
{
	LOG_TRACE << "Acceptor destructor called for " << identifier_;
}

void acceptor::handleAccept( const boost::system::error_code& e )
{
	if ( !e )	{
		LOG_DEBUG << "Received new connection on " << identifier_;
		newConnection_->start();

		connectionHandler *handler = srvHandler_.newConnection( LocalTCPendpoint( acceptor_.local_endpoint().address().to_string(),
											  acceptor_.local_endpoint().port() ));
		newConnection_.reset( new connection( IOservice_, connList_, handler ));
		acceptor_.async_accept( newConnection_->socket(),
					strand_.wrap( boost::bind( &acceptor::handleAccept,
								   this,
								   boost::asio::placeholders::error )));
		LOG_DATA << "Acceptor on " << identifier_ << " ready for new connection";
	}
}


// Post a call to the stop function so that acceptor::stop() is safe to call
// from any thread.
void acceptor::stop()
{
	LOG_TRACE << "Acceptor for " << identifier_ << " received a shutdown request";
	IOservice_.post( strand_.wrap( boost::bind( &acceptor::handleStop, this )));
}

// The server is stopped by closing the acceptor.
// When all outstanding operations are completed
// all calls to io_service::run() will return.
void acceptor::handleStop()
{
	acceptor_.close();
	LOG_DEBUG << "Closed acceptor for " << identifier_;
	// at this point no more connections are accepted
	// Signal the list of processors to terminate
	connection_ptr conn;
	while (( conn = connList_.pop()) != NULL )
		conn->signal();
}


#ifdef WITH_SSL

SSLacceptor::SSLacceptor( boost::asio::io_service& IOservice,
			  const std::string& certFile, const std::string& keyFile,
			  bool verify, const std::string& CAchainFile, const std::string& CAdirectory,
			  const std::string& host, unsigned short port, unsigned maxConnections,
			  ServerHandler& srvHandler) :
	IOservice_( IOservice ),
	strand_( IOservice_ ),
	acceptor_( IOservice_ ),
	SSLcontext_( IOservice_, boost::asio::ssl::context::sslv23 ),
	connList_ ( maxConnections ),
	srvHandler_( srvHandler )
{
	boost::system::error_code	ec;

	SSLcontext_.set_options( boost::asio::ssl::context::default_workarounds
			    | boost::asio::ssl::context::no_sslv2
			    | boost::asio::ssl::context::single_dh_use
			    );
	SSLcontext_.set_password_callback( boost::bind( &SSLacceptor::getPassword, this ));
	if ( certFile.empty() )	{
		LOG_FATAL << "Empty SSL certificate filename";
		exit( 1 );
	}
	else if ( SSLcontext_.use_certificate_chain_file( certFile, ec ) != 0 )	{
		LOG_FATAL << ec.message() << " loading SSL certificate file: " << certFile;
		exit( 1 );
	}

	if ( keyFile.empty() )	{
		LOG_FATAL << "SSL key filename is required";
		exit( 1 );
	}
	else if ( SSLcontext_.use_private_key_file( keyFile, boost::asio::ssl::context::pem, ec ) != 0 )	{
		LOG_FATAL << ec.message() << " loading SSL key file: " << keyFile;
		exit( 1 );
	}
//		SSLcontext_->use_tmp_dh_file( "dh4096.pem" );
	if ( verify )	{
		if ( ! CAchainFile.empty() )	{
			if ( SSLcontext_.load_verify_file( CAchainFile, ec ) != 0 )	{
				LOG_FATAL << ec.message() << " loading SSL CA chain file: " << CAchainFile;
				exit( 1 );
			}
		}
		if ( ! CAdirectory.empty() )	{
			if ( SSLcontext_.add_verify_path( CAdirectory, ec ) != 0 )	{
				LOG_FATAL << ec.message() << " setting CA directory: " << CAdirectory;
				exit( 1 );
			}
		}
		if ( CAchainFile.empty() && CAdirectory.empty() )	{
			LOG_FATAL << "Either a CA directory or a CA chain file is required";
			exit( 1 );
		}
		SSLcontext_.set_verify_mode( boost::asio::ssl::context::verify_peer |
					     boost::asio::ssl::context::verify_fail_if_no_peer_cert );
		LOG_DEBUG << "SSL client certificate verification set to VERIFY";
	}
	else	{
		SSLcontext_.set_verify_mode( boost::asio::ssl::context::verify_none );
		LOG_DEBUG << "SSL client certificate verification set to NONE";
	}

	// Open the acceptor(s) with the option to reuse the address (i.e. SO_REUSEADDR).
	boost::asio::ip::tcp::resolver resolver( IOservice_ );
	boost::asio::ip::tcp::resolver::query query( host, "" );
	boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve( query );
	endpoint.port( port );

	connectionHandler *handler = srvHandler_.newSSLconnection( LocalSSLendpoint( host, port ));
	newConnection_ = SSLconnection_ptr( new SSLconnection( IOservice_, SSLcontext_, connList_, handler ));

	acceptor_.open( endpoint.protocol() );
	acceptor_.set_option( boost::asio::ip::tcp::acceptor::reuse_address( true ));
	acceptor_.bind( endpoint );
	acceptor_.listen();
	acceptor_.async_accept( newConnection_->socket().lowest_layer(),
				strand_.wrap( boost::bind( &SSLacceptor::handleAccept,
							   this,
							   boost::asio::placeholders::error )));
	identifier_ = acceptor_.local_endpoint().address().to_string()
		      + ":" + boost::lexical_cast<std::string>( acceptor_.local_endpoint().port() ) + " (SSL)";
	LOG_INFO << "Accepting connections on " << identifier_;
}

/// acceptor destructor (SSL acceptor)
SSLacceptor::~SSLacceptor()
{
	LOG_TRACE << "Acceptor destructor called for " << identifier_;
}

void SSLacceptor::handleAccept( const boost::system::error_code& e )
{
	if ( !e )	{
		LOG_DEBUG << "Received new connection on " << identifier_;
		newConnection_->start();

		connectionHandler *handler = srvHandler_.newSSLconnection( LocalSSLendpoint( acceptor_.local_endpoint().address().to_string(),
											     acceptor_.local_endpoint().port() ));
		newConnection_.reset( new SSLconnection( IOservice_, SSLcontext_, connList_, handler ));
		acceptor_.async_accept( newConnection_->socket().lowest_layer(),
					strand_.wrap( boost::bind( &SSLacceptor::handleAccept,
								   this,
								   boost::asio::placeholders::error )));
		LOG_DATA << "Acceptor on " << identifier_ << " ready for new connection";
	}
}

void SSLacceptor::stop()
{
	LOG_TRACE << "Acceptor for " << identifier_ << " received a shutdown request";
	IOservice_.post( strand_.wrap( boost::bind( &SSLacceptor::handleStop, this )));
}

void SSLacceptor::handleStop()
{
	acceptor_.close();
	LOG_DEBUG << "Closed acceptor for " << identifier_;
	// at this point no more connections are accepted
	// Signal the list of processors to terminate
	SSLconnection_ptr conn;
	while (( conn = connList_.pop()) != NULL )
		conn->signal();
}

#endif // WITH_SSL

} // namespace Network
} // namespace _SMERP
