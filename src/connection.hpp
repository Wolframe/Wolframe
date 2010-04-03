//
// connection.hpp
//

#ifndef _NETWORK_CONNECTION_HPP_INCLUDED
#define _NETWORK_CONNECTION_HPP_INCLUDED

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/array.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "reply.hpp"
#include "request.hpp"
#include "requestHandler.hpp"

namespace _SMERP {

	typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket>	ssl_socket;

	/// Represents a single connection from a client.
	class connection : public boost::enable_shared_from_this<connection>,
			private boost::noncopyable
	{
	public:
		/// Construct a connection with the given io_service and SSL conetext.
		explicit connection( boost::asio::io_service& io_service, requestHandler& handler, long timeoutDuration,
				     boost::asio::ssl::context *SSLcontext = NULL );

		/// Get the socket associated with the connection.
		boost::asio::ip::tcp::socket& socket()		{ return *socket_; };

		/// Get the socket associated with the SSL connection.
		ssl_socket::lowest_layer_type& SSLsocket()	{ return SSLsocket_->lowest_layer(); };

		/// Start the first asynchronous operation for the connection.
		void start();

	private:
		/// Handle the SSL handshake
		void handleHandshake( const boost::system::error_code& error );

		/// Handle completion of a read operation.
		void handle_read(const boost::system::error_code& e,
				 std::size_t bytes_transferred);

		/// Handle completion of a write operation.
		void handle_write(const boost::system::error_code& e);

		/// Handle completion of a timer operation.
		void handleTimeout( const boost::system::error_code& e );

		/// Strand to ensure the connection's handlers are not called concurrently.
		boost::asio::io_service::strand strand_;

		/// true if it is a SSL connection, false otherwise
		bool	isSSL_;

		/// Socket for the connection.
		boost::asio::ip::tcp::socket *socket_;

		/// Socket for the SSL connection.
		ssl_socket	*SSLsocket_;

		/// The handler used to process the incoming request.
		requestHandler& requestHandler_;

		/// Buffer for incoming data.
		boost::array<char, 8192> buffer_;

		/// The incoming request.
		request request_;

		/// The reply to be sent back to the client.
		reply reply_;

		/// The timer for timeouts.
		boost::asio::deadline_timer timer_;

		/// The duration for timeouts in milliseconds.
		long timeoutDuration_;

	};

	typedef boost::shared_ptr<connection> connection_ptr;


} // namespace _SMERP

#endif // _NETWORK_CONNECTION_HPP_INCLUDED
