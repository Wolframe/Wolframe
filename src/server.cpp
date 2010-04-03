//
// server.cpp
//

#include "server.hpp"
#include "logger.hpp"

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>


namespace _SMERP {

server::server( const ApplicationConfiguration& config )
	: threadPoolSize_( config.threads ),
    timeout_duration_((unsigned long)config.idleTimeout ),
	strand_( IOservice_ ),
    requestHandler_()
{
	int	verify = 0;

	if ( config.address.size() > 0 )	{
		acceptor_ = new boost::asio::ip::tcp::acceptor( IOservice_ );
		// Open the acceptor(s) with the option to reuse the address (i.e. SO_REUSEADDR).
		boost::asio::ip::tcp::resolver resolver( IOservice_ );
		boost::asio::ip::tcp::resolver::query query( config.address[0].first, "");
		boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve( query );
		endpoint.port( config.address[0].second );
		newConnection_ = connection_ptr( new connection( IOservice_, requestHandler_, timeout_duration_ ));
		acceptor_->open( endpoint.protocol() );
		acceptor_->set_option( boost::asio::ip::tcp::acceptor::reuse_address( true ));
		acceptor_->bind( endpoint );
		acceptor_->listen();
		acceptor_->async_accept( newConnection_->socket(),
					 strand_.wrap( boost::bind( &server::handleAccept,
								    this,
								    boost::asio::placeholders::error )));
		LOG_INFO << "Accepting connections on " << acceptor_->local_endpoint().address().to_string()
			 << " port " << acceptor_->local_endpoint().port();
	}
	else
		acceptor_ = NULL;

	if ( config.SSLaddress.size() > 0 )	{
		SSLcontext_ = new boost::asio::ssl::context( IOservice_, boost::asio::ssl::context::sslv23 );
		if ( config.SSLverify )
			verify = boost::asio::ssl::context::verify_peer |
					boost::asio::ssl::context::verify_fail_if_no_peer_cert;
		else
			verify = boost::asio::ssl::context::verify_none;
		SSLcontext_->set_options( boost::asio::ssl::context::default_workarounds
					| boost::asio::ssl::context::no_sslv2
					| boost::asio::ssl::context::single_dh_use
					| verify
					);
		SSLcontext_->set_password_callback( boost::bind( &server::getPassword, this ));
		SSLcontext_->use_certificate_chain_file( config.SSLcertificate );
		SSLcontext_->use_private_key_file( config.SSLkey, boost::asio::ssl::context::pem );
		if ( ! config.SSLCAchainFile.empty() )
			SSLcontext_->load_verify_file( config.SSLCAchainFile );
		if ( ! config.SSLCAdirectory.empty() )
			SSLcontext_->add_verify_path( config.SSLCAdirectory );
//		SSLcontext_->use_tmp_dh_file( "dh4096.pem" );
		LOG_DEBUG << "SSL context created";

		newSSLconnection_ = connection_ptr( new connection( IOservice_, requestHandler_, timeout_duration_, SSLcontext_ ));

		SSLacceptor_ = new boost::asio::ip::tcp::acceptor( IOservice_ );
		// Open the acceptor(s) with the option to reuse the address (i.e. SO_REUSEADDR).
		boost::asio::ip::tcp::resolver resolver( IOservice_ );
		boost::asio::ip::tcp::resolver::query query( config.SSLaddress[0].first, "");
		boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve( query );
		endpoint.port( config.SSLaddress[0].second );
		SSLacceptor_->open( endpoint.protocol() );
		SSLacceptor_->set_option( boost::asio::ip::tcp::acceptor::reuse_address( true ));
		SSLacceptor_->bind( endpoint );
		SSLacceptor_->listen();
		SSLacceptor_->async_accept( newSSLconnection_->SSLsocket(),
					    strand_.wrap( boost::bind( &server::handleSSLaccept,
							  this,
							  boost::asio::placeholders::error )));
		LOG_INFO << "Accepting SSL connections on " << SSLacceptor_->local_endpoint().address().to_string()
			 << " port " << SSLacceptor_->local_endpoint().port();
	}
	else
		SSLacceptor_ = NULL;
	LOG_DEBUG << "Server is waiting for connections";
}


server::~server()
{
	if ( acceptor_ != NULL )	{
		delete acceptor_;
		LOG_TRACE << "Acceptor(s) deleted";
	}
	if ( SSLacceptor_ != NULL )	{
		delete SSLacceptor_;
		LOG_TRACE << "SSL acceptor(s) deleted";
	}
	if ( SSLcontext_ != NULL )	{
		delete SSLcontext_;
		LOG_TRACE << "SSL context deleted";
	}
}


void server::run()
{
	// Create a pool of threads to run all of the io_services.
	std::vector<boost::shared_ptr<boost::thread> > threads;
	for ( std::size_t i = 0; i < threadPoolSize_; ++i )	{
		boost::shared_ptr<boost::thread> thread( new boost::thread( boost::bind( &boost::asio::io_service::run, &IOservice_ )));
		threads.push_back( thread );
	}

	// Wait for all threads in the pool to exit.
	for ( std::size_t i = 0; i < threads.size(); ++i )
		threads[i]->join();

	// Reset io_services.
	IOservice_.reset();
}


void server::stop()
{
	LOG_DEBUG << "Network server received a shutdown request";
	// Post a call to the stop function so that server::stop() is safe to call
	// from any thread.
	IOservice_.post( strand_.wrap( boost::bind( &server::handleStop, this )));
}


// Stop io_services the hard way.
void server::abort()
{
	LOG_DEBUG << "Network server received an abort request";
	IOservice_.stop();
}


void server::handleAccept( const boost::system::error_code& e )
{
	if ( !e )	{
		newConnection_->start();
		LOG_INFO << "Accepted connection from " << newConnection_->socket().remote_endpoint().address().to_string()
			 << ":" << newConnection_->socket().remote_endpoint().port();

		newConnection_.reset( new connection( IOservice_, requestHandler_, timeout_duration_ ));
		acceptor_->async_accept( newConnection_->socket(),
					 strand_.wrap( boost::bind( &server::handleAccept,
								    this,
								    boost::asio::placeholders::error )));
		LOG_TRACE << "Acceptor ready for new connection";
	}
}


void server::handleSSLaccept( const boost::system::error_code& e )
{
	if ( !e )	{
		newSSLconnection_->start();
		LOG_INFO << "Accepted SSL connection from " << newSSLconnection_->SSLsocket().remote_endpoint().address().to_string()
			 << ":" << newSSLconnection_->SSLsocket().remote_endpoint().port();

		newSSLconnection_.reset( new connection( IOservice_, requestHandler_, timeout_duration_, SSLcontext_ ));
		SSLacceptor_->async_accept( newSSLconnection_->SSLsocket(),
					    strand_.wrap( boost::bind( &server::handleSSLaccept,
								       this,
								       boost::asio::placeholders::error )));
		LOG_TRACE << "Acceptor ready for new SSL connection";
	}
}


// The server is stopped by closing the acceptor.
// When all outstanding operations are completed
// all calls to io_service::run() will return.
void server::handleStop()
{
	if ( acceptor_ != NULL )	{
		LOG_TRACE << "Closing acceptor for unencrypted connections";
		acceptor_->close();
	}
	if ( SSLacceptor_ != NULL )	{
		LOG_TRACE << "Closing acceptor for SSL connections";
		SSLacceptor_->close();
	}
}


} // namespace _SMERP
