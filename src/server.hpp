//
// server.hpp
//

#ifndef NETWORK_SERVER_HPP_INCLUDED
#define NETWORK_SERVER_HPP_INCLUDED

#include <boost/asio.hpp>
#include <string>
#include <vector>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include "appConfig.hpp"
#include "connection.hpp"
#include "requestHandler.hpp"

namespace _SMERP {

/// The top-level class of the SMERP server.
class server: private boost::noncopyable
{
/// public interface
public:
	/// Construct the server
	explicit server( const ApplicationConfiguration& config );

	/// Run the server's io_service loop.
	void run();

	/// Stop the server. Outstanding asynchronous operations will be completed.
	void stop();

	/// Abort the server. Outstanding asynchronous operations will be aborted.
	void abort();

/// private functions of the server
private:
	/// Handle completion of an asynchronous accept operation.
	void handleAccept( const boost::system::error_code& e );

	/// Handle a request to stop the server.
	void handleStop();

/// object variables

	std::size_t	threadPoolSize_;		// The number of threads that will call io_service::run().
	unsigned long	timeout_duration_ms_;		// The duration for timeouts in milliseconds.

	boost::asio::io_service		io_service_;	// The io_service used to perform asynchronous operations.
	boost::asio::io_service::strand	strand_;	// Strand to ensure the acceptor's handlers are not called concurrently.
	//	std::vector<boost::asio::ip::tcp::acceptor>	acceptor_;	// Acceptor(s) used to listen for incoming connections.
	boost::asio::ip::tcp::acceptor	acceptor_;	// Acceptor(s) used to listen for incoming connections.

	connection_ptr new_connection_;			// The next connection to be accepted.

	requestHandler requestHandler_;			// The handler for all incoming requests.
};

} // namespace _SMERP

#endif // NETWORK_SERVER_HPP_INCLUDED
