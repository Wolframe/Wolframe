//
// connection.hpp
//

#ifndef _SSL_CONNECTION_HPP_INCLUDED
#define _SSL_CONNECTION_HPP_INCLUDED

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

#include "baseConnection.hpp"
#include "requestHandler.hpp"


namespace _SMERP {

	typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket>	ssl_socket;

	/// Represents a single connection from a client.
	class SSLconnection : public baseConnection,
			public boost::enable_shared_from_this<SSLconnection>/*,
			private boost::noncopyable*/
	{
	public:
		/// Construct a connection with the given io_service and SSL conetext.
		explicit SSLconnection( boost::asio::io_service& io_service,
					requestHandler& handler,
					unsigned long idleTimeout, unsigned long requestTimeout,
					unsigned long processTimeout, unsigned long answerTimeout,
					boost::asio::ssl::context& SSLcontext );

		/// Get the socket associated with the SSL connection.
		ssl_socket::lowest_layer_type& socket()	{ return SSLsocket_.lowest_layer(); }

		/// Start the first asynchronous operation for the connection.
		void start();

	private:
		/// Handle the SSL handshake
		void handleHandshake( const boost::system::error_code& error );

		/// Socket for the SSL connection.
		ssl_socket	SSLsocket_;
	};

	typedef boost::shared_ptr<SSLconnection> SSLconnection_ptr;


} // namespace _SMERP

#endif // _SSL_CONNECTION_HPP_INCLUDED
