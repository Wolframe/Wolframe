/************************************************************************

 Copyright (C) 2011 - 2014 Project Wolframe.
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
// connection.cpp
//

#include "connectionBase.hpp"
#include "connection.hpp"
#include "logger-v1.hpp"
#include "system/connectionHandler.hpp"

#ifdef WITH_SSL
#include "system/SSLcertificateInfo.hpp"
#endif

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

namespace _Wolframe {
namespace net {

static const char* REFUSE_MSG = "ERR Server is busy. Please try again later.\n";
static const char* DENY_MSG = "BAD Server denied connection.\n";

void GlobalConnectionList::addList( SocketConnectionList< connection_ptr >* lst )
{
	{
		boost::mutex::scoped_lock lock( m_mutex );
		m_connList.push_back( lst );
	}
	LOG_DATA << "Added unencrypted connection list, " << m_connList.size() << " list(s) for unencrypted connections";
}

void GlobalConnectionList::removeList( SocketConnectionList< connection_ptr >* lst )
{
	{
		boost::mutex::scoped_lock lock( m_mutex );
		m_connList.remove( lst );
	}
	LOG_DATA << "Removed unencrypted connection list, " << m_connList.size() << " list(s) for unencrypted connections";
}

#ifdef WITH_SSL
void GlobalConnectionList::addList( SocketConnectionList< SSLconnection_ptr >* lst )
{
	{
		boost::mutex::scoped_lock lock( m_mutex );
		m_SSLconnList.push_back( lst );
	}
	LOG_DATA << "Added SSL connection list, " << m_SSLconnList.size() << " list(s) for SSL connections";
}

void GlobalConnectionList::removeList( SocketConnectionList< SSLconnection_ptr >* lst )
{
	{
		boost::mutex::scoped_lock lock( m_mutex );
		m_SSLconnList.remove( lst );
	}
	LOG_DATA << "Removed SSL connection list, " << m_SSLconnList.size() << " list(s) for SSL connections";
}
#endif // WITH_SSL


connection::connection( boost::asio::io_service& IOservice,
			SocketConnectionList< connection_ptr >* connList,
			ConnectionHandler* handler ) :
	ConnectionBase< boost::asio::ip::tcp::socket >( IOservice, handler ),
	m_socket( IOservice ),
	m_connList( connList )
{
	LOG_TRACE << "New connection created";
}

connection::~connection()
{
	if ( m_ID.empty() )
		LOG_TRACE << "Connection (unconnected) destroyed";
	else
		LOG_TRACE << "Connection to " << m_ID <<" destroyed";
}

void connection::deny_connection()
{
	LOG_DEBUG << "Refusing connection from " << identifier() << ". Not allowed.";
	boost::system::error_code ignored_ec;
	socket().write_some( boost::asio::buffer( DENY_MSG, strlen( DENY_MSG ) ));
	socket().lowest_layer().shutdown( boost::asio::ip::tcp::socket::shutdown_both, ignored_ec );
}

void connection::start()
{
	assert( m_connList );
	identifier( std::string( socket().remote_endpoint().address().to_string())
		    + ":" + boost::lexical_cast<std::string>( socket().remote_endpoint().port() ));
	LOG_TRACE << "Starting connection to " << identifier();

	if ( m_connList->push( boost::static_pointer_cast< connection >( shared_from_this() )) )	{
		m_connHandler->setPeer( RemoteTCPendpoint( socket().remote_endpoint().address().to_string(),
							   socket().remote_endpoint().port()));
		nextOperation();
	}
	else	{
		// the maximum number of connections has been reached -> refuse the connection
		LOG_DEBUG << "Refusing connection from " << identifier() << ". Too many connections.";
		boost::system::error_code ignored_ec;
		socket().write_some( boost::asio::buffer( REFUSE_MSG, strlen( REFUSE_MSG ) ));
		socket().lowest_layer().shutdown( boost::asio::ip::tcp::socket::shutdown_both, ignored_ec );
	}
}


std::string connection::toString() const
{
	std::string str;

	boost::system::error_code ec;
	boost::asio::ip::tcp::endpoint ep = m_socket.remote_endpoint( ec );
	if ( !ec )	{
		str = ep.address().to_string()  + ":" + boost::lexical_cast< std::string >( ep.port() );
		str += "->";
	}
	ep = m_socket.local_endpoint( ec );
	if ( !ec )
		str += ep.address().to_string() + ":" + boost::lexical_cast< std::string >( ep.port() );
	return str;
}


#ifdef WITH_SSL

SSLconnection::SSLconnection( boost::asio::io_service& IOservice,
			      boost::asio::ssl::context& SSLcontext,
			      SocketConnectionList< SSLconnection_ptr >* connList,
			      ConnectionHandler *handler ) :
	ConnectionBase< ssl_socket >( IOservice, handler ),
	m_SSLsocket( IOservice, SSLcontext ),
	m_connList( connList ),
	m_connection_denied( false )
{
	LOG_TRACE << "New SSL connection created";
}

SSLconnection::~SSLconnection()
{
	if ( m_ID.empty() )
		LOG_TRACE << "SSL connection (unconnected) destroyed";
	else
		LOG_TRACE << "Connection to " << m_ID <<" destroyed";
}


void SSLconnection::start()
{
	identifier( std::string( m_SSLsocket.lowest_layer().remote_endpoint().address().to_string())
		    + ":" + boost::lexical_cast<std::string>( m_SSLsocket.lowest_layer().remote_endpoint().port() )
		    + " (SSL)");
	LOG_TRACE << "Starting connection to " << identifier();

	m_SSLsocket.async_handshake( boost::asio::ssl::stream_base::server,
				     m_strand.wrap( boost::bind( &SSLconnection::handleHandshake,
								 boost::static_pointer_cast< SSLconnection >( shared_from_this() ),
								 boost::asio::placeholders::error )));
}

void SSLconnection::deny_connection()
{
	m_connection_denied = true;
}

void SSLconnection::handleHandshake( const boost::system::error_code& e )
{
	assert( m_connList );

	if ( !e )	{
		LOG_DATA << "successful SSL handshake, peer " << identifier();

		SSL* ssl = m_SSLsocket.impl()->ssl;
		X509* peerCert = SSL_get_peer_certificate( ssl );
		SSLcertificateInfo* certInfo = NULL;

		if ( peerCert )	{
			certInfo = new SSLcertificateInfo( peerCert );
		}
		if (m_connection_denied)
		{
			LOG_DEBUG << "Refusing connection from " << identifier() << ". Not allowed.";
			boost::system::error_code ignored_ec;
			socket().write_some( boost::asio::buffer( DENY_MSG, strlen( DENY_MSG ) ));
			socket().lowest_layer().shutdown( boost::asio::ip::tcp::socket::shutdown_both, ignored_ec );
		}
		else if ( m_connList->push( boost::static_pointer_cast< SSLconnection >( shared_from_this() )) )	{
			m_connHandler->setPeer( RemoteSSLendpoint( m_SSLsocket.lowest_layer().remote_endpoint().address().to_string(),
								   m_SSLsocket.lowest_layer().remote_endpoint().port(),
								   certInfo ));
			nextOperation();
		}
		else	{
			// the maximum number of connections has been reached -> refuse the connection
			LOG_DEBUG << "Refusing connection from " << identifier() << ". Too many connections.";
			boost::system::error_code ignored_ec;
			socket().write_some( boost::asio::buffer( REFUSE_MSG, strlen( REFUSE_MSG ) ));
			socket().lowest_layer().shutdown( boost::asio::ip::tcp::socket::shutdown_both, ignored_ec );
		}
	}
	else	{
		LOG_DEBUG << e.message() << ", SSL handshake, peer " << identifier();
		// delete this;
		boost::system::error_code ignored_ec;
		socket().lowest_layer().shutdown( boost::asio::ip::tcp::socket::shutdown_both, ignored_ec );
	}
}


std::string SSLconnection::toString() const
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

#endif // WITH_SSL

}} // namespace _Wolframe::net
