/************************************************************************

 Copyright (C) 2011 Project Wolframe.
 All rights reserved.

 This file is part of Project Wolframe.

 Commercial Usage
    Licensees holding valid Project Wolframe Commercial licenses may
    use this file in accordance with the Project Wolframe
    Commercial License Agreement provided with the Software or,
    alternatively, in accordance with the terms contained
    in a written agreement between the licensee and Project Wolframe.

 GNU General Public License Usage
    Alternatively, you can redistribute this file and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Wolframe is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
//
// acceptor.cpp
//

#include "acceptor.hpp"
#include "connection.hpp"
#include "logger.hpp"
#include "getPassword.hpp"

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>


namespace _Wolframe {
namespace net {

acceptor::acceptor( boost::asio::io_service& IOservice,
		    const std::string& host, unsigned short port, unsigned maxConnections,
		    GlobalConnectionList& globalList,
		    _Wolframe::ServerHandler& srvHandler ) :
	IOservice_( IOservice ),
	strand_( IOservice_ ),
	acceptor_( IOservice_ ),
	connList_ ( maxConnections, globalList ),
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
			  GlobalConnectionList& globalList,
			  _Wolframe::ServerHandler& srvHandler) :
	IOservice_( IOservice ),
	strand_( IOservice_ ),
	acceptor_( IOservice_ ),
	SSLcontext_( IOservice_, boost::asio::ssl::context::sslv23 ),
	connList_ ( maxConnections, globalList ),
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

	if ( verify )	{
		if ( CAchainFile.empty() && CAdirectory.empty() )	{
			LOG_FATAL << "Either a CA directory or a CA chain file is required";
			exit( 1 );
		}
		SSLcontext_.set_verify_mode( boost::asio::ssl::context::verify_peer |
					     boost::asio::ssl::context::verify_fail_if_no_peer_cert );
		LOG_DEBUG << "SSL client certificate verification set to VERIFY";
	}
	else	{
//		SSLcontext_.set_verify_mode( boost::asio::ssl::context::verify_none );
		SSLcontext_.set_verify_mode( boost::asio::ssl::context::verify_peer );
		LOG_DEBUG << "SSL client certificate verification set to NONE";
	}

	// Open the acceptor(s) with the option to reuse the address (i.e. SO_REUSEADDR).
	boost::asio::ip::tcp::resolver resolver( IOservice_ );
	boost::asio::ip::tcp::resolver::query query( host, "" );
	boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve( query );
	endpoint.port( port );

	connectionHandler *handler = srvHandler_.newConnection( LocalSSLendpoint( host, port ));
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

		connectionHandler *handler = srvHandler_.newConnection( LocalSSLendpoint( acceptor_.local_endpoint().address().to_string(),
											     acceptor_.local_endpoint().port() ));
		newConnection_.reset( new SSLconnection( IOservice_, SSLcontext_, connList_, handler ));
		acceptor_.async_accept( newConnection_->socket().lowest_layer(),
					strand_.wrap( boost::bind( &SSLacceptor::handleAccept,
								   this,
								   boost::asio::placeholders::error )));
		LOG_DATA << "Acceptor on " << identifier_ << " ready for new connection";
	}
}

std::string SSLacceptor::getPassword()
{
	return _Wolframe::AAAA::getPassword();
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

}} // namespace _Wolframe::net
