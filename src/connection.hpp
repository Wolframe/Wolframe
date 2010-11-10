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

namespace _SMERP {
	namespace Network {

	template< typename T >
	class ConnectionList
	{
	public:
		void push( T conn )
		{
			connList_.push_back( conn );
		}

		void remove( T conn )
		{
			connList_.remove( conn );
		}

		T pop()
		{
			if ( connList_.empty())
				return T();
			T conn = connList_.front();
			connList_.pop_front();
			return conn;
		}

	private:
		std::list< T >	connList_;
	};


	class connection;
	typedef boost::shared_ptr<connection> connection_ptr;

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
		void unregister();

	private:
		/// Socket for the connection.
		boost::asio::ip::tcp::socket	socket_;

		/// List of connections to which it belongs
		ConnectionList<connection_ptr>&	connList_;
	};


#ifdef WITH_SSL

	class SSLconnection;
	typedef boost::shared_ptr<SSLconnection> SSLconnection_ptr;

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
		void unregister();

	private:
		/// Handle the SSL handshake
		void handleHandshake( const boost::system::error_code& error );

		/// Socket for the SSL connection.
		ssl_socket				SSLsocket_;

		/// List of connections to which it belongs
		ConnectionList<SSLconnection_ptr>&	connList_;
	};

#endif // WITH_SSL

	} // namespace Network
} // namespace _SMERP

#endif // _CONNECTION_HPP_INCLUDED
