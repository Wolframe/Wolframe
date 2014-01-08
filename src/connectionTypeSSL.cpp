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
//\file connectionTypeSSL.cpp
//\brief Implementation of the connection type SSL
#include "connectionTypeSSL.hpp"
#ifdef WITH_SSL

using namespace _Wolframe;
using namespace _Wolframe::net;

ConnectionTypeSSL::ConnectionTypeSSL(
			boost::asio::io_service& IOservice,
			boost::asio::ssl::context& SSLcontext,
			types::SyncCounter* classCounter_,
			types::SyncCounter* globalCounter_,
			ConnectionHandler *handler )
	:ConnectionBase< ssl_socket >( IOservice, handler )
	,ConnectionCount(classCounter_,globalCounter_)
	m_SSLsocket( IOservice, SSLcontext )
{
	LOG_TRACE << "New SSL connection created";
}

ConnectionTypeSSL::~ConnectionTypeSSL()
{
	if ( m_ID.empty() )
		LOG_TRACE << "SSL connection (unconnected) destroyed";
	else
		LOG_TRACE << "SSL connection to " << m_ID <<" destroyed";
}


void ConnectionTypeSSL::start()
{
	identifier( std::string( m_SSLsocket.lowest_layer().remote_endpoint().address().to_string())
		    + ":" + boost::lexical_cast<std::string>( m_SSLsocket.lowest_layer().remote_endpoint().port() )
		    + " (SSL)");
	LOG_TRACE << "Starting connection to " << identifier();

	m_SSLsocket.async_handshake( boost::asio::ssl::stream_base::server,
				     m_strand.wrap( boost::bind( &ConnectionTypeSSL::handleHandshake,
								 boost::static_pointer_cast< ConnectionTypeSSL >( shared_from_this() ),
								 boost::asio::placeholders::error )));
}


void ConnectionTypeSSL::handleHandshake( const boost::system::error_code& e )
{
	assert( m_connList );

	if ( !e )	{
		LOG_DATA << "successful SSL handshake, peer " << identifier();

		// if the maximum number of connections has been reached refuse the connection
		if ( !registerConnection() )	{
			LOG_DEBUG << "Refusing connection from " << identifier() << ". Too many connections.";
			boost::system::error_code ignored_ec;
			socket().write_some( boost::asio::buffer( REFUSE_MSG, strlen( REFUSE_MSG ) ));
			socket().lowest_layer().shutdown( boost::asio::ip::tcp::socket::shutdown_both, ignored_ec );
		}
		else	{
			SSL* ssl = m_SSLsocket.impl()->ssl;
			X509* peerCert = SSL_get_peer_certificate( ssl );
			SSLcertificateInfo* certInfo = NULL;

			if ( peerCert )	{
				certInfo = new SSLcertificateInfo( peerCert );
			}
			m_connList->push( boost::static_pointer_cast< ConnectionTypeSSL >( shared_from_this()) );
			m_connHandler->setPeer( RemoteSSLendpoint( m_SSLsocket.lowest_layer().remote_endpoint().address().to_string(),
								   m_SSLsocket.lowest_layer().remote_endpoint().port(),
								   certInfo ));
			nextOperation();
		}
	}
	else	{
		LOG_DEBUG << e.message() << ", SSL handshake, peer " << identifier();
		//		delete this;
		boost::system::error_code ignored_ec;
		socket().lowest_layer().shutdown( boost::asio::ip::tcp::socket::shutdown_both, ignored_ec );
	}
}


std::string ConnectionTypeSSL::toString() const
{
	std::string str;

	boost::system::error_code ec;
	boost::asio::ip::tcp::endpoint ep = m_SSLsocket.lowest_layer().remote_endpoint( ec );
	if ( !ec )	{
		str = ep.address().to_string() + ":" + boost::lexical_cast< std::string >( ep.port() );
		str += "->";
	}
	ep = m_SSLsocket.lowest_layer().local_endpoint( ec );
	if ( !ec )
		str += ep.address().to_string() + ":" + boost::lexical_cast< std::string >( ep.port() );
	return str;
}

#endif

