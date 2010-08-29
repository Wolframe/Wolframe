//
// acceptor.cpp
//

#include "acceptor.hpp"
#include "connection.hpp"
#include "connectionTimeout.hpp"
#include "logger.hpp"

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>


namespace _SMERP {

acceptor::acceptor( boost::asio::io_service& IOservice,
		    const std::string& host, const unsigned short port,
		    connectionTimeout& timeouts, requestHandler& reqHandler) :
	IOservice_( IOservice ),
	strand_( IOservice_ ),
	acceptor_( IOservice_ ),
	timeouts_( timeouts ),
	requestHandler_( reqHandler )
{
	// Open the acceptor(s) with the option to reuse the address (i.e. SO_REUSEADDR).
	boost::asio::ip::tcp::resolver resolver( IOservice_ );
	boost::asio::ip::tcp::resolver::query query( host, "");
	boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve( query );
	endpoint.port( port );

	newConnection_ = connection_ptr( new connection( IOservice_, timeouts_, requestHandler_ ));

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

SSLacceptor::SSLacceptor( boost::asio::io_service& IOservice,
			  const std::string& certFile, const std::string& keyFile,
			  bool verify, const std::string& CAchainFile, const std::string& CAdirectory,
			  const std::string& host, const unsigned short port,
			  connectionTimeout& timeouts, requestHandler& reqHandler) :
	IOservice_( IOservice ),
	strand_( IOservice_ ),
	acceptor_( IOservice_ ),
	SSLcontext_( IOservice_, boost::asio::ssl::context::sslv23 ),
	timeouts_( timeouts ),
	requestHandler_( reqHandler )
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
		LOG_FATAL << "Empty SSL key filename";
		exit( 1 );
	}
	else if ( SSLcontext_.use_private_key_file( keyFile, boost::asio::ssl::context::pem, ec ) != 0 )	{
		LOG_FATAL << ec.message() << " loading SSL key file: " << keyFile;
		exit( 1 );
	}
	//		SSLcontext_->use_tmp_dh_file( "dh4096.pem" );
	if ( verify )	{
		if ( ! CAchainFile.empty() )
			if ( SSLcontext_.load_verify_file( CAchainFile, ec ) != 0 )	{
			LOG_FATAL << ec.message() << " loading SSL CA chain file: " << CAchainFile;
			exit( 1 );
		}
		if ( ! CAdirectory.empty() )
			if ( SSLcontext_.add_verify_path( CAdirectory, ec ) != 0 )	{
			LOG_FATAL << ec.message() << " setting CA directory: " << CAdirectory;
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
	boost::asio::ip::tcp::resolver::query query( host, "");
	boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve( query );
	endpoint.port( port );

	newConnection_ = SSLconnection_ptr( new SSLconnection( IOservice_, SSLcontext_, timeouts_, requestHandler_ ));

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


/// acceptor destructor
acceptor::~acceptor()
{
	LOG_TRACE << "Acceptor destructor called for " << identifier_;
}


/// acceptor destructor (SSL acceptor)
SSLacceptor::~SSLacceptor()
{
	LOG_TRACE << "Acceptor destructor called for " << identifier_;
}


void acceptor::handleAccept( const boost::system::error_code& e )
{
	if ( !e )	{
		newConnection_->start();
		LOG_DEBUG << "Received new connection on " << identifier_;

		newConnection_.reset( new connection( IOservice_, timeouts_, requestHandler_ ));
		acceptor_.async_accept( newConnection_->socket(),
					strand_.wrap( boost::bind( &acceptor::handleAccept,
								   this,
								   boost::asio::placeholders::error )));
		LOG_DATA << "Acceptor " << identifier_ << " ready for new connection";
	}
}

void SSLacceptor::handleAccept( const boost::system::error_code& e )
{
	if ( !e )	{
		newConnection_->start();
		LOG_DEBUG << "Received new connection on " << identifier_;

		newConnection_.reset( new SSLconnection( IOservice_, SSLcontext_, timeouts_, requestHandler_ ));
		acceptor_.async_accept( newConnection_->socket().lowest_layer(),
					strand_.wrap( boost::bind( &SSLacceptor::handleAccept,
								   this,
								   boost::asio::placeholders::error )));
		LOG_DATA << "Acceptor " << identifier_ << " ready for new connection";
	}
}


// Post a call to the stop function so that acceptor::stop() is safe to call
// from any thread.
void acceptor::stop()
{
	LOG_TRACE << "Acceptor for " << identifier_ << " received a shutdown request";
	IOservice_.post( strand_.wrap( boost::bind( &acceptor::handleStop, this )));
}

void SSLacceptor::stop()
{
	LOG_TRACE << "Acceptor for " << identifier_ << " received a shutdown request";
	IOservice_.post( strand_.wrap( boost::bind( &SSLacceptor::handleStop, this )));
}


// The server is stopped by closing the acceptor.
// When all outstanding operations are completed
// all calls to io_service::run() will return.
void acceptor::handleStop()
{
	acceptor_.close();
	LOG_DEBUG << "Closed acceptor for " << identifier_;
}

void SSLacceptor::handleStop()
{
	acceptor_.close();
	LOG_DEBUG << "Closed acceptor for " << identifier_;
}

} // namespace _SMERP
