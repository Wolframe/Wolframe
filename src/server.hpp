//
// server.hpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2008 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef NETWORK_SERVER_HPP_INCLUDED
#define NETWORK_SERVER_HPP_INCLUDED

#include <boost/asio.hpp>
#include <string>
#include <vector>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include "connection.hpp"
#include "request_handler.hpp"

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
      const std::string& doc_root, std::size_t thread_pool_size,
      long timeout_duration_ms);

  /// Run the server's io_service loop.
  void run();

  /// Stop the server. Outstanding asynchronous operations will be completed.
  void stop();

  // Abort the server. Outstanding asynchronous operations will be aborted.
  void abort();

private:
  /// Handle completion of an asynchronous accept operation.
  void handle_accept(const boost::system::error_code& e);

  /// Handle a request to stop the server.
  void handle_stop();

  /// The number of threads that will call io_service::run().
  std::size_t thread_pool_size_;

  /// The duration for timeouts in milliseconds.
  long timeout_duration_ms_;

  /// The io_service used to perform asynchronous operations.
  boost::asio::io_service io_service_;

  /// Strand to ensure the acceptor's handlers are not called concurrently.
  boost::asio::io_service::strand strand_;

  /// Acceptor used to listen for incoming connections.
  boost::asio::ip::tcp::acceptor acceptor_;

  /// The next connection to be accepted.
  connection_ptr new_connection_;

  /// The handler for all incoming requests.
  request_handler request_handler_;
};

} // namespace server3
} // namespace http

#endif // NETWORK_SERVER_HPP_INCLUDED
