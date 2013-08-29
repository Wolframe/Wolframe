/************************************************************************

 Copyright (C) 2011 - 2013 Project Wolframe.
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
#include "system/errorCode.hpp"
#include "logger-v1.hpp"
#include "utils/getPassword.hpp"

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
	m_IOservice( IOservice ),
	m_strand( m_IOservice ),
	m_acceptor( m_IOservice ),
	m_connList ( maxConnections, globalList ),
	m_srvHandler( srvHandler )
{
	// Open the acceptor(s) with the option to reuse the address (i.e. SO_REUSEADDR).
	boost::asio::ip::tcp::endpoint endpoint;
	try	{
		boost::asio::ip::tcp::resolver resolver( m_IOservice );
		boost::asio::ip::tcp::resolver::query query( host, "7661" );
		endpoint = *resolver.resolve( query );
	}
	catch ( std::exception& e )	{
		LOG_FATAL << "Unable to resolve host '" << host << "' (" << e.what() << ")";
		exit( ErrorCode::FAILURE );
	}
	endpoint.port( port );

	ConnectionHandler *handler = m_srvHandler.newConnection( LocalTCPendpoint( host, port ));
	m_newConnection = connection_ptr( new connection( m_IOservice, &m_connList, handler ));

	try	{
		m_acceptor.open( endpoint.protocol() );
		m_acceptor.set_option( boost::asio::ip::tcp::acceptor::reuse_address( true ));
		m_acceptor.bind( endpoint );
		m_acceptor.listen();
	}
	catch ( std::exception& e )	{
		LOG_FATAL << "Unable to listen on " << endpoint << " (" << e.what() << ")";
		exit( ErrorCode::FAILURE );
	}

	m_identifier = m_acceptor.local_endpoint().address().to_string()
			+ ":" + boost::lexical_cast<std::string>( m_acceptor.local_endpoint().port() );

	m_acceptor.async_accept( m_newConnection->socket(),
				 m_strand.wrap( boost::bind( &acceptor::handleAccept,
							     this,
							     boost::asio::placeholders::error )));
	LOG_INFO << "Accepting connections on " << m_identifier;
}

/// acceptor destructor
acceptor::~acceptor()
{
	LOG_TRACE << "Acceptor destructor called for " << m_identifier;
}

void acceptor::handleAccept( const boost::system::error_code& e )
{
	if ( !e )	{
		LOG_TRACE << "Received new connection on " << m_identifier;
		m_newConnection->start();

		ConnectionHandler *handler = m_srvHandler.newConnection( LocalTCPendpoint( m_acceptor.local_endpoint().address().to_string(),
											   m_acceptor.local_endpoint().port() ));
		m_newConnection.reset( new connection( m_IOservice, &m_connList, handler ));
		m_acceptor.async_accept( m_newConnection->socket(),
					 m_strand.wrap( boost::bind( &acceptor::handleAccept,
								     this,
								     boost::asio::placeholders::error )));
		LOG_DATA << "Acceptor on " << m_identifier << " ready for new connection";
	}
}

// Post a call to the stop function so that acceptor::stop() is safe to call
// from any thread.
void acceptor::stop()
{
	LOG_TRACE << "Acceptor for " << m_identifier << " received a shutdown request";
	m_IOservice.post( m_strand.wrap( boost::bind( &acceptor::handleStop, this )));
}

// The server is stopped by closing the acceptor.
// When all outstanding operations are completed
// all calls to io_service::run() will return.
void acceptor::handleStop()
{
	m_acceptor.close();
	LOG_DEBUG << "Closed acceptor for " << m_identifier;
	// at this point no more connections are accepted
	// Signal the list of processors to terminate
	connection_ptr conn;
	while (( conn = m_connList.pop()) != NULL )	{
		conn->setUnregistered();
		conn->signal();
	}
}


#ifdef WITH_SSL

SSLacceptor::SSLacceptor( boost::asio::io_service& IOservice,
			  const std::string& certFile, const std::string& keyFile,
			  bool verify, const std::string& CAchainFile, const std::string& CAdirectory,
			  const std::string& host, unsigned short port, unsigned maxConnections,
			  GlobalConnectionList& globalList,
			  _Wolframe::ServerHandler& srvHandler) :
	m_IOservice( IOservice ),
	m_strand( m_IOservice ),
	m_acceptor( m_IOservice ),
	m_SSLcontext( m_IOservice, boost::asio::ssl::context::sslv23 ),
	m_connList ( maxConnections, globalList ),
	m_srvHandler( srvHandler )
{
	boost::system::error_code	ec;

	m_SSLcontext.set_options( boost::asio::ssl::context::default_workarounds
				  | boost::asio::ssl::context::no_sslv2
				  | boost::asio::ssl::context::single_dh_use
				  );
	m_SSLcontext.set_password_callback( boost::bind( &SSLacceptor::getPassword, this ));
	if ( certFile.empty() )	{
		LOG_FATAL << "Empty SSL certificate filename";
		exit( 1 );
	}
	else if ( m_SSLcontext.use_certificate_chain_file( certFile, ec ) != 0 )	{
		LOG_FATAL << ec.message() << " loading SSL certificate file: " << certFile;
		exit( 1 );
	}

	if ( keyFile.empty() )	{
		LOG_FATAL << "SSL key filename is required";
		exit( 1 );
	}
	else if ( m_SSLcontext.use_private_key_file( keyFile, boost::asio::ssl::context::pem, ec ) != 0 )	{
		LOG_FATAL << ec.message() << " loading SSL key file: " << keyFile;
		exit( 1 );
	}
	//		SSLcontext_->use_tmp_dh_file( "dh4096.pem" );

	if ( ! CAchainFile.empty() )	{
		if ( m_SSLcontext.load_verify_file( CAchainFile, ec ) != 0 )	{
			LOG_FATAL << ec.message() << " loading SSL CA chain file: " << CAchainFile;
			exit( 1 );
		}
	}
	if ( ! CAdirectory.empty() )	{
		if ( m_SSLcontext.add_verify_path( CAdirectory, ec ) != 0 )	{
			LOG_FATAL << ec.message() << " setting CA directory: " << CAdirectory;
			exit( 1 );
		}
	}

	if ( verify )	{
		if ( CAchainFile.empty() && CAdirectory.empty() )	{
			LOG_FATAL << "Either a CA directory or a CA chain file is required";
			exit( 1 );
		}
		m_SSLcontext.set_verify_mode( boost::asio::ssl::context::verify_peer |
					      boost::asio::ssl::context::verify_fail_if_no_peer_cert );
		LOG_DEBUG << "SSL client certificate verification set to VERIFY";
	}
	else	{
		//		SSLcontext_.set_verify_mode( boost::asio::ssl::context::verify_none );
		m_SSLcontext.set_verify_mode( boost::asio::ssl::context::verify_peer );
		LOG_DEBUG << "SSL client certificate verification set to NONE";
	}

	// Open the acceptor(s) with the option to reuse the address (i.e. SO_REUSEADDR).
	boost::asio::ip::tcp::resolver resolver( m_IOservice );
	// Aba: empty service fails on boost 1.42 with "Service not found"
	boost::asio::ip::tcp::resolver::query query( host, "7661" );
	boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve( query );
	endpoint.port( port );

	ConnectionHandler *handler = m_srvHandler.newConnection( LocalSSLendpoint( host, port ));
	m_newConnection = SSLconnection_ptr( new SSLconnection( m_IOservice, m_SSLcontext, &m_connList, handler ));

	m_acceptor.open( endpoint.protocol() );
	m_acceptor.set_option( boost::asio::ip::tcp::acceptor::reuse_address( true ));
	m_acceptor.bind( endpoint );
	m_acceptor.listen();
	m_acceptor.async_accept( m_newConnection->socket().lowest_layer(),
				 m_strand.wrap( boost::bind( &SSLacceptor::handleAccept,
							     this,
							     boost::asio::placeholders::error )));
	m_identifier = m_acceptor.local_endpoint().address().to_string()
			+ ":" + boost::lexical_cast<std::string>( m_acceptor.local_endpoint().port() ) + " (SSL)";
	LOG_INFO << "Accepting connections on " << m_identifier;
}

/// acceptor destructor (SSL acceptor)
SSLacceptor::~SSLacceptor()
{
	LOG_TRACE << "Acceptor destructor called for " << m_identifier;
}

void SSLacceptor::handleAccept( const boost::system::error_code& e )
{
	if ( !e )	{
		LOG_DEBUG << "Received new connection on " << m_identifier;
		m_newConnection->start();

		ConnectionHandler *handler = m_srvHandler.newConnection( LocalSSLendpoint( m_acceptor.local_endpoint().address().to_string(),
											   m_acceptor.local_endpoint().port() ));
		m_newConnection.reset( new SSLconnection( m_IOservice, m_SSLcontext, &m_connList, handler ));
		m_acceptor.async_accept( m_newConnection->socket().lowest_layer(),
					 m_strand.wrap( boost::bind( &SSLacceptor::handleAccept,
								     this,
								     boost::asio::placeholders::error )));
		LOG_DATA << "Acceptor on " << m_identifier << " ready for new connection";
	}
}

std::string SSLacceptor::getPassword()
{
	return _Wolframe::AAAA::getPassword();
}

void SSLacceptor::stop()
{
	LOG_TRACE << "Acceptor for " << m_identifier << " received a shutdown request";
	m_IOservice.post( m_strand.wrap( boost::bind( &SSLacceptor::handleStop, this )));
}

void SSLacceptor::handleStop()
{
	m_acceptor.close();
	LOG_DEBUG << "Closed acceptor for " << m_identifier;
	// at this point no more connections are accepted
	// Signal the list of processors to terminate
	SSLconnection_ptr conn;
	while (( conn = m_connList.pop()) != NULL )	{
		conn->setUnregistered();
		conn->signal();
	}
}

#endif // WITH_SSL

}} // namespace _Wolframe::net
