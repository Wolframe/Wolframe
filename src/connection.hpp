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
#include "atomicCounter.hpp"

namespace _SMERP {
namespace Network {


	class GlobalConnectionList;

	template< typename T >
	class SocketConnectionList
	{
	public:
		SocketConnectionList( unsigned maxConnections )
					{ maxConn_ = maxConnections; }

		std::size_t size()	{ return connList_.size(); }

		void push( T conn )	{
			connList_.push_back( conn );
			LOG_DATA << "Push connection to list: " << connList_.size() << " of " << maxConn_;
		}

		void remove( T conn )	{
			connList_.remove( conn );
			LOG_DATA << "Remove connection from list: " << connList_.size() << " of " << maxConn_;
		}

		T pop()	{
			if ( connList_.empty())
				return T();
			T conn = connList_.front();
			connList_.pop_front();
			LOG_DATA << "Pop connection from list: " << connList_.size() << " of " << maxConn_;
			return conn;
		}

		bool isFull()		{
			LOG_DATA << "Check local number of connections: " << connList_.size() << " of " << maxConn_;
			return( maxConn_ > 0 && connList_.size() >= maxConn_ );
		}

	private:
		std::list< T >		connList_;
		unsigned		maxConn_;
//		GlobalConnectionList&	globalList_;
	};


	class connection;		// forward declaration for connection_ptr
	typedef boost::shared_ptr< connection > connection_ptr;

	/// Represents a single connection from a client.
	class connection : public connectionBase< boost::asio::ip::tcp::socket >
	{
	public:
		/// Construct a connection with the given io_service.
		explicit connection( boost::asio::io_service& IOservice,
				     SocketConnectionList<connection_ptr>& connList,
				     connectionHandler *handler );

		~connection();

		/// Get the socket associated with the connection.
		boost::asio::ip::tcp::socket& socket()	{ return socket_; }

		/// Start the first asynchronous operation for the connection.
		void start();

		/// Unregister the connection from the list of active connections
		void unregister()	{
			connList_.remove( boost::static_pointer_cast<connection>( shared_from_this()) );
		}

	private:
		/// Socket for the connection.
		boost::asio::ip::tcp::socket		socket_;

		/// List of connections to which it belongs
		SocketConnectionList<connection_ptr>&	connList_;
	};


#ifdef WITH_SSL

	class SSLconnection;		// forward declaration for SSLconnection_ptr
	typedef boost::shared_ptr< SSLconnection > SSLconnection_ptr;

	typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket>	ssl_socket;

	/// Represents a single connection from a client.
	class SSLconnection : public connectionBase< ssl_socket >
	{
	public:
		/// Construct a connection with the given io_service and SSL conetext.
		explicit SSLconnection( boost::asio::io_service& IOservice,
					boost::asio::ssl::context& SSLcontext,
					SocketConnectionList<SSLconnection_ptr>& connList,
					connectionHandler *handler );

		~SSLconnection();

		/// Get the socket associated with the SSL connection.
		ssl_socket& socket()	{ return SSLsocket_; }

		/// Start the first asynchronous operation for the connection.
		void start();

		/// Unregister the connection from the list of active connections
		void unregister()	{
			connList_.remove( boost::static_pointer_cast<SSLconnection>( shared_from_this()) );
		}

	private:
		/// Handle the SSL handshake
		void handleHandshake( const boost::system::error_code& error );

		/// Socket for the SSL connection.
		ssl_socket				SSLsocket_;

		/// List of connections to which it belongs
		SocketConnectionList<SSLconnection_ptr>& connList_;
	};

#endif // WITH_SSL


	class GlobalConnectionList
	{
	public:
		GlobalConnectionList( unsigned maxConnections )
					{ maxConn_ = maxConnections; }

		void addList( SocketConnectionList< connection_ptr > lst )	{
			connList_.push_back( lst );
			LOG_DATA << "Added unencrypted connection list, " << connList_.size() << "lists";
		}

#ifdef WITH_SSL
		void addList( SocketConnectionList< SSLconnection_ptr > lst )	{
			SSLconnList_.push_back( lst );
			LOG_DATA << "Added unencrypted connection list, " << SSLconnList_.size() << "lists";
		}
#endif // WITH_SSL

		bool isFull()		{
			unsigned conns = 0;

			for ( std::list< SocketConnectionList< connection_ptr > >::iterator it = connList_.begin();
											it != connList_.end(); it++ )
				conns += it->size();
#ifdef WITH_SSL
			for ( std::list< SocketConnectionList< SSLconnection_ptr > >::iterator it = SSLconnList_.begin();
											it != SSLconnList_.end(); it++ )
				conns += it->size();
#endif // WITH_SSL
			LOG_DATA << "Check global number of connections: local: " << conns << " of " << maxConn_;
			return( maxConn_ > 0 && conns >= maxConn_ );
		}

	private:
		std::list< SocketConnectionList< connection_ptr > >	connList_;
#ifdef WITH_SSL
		std::list< SocketConnectionList< SSLconnection_ptr > >	SSLconnList_;
#endif // WITH_SSL
		unsigned						maxConn_;
	};

} // namespace Network
} // namespace _SMERP

#endif // _CONNECTION_HPP_INCLUDED
