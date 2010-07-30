//
// server.cpp
//

#include "SSLserver.hpp"
#include "logger.hpp"
#include "SSLconnection.hpp"

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

namespace _SMERP {

SSLserver::SSLserver( const ApplicationConfiguration& config )
	: SSLcontext_( IOservice_, boost::asio::ssl::context::sslv23 )
{
	boost::system::error_code	ec;

	SSLcontext_.set_options( boost::asio::ssl::context::default_workarounds
			    | boost::asio::ssl::context::no_sslv2
			    | boost::asio::ssl::context::single_dh_use
			    );
	SSLcontext_.set_password_callback( boost::bind( &SSLserver::getPassword, this ));
	if ( config.SSLaddress[i].certFile.empty() )	{
		LOG_FATAL << "Empty SSL certificate filename";
		exit( 1 );
	}
	else if ( SSLcontext_.use_certificate_chain_file( config.SSLaddress[i].certFile, ec ) != 0 )	{
		LOG_FATAL << ec.message() << " loading SSL certificate file: " << config.SSLaddress[i].certFile;
		exit( 1 );
	}

	if ( config.SSLaddress[i].keyFile.empty() )	{
		LOG_FATAL << "Empty SSL key filename";
		exit( 1 );
	}
	else if ( SSLcontext_.use_private_key_file( config.SSLaddress[i].keyFile, boost::asio::ssl::context::pem, ec ) != 0 )	{
		LOG_FATAL << ec.message() << " loading SSL key file: " << config.SSLaddress[i].keyFile;
		exit( 1 );
	}
	//		SSLcontext_->use_tmp_dh_file( "dh4096.pem" );
	if ( config.SSLaddress[i].verify )	{
		if ( ! config.SSLaddress[i].CAchainFile.empty() )
			if ( SSLcontext_.load_verify_file( config.SSLaddress[i].CAchainFile, ec ) != 0 )	{
			LOG_FATAL << ec.message() << " loading SSL CA chain file: " << config.SSLaddress[i].CAchainFile;
			exit( 1 );
		}
		if ( ! config.SSLaddress[i].CAdirectory.empty() )
			if ( SSLcontext_.add_verify_path( config.SSLaddress[i].CAdirectory, ec ) != 0 )	{
			LOG_FATAL << ec.message() << " setting CA directory: " << config.SSLaddress[i].CAdirectory;
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
	LOG_DEBUG << "SSL context created";

	newConnection_ = connection_ptr( new SSLconnection( IOservice_, requestHandler_, timeout_duration_, cntxt ));

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

	LOG_DEBUG << "SSL server is waiting for connections";
}


} // namespace _SMERP
