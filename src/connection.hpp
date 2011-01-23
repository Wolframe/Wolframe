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

	template< typename T >
	class ConnectionList
	{
	public:
		ConnectionList( unsigned maxConnections )
					{ maxConn_ = maxConnections; }

		void push( T conn )	{
			connList_.push_back( conn );
			LOG_DATA << "Push connection: local: " << connList_.size() << " of " << maxConn_;
		}

		void remove( T conn )	{
			connList_.remove( conn );
			LOG_DATA << "Remove connection: local: " << connList_.size() << " of " << maxConn_;
		}

		T pop()	{
			if ( connList_.empty())
				return T();
			T conn = connList_.front();
			connList_.pop_front();
			LOG_DATA << "Pop connection: local: " << connList_.size() << " of " << maxConn_;
			return conn;
		}

		bool full()		{
			LOG_DATA << "Check number of connections: local: " << connList_.size() << " of " << maxConn_;
			return( maxConn_ > 0 && connList_.size() >= maxConn_ );
		}

	private:
		std::list< T >			connList_;
		unsigned			maxConn_;
	};


	class connection;		// forward declaration for connection_ptr
	typedef boost::shared_ptr< connection > connection_ptr;

	/// Represents a single connection from a client.
	class connection : public connectionBase< boost::asio::ip::tcp::socket >
	{
	public:
		/// Construct a connection with the given io_service.
		explicit connection( boost::asio::io_service& IOservice,
				     ConnectionList<connection_ptr>& connList,
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
		ConnectionList< connection_ptr >&	connList_;
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
					ConnectionList<SSLconnection_ptr>& connList,
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
		ConnectionList< SSLconnection_ptr >&	connList_;
	};

#endif // WITH_SSL

	} // namespace Network
} // namespace _SMERP

#endif // _CONNECTION_HPP_INCLUDED
