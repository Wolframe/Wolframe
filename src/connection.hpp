/************************************************************************

 Copyright (C) 2011, 2012 Project Wolframe.
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
// connection.hpp
//

#ifndef _CONNECTION_HPP_INCLUDED
#define _CONNECTION_HPP_INCLUDED

#include <boost/asio.hpp>
#ifdef WITH_SSL
#include <boost/asio/ssl.hpp>
#endif // WITH_SSL

#include <list>
#include "connectionBase.hpp"
#include "connectionHandler.hpp"
#include <boost/thread/mutex.hpp>

namespace _Wolframe {
namespace net {

class connection;		// forward declaration for connection_ptr
typedef boost::shared_ptr< connection > connection_ptr;

#ifdef WITH_SSL
class SSLconnection;		// forward declaration for SSLconnection_ptr
typedef boost::shared_ptr< SSLconnection > SSLconnection_ptr;

#endif // WITH_SSL


template< typename T > class SocketConnectionList;

class GlobalConnectionList
{
public:
	GlobalConnectionList( unsigned maxConnections )	{
		m_maxConn = maxConnections;
	}

	void addList( SocketConnectionList< connection_ptr >* lst );
	void removeList( SocketConnectionList< connection_ptr >* lst );
#ifdef WITH_SSL
	void addList( SocketConnectionList< SSLconnection_ptr >* lst );
	void removeList( SocketConnectionList< SSLconnection_ptr >* lst );
#endif // WITH_SSL
	bool isFull();

private:
	std::list< SocketConnectionList< connection_ptr >* >	m_connList;
#ifdef WITH_SSL
	std::list< SocketConnectionList< SSLconnection_ptr >* >	m_SSLconnList;
#endif // WITH_SSL
	unsigned						m_maxConn;
	boost::mutex						m_mutex;
};


template< typename T >
class SocketConnectionList
{
public:
	SocketConnectionList( unsigned maxConnections, GlobalConnectionList& globalList )
		: m_connListSize( 0 ), m_globalList( globalList )
	{
		m_maxConn = maxConnections;
		m_globalList.addList( this );
	}

	~SocketConnectionList()
	{
		m_globalList.removeList( this );
	}

	std::size_t size()	{ return m_connListSize; }

	void push( T conn )
	{
		{
			boost::mutex::scoped_lock lock( m_mutex );
			m_connList.push_back( conn );
			++m_connListSize;
			assert( m_connListSize == m_connList.size() );
		}
		LOG_DATA << "PUSH - Connections on socket: " << m_connListSize << " of maximum " << m_maxConn << ", " << conn->toString();
	}

	void remove( T conn )
	{
		{
			boost::mutex::scoped_lock lock( m_mutex );
			m_connList.remove( conn );
			--m_connListSize;
			assert( m_connListSize == m_connList.size() );
		}
		LOG_DATA << "REMOVE - Connections on socket: " << m_connListSize << " of maximum " << m_maxConn << ", " << conn->toString();
	}

	T pop()
	{
		T conn;
		LOG_DATA << "POP - Connections on socket: " << m_connListSize << " of maximum " << m_maxConn;
		{
			boost::mutex::scoped_lock lock( m_mutex );
			if ( m_connList.empty() )
				return T();
			conn = m_connList.front();
			m_connList.pop_front();
			--m_connListSize;
			assert( m_connListSize == m_connList.size() );
		}
		LOG_DATA << "POP - Connection " << conn->toString();
		return conn;
	}

	bool isFull()	{
		if ( m_maxConn > 0 )	{
			LOG_DATA << "Connections on socket: " << m_connListSize << " of maximum " << m_maxConn;
			return(( m_connListSize >= m_maxConn ) || m_globalList.isFull() );
		}
		else	{
			LOG_DATA << "Connections on socket: " << m_connListSize << ", no maximum limit";
			return( m_globalList.isFull() );
		}
	}

private:
	std::list< T >		m_connList;
	std::size_t		m_connListSize;
	unsigned		m_maxConn;
	GlobalConnectionList&	m_globalList;
	boost::mutex		m_mutex;
};


/// Represents a single connection from a client.
class connection : public ConnectionBase< boost::asio::ip::tcp::socket >
{
public:
	/// Construct a connection with the given io_service.
	explicit connection( boost::asio::io_service& IOservice,
			     SocketConnectionList<connection_ptr>& connList,
			     ConnectionHandler *handler );

	~connection();

	/// Get the socket associated with the connection.
	boost::asio::ip::tcp::socket& socket()	{ return m_socket; }

	/// Start the first asynchronous operation for the connection.
	void start();

	/// Unregister the connection from the list of active connections
	void unregister()	{
		m_connList.remove( boost::static_pointer_cast<connection>( shared_from_this()) );
	}

	std::string toString() const;

private:
	/// Socket for the connection.
	boost::asio::ip::tcp::socket		m_socket;

	/// List of connections to which it belongs
	SocketConnectionList<connection_ptr>&	m_connList;
};


#ifdef WITH_SSL

typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket>	ssl_socket;

/// Represents a single connection from a client.
class SSLconnection : public ConnectionBase< ssl_socket >
{
public:
	/// Construct a connection with the given io_service and SSL conetext.
	explicit SSLconnection( boost::asio::io_service& IOservice,
				boost::asio::ssl::context& SSLcontext,
				SocketConnectionList<SSLconnection_ptr>& connList,
				ConnectionHandler *handler );

	~SSLconnection();

	/// Get the socket associated with the SSL connection.
	ssl_socket& socket()	{ return m_SSLsocket; }

	/// Start the first asynchronous operation for the connection.
	void start();

	/// Unregister the connection from the list of active connections
	void unregister()	{
		m_connList.remove( boost::static_pointer_cast<SSLconnection>( shared_from_this()) );
	}

	std::string toString() const;

private:
	/// Handle the SSL handshake
	void handleHandshake( const boost::system::error_code& error );

	/// Socket for the SSL connection.
	ssl_socket				m_SSLsocket;

	/// List of connections to which it belongs
	SocketConnectionList<SSLconnection_ptr>& m_connList;
};

#endif // WITH_SSL

}} // namespace _Wolframe::net

#endif // _CONNECTION_HPP_INCLUDED
