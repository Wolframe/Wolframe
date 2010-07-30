//
// connection.hpp
//

#ifndef _CONNECTION_HPP_INCLUDED
#define _CONNECTION_HPP_INCLUDED

#include <boost/asio.hpp>

#include "baseConnection.hpp"
#include "requestHandler.hpp"

namespace _SMERP {

	/// Represents a single connection from a client.
	class connection : public baseConnection
	{
	public:
		/// Construct a connection with the given io_service.
		explicit connection( boost::asio::io_service& io_service,
				     requestHandler& handler,
				     unsigned long idleTimeout, unsigned long requestTimeout,
				     unsigned long processTimeout, unsigned long answerTimeout );

		/// Get the socket associated with the connection.
		boost::asio::ip::tcp::socket& socket()	{ return socket_; }

		/// Start the first asynchronous operation for the connection.
		void start();

	private:
		/// Socket for the connection.
		boost::asio::ip::tcp::socket socket_;
	};

//	typedef boost::shared_ptr<connection> connection_ptr;


} // namespace _SMERP

#endif // _CONNECTION_HPP_INCLUDED
