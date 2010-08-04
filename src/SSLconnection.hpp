//
// SSLconnection.hpp
//

#ifndef _SSL_CONNECTION_HPP_INCLUDED
#define _SSL_CONNECTION_HPP_INCLUDED

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

#include "connectionTimeout.hpp"
#include "connectionBase.hpp"
#include "requestHandler.hpp"

namespace _SMERP {

	typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket>	ssl_socket;

	/// Represents a single connection from a client.
	class SSLconnection : public connectionBase< ssl_socket >
	{
	public:
		/// Construct a connection with the given io_service and SSL conetext.
		explicit SSLconnection( boost::asio::io_service& IOservice,
					boost::asio::ssl::context& SSLcontext,
					connectionTimeout& timeouts,
					requestHandler& handler );

		/// Get the socket associated with the SSL connection.
		ssl_socket& socket()	{ return SSLsocket_; }

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
