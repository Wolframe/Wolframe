//
// reply.hpp
// ~~~~~~~~~
//
// Copyright (c) 2003-2010 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef SERVER_REPLY_HPP_INCLUDED
#define SERVER_REPLY_HPP_INCLUDED

#include <string>
#include <vector>
#include <boost/asio.hpp>

namespace http {
namespace server3 {

/// A reply to be sent to a client.
struct reply
{
//private:
  /// The content to be sent in the reply.
  std::string content;

public:
  /// Convert the reply into a vector of buffers. The buffers do not own the
  /// underlying memory blocks, therefore the reply object must remain valid and
  /// not be changed until the write operation has completed.
  std::vector<boost::asio::const_buffer> to_buffers();
};

} // namespace server3
} // namespace http

#endif // SERVER_REPLY_HPP_INCLUDED
