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
	boost::system::error_code	ec;

	for ( size_t i = 0; i < config.address.size(); i++ )	{
		boost::asio::ip::tcp::acceptor* acptr = new boost::asio::ip::tcp::acceptor( IOservice_ );

		// Open the acceptor(s) with the option to reuse the address (i.e. SO_REUSEADDR).
		boost::asio::ip::tcp::resolver resolver( IOservice_ );
		boost::asio::ip::tcp::resolver::query query( config.address[i].host, "");
		boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve( query );
		endpoint.port( config.address[i].port );

		newConnection_ = connection_ptr( new connection( IOservice_, requestHandler_, timeout_duration_ ));

		acptr->open( endpoint.protocol() );
		acptr->set_option( boost::asio::ip::tcp::acceptor::reuse_address( true ));
		acptr->bind( endpoint );
		acptr->listen();
		acptr->async_accept( newConnection_->socket(),
					 strand_.wrap( boost::bind( &server::handleAccept,
								    this,
								    boost::asio::placeholders::error )));
		LOG_INFO << "Accepting connections on " << acptr->local_endpoint().address().to_string()
			 << " port " << acptr->local_endpoint().port();
		acceptor_.push_back( acptr );
	}

	for ( size_t i = 0; i < config.SSLaddress.size(); i++ )	{
		boost::asio::ssl::context* cntxt = new boost::asio::ssl::context( IOservice_, boost::asio::ssl::context::sslv23 );
		cntxt->set_options( boost::asio::ssl::context::default_workarounds
					  | boost::asio::ssl::context::no_sslv2
					  | boost::asio::ssl::context::single_dh_use
					  );
		cntxt->set_password_callback( boost::bind( &server::getPassword, this ));
		if ( config.SSLaddress[i].certFile.empty() )	{
			LOG_FATAL << "Empty SSL certificate filename";
			exit( 1 );
		}
		else if ( cntxt->use_certificate_chain_file( config.SSLaddress[i].certFile, ec ) != 0 )	{
			LOG_FATAL << ec.message() << " loading SSL certificate file: " << config.SSLaddress[i].certFile;
			exit( 1 );
		}

		if ( config.SSLaddress[i].keyFile.empty() )	{
			LOG_FATAL << "Empty SSL key filename";
			exit( 1 );
		}
		else if ( cntxt->use_private_key_file( config.SSLaddress[i].keyFile, boost::asio::ssl::context::pem, ec ) != 0 )	{
			LOG_FATAL << ec.message() << " loading SSL key file: " << config.SSLaddress[i].keyFile;
			exit( 1 );
		}
		//		SSLcontext_->use_tmp_dh_file( "dh4096.pem" );
		if ( config.SSLaddress[i].verify )	{
			if ( ! config.SSLaddress[i].CAchainFile.empty() )
				if ( cntxt->load_verify_file( config.SSLaddress[i].CAchainFile, ec ) != 0 )	{
					LOG_FATAL << ec.message() << " loading SSL CA chain file: " << config.SSLaddress[i].CAchainFile;
					exit( 1 );
				}
			if ( ! config.SSLaddress[i].CAdirectory.empty() )
				if ( cntxt->add_verify_path( config.SSLaddress[i].CAdirectory, ec ) != 0 )	{
					LOG_FATAL << ec.message() << " setting CA directory: " << config.SSLaddress[i].CAdirectory;
					exit( 1 );
				}

			cntxt->set_verify_mode( boost::asio::ssl::context::verify_peer |
						      boost::asio::ssl::context::verify_fail_if_no_peer_cert );
			LOG_DEBUG << "SSL client certificate verification set to VERIFY";
		}
		else	{
			cntxt->set_verify_mode( boost::asio::ssl::context::verify_none );
			LOG_DEBUG << "SSL client certificate verification set to NONE";
		}
		LOG_DEBUG << "SSL context created";

		newSSLconnection_ = connection_ptr( new connection( IOservice_, requestHandler_, timeout_duration_, cntxt ));

		boost::asio::ip::tcp::acceptor* acptr = new boost::asio::ip::tcp::acceptor( IOservice_ );
		// Open the acceptor(s) with the option to reuse the address (i.e. SO_REUSEADDR).
		boost::asio::ip::tcp::resolver resolver( IOservice_ );
		boost::asio::ip::tcp::resolver::query query( config.SSLaddress[i].host, "");
		boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve( query );
		endpoint.port( config.SSLaddress[i].port );
		acptr->open( endpoint.protocol() );
		acptr->set_option( boost::asio::ip::tcp::acceptor::reuse_address( true ));
		acptr->bind( endpoint );
		acptr->listen();
		acptr->async_accept( newSSLconnection_->SSLsocket(),
				    strand_.wrap( boost::bind( &server::handleSSLaccept,
							       this,
							       boost::asio::placeholders::error )));
		LOG_INFO << "Accepting SSL connections on " << acptr->local_endpoint().address().to_string()
			<< " port " << acptr->local_endpoint().port();

		SSLcontext_.push_back( cntxt );
		SSLacceptor_.push_back( acptr );
	}
	LOG_DEBUG << "Server is waiting for connections";
}


