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
#include "connection.hpp"
#include "requestHandler.hpp"

namespace http {
namespace server3 {

/// The top-level class of the HTTP server.
class server
  : private boost::noncopyable
{
public:
  /// Construct the server to listen on the specified TCP address and port, and
  /// serve up files from the given directory.
  explicit server(const std::string& address, const std::string& port,
      std::size_t thread_pool_size,
      long timeout_duration_ms);

  /// Run the server's io_service loop.
  void run();

  /// Stop the server. Outstanding asynchronous operations will be completed.
  void stop();

  /// Abort the server. Outstanding asynchronous operations will be aborted.
  void abort();

private:
  /// Handle completion of an asynchronous accept operation.
  void handle_accept(const boost::system::error_code& e);

  /// Handle a request to stop the server.
  void handle_stop();


	std::size_t thread_pool_size_;			// The number of threads that will call io_service::run().
	long timeout_duration_ms_;			// The duration for timeouts in milliseconds.
	boost::asio::io_service io_service_;		// The io_service used to perform asynchronous operations.

	boost::asio::io_service::strand strand_;	// Strand to ensure the acceptor's handlers are not called concurrently.

	boost::asio::ip::tcp::acceptor acceptor_;	// Acceptor used to listen for incoming connections.

	connection_ptr new_connection_;			// The next connection to be accepted.

	request_handler request_handler_;		// The handler for all incoming requests.
};

} // namespace server3
} // namespace http

#endif // NETWORK_SERVER_HPP_INCLUDED