server::~server()
{
	size_t	i;

	if ( SSLacceptor_.size() > 0 )	{
		for ( i = 0; i < acceptor_.size(); i++ )
			delete acceptor_[i];
		LOG_TRACE << i << " acceptor(s) deleted";
	}

	if ( SSLacceptor_.size() > 0 )	{
		for ( i = 0; i < SSLacceptor_.size(); i++ )
			delete SSLacceptor_[i];
		LOG_TRACE << i << " SSL acceptor(s) deleted";
	}

	if ( SSLcontext_.size() > 0 )	{
		for ( i = 0; i < SSLcontext_.size(); i++ )
			delete SSLcontext_[i];
		LOG_TRACE << i << "SSL context(s) deleted";
	}
}


void server::run()
{
	// Create a pool of threads to run all of the io_services.
	std::vector<boost::shared_ptr<boost::thread> >	threads;
	std::size_t					i;

	for ( i = 0; i < threadPoolSize_; ++i )	{
		boost::shared_ptr<boost::thread> thread( new boost::thread( boost::bind( &boost::asio::io_service::run, &IOservice_ )));
		threads.push_back( thread );
	}
	LOG_TRACE << i << " network server threads started";

	// Wait for all threads in the pool to exit.
	for ( i = 0; i < threads.size(); ++i )
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
		acceptor_[0]->async_accept( newConnection_->socket(),
					 strand_.wrap( boost::bind( &server::handleAccept,
								    this,
								    boost::asio::placeholders::error )));
		LOG_TRACE << "Acceptor " << 0 << " ready for new connection";
	}
}


void server::handleSSLaccept( const boost::system::error_code& e )
{
	if ( !e )	{
		newSSLconnection_->start();
		LOG_INFO << "Accepted SSL connection from " << newSSLconnection_->SSLsocket().remote_endpoint().address().to_string()
			 << ":" << newSSLconnection_->SSLsocket().remote_endpoint().port();

		newSSLconnection_.reset( new connection( IOservice_, requestHandler_, timeout_duration_, SSLcontext_[0] ));
		SSLacceptor_[0]->async_accept( newSSLconnection_->SSLsocket(),
					    strand_.wrap( boost::bind( &server::handleSSLaccept,
								       this,
								       boost::asio::placeholders::error )));
		LOG_TRACE << "Acceptor " << 0 << " ready for new SSL connection";
	}
}


// The server is stopped by closing the acceptor.
// When all outstanding operations are completed
// all calls to io_service::run() will return.
void server::handleStop()
{
	size_t	i;

	for ( i = 0; i < acceptor_.size(); i++ )
		acceptor_[i]->close();
	LOG_TRACE << "Closed " << i << " acceptor(s) for unencrypted connections";

	for ( i = 0; i < SSLacceptor_.size(); i++ )
		SSLacceptor_[i]->close();
	LOG_TRACE << "Closed " << i << " acceptor(s) for SSL connections";
}


} // namespace _SMERP
