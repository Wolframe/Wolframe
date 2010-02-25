//
// request.hpp
// ~~~~~~~~~~~
//
// Copyright (c) 2003-2010 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef SERVER_REQUEST_HPP_INCLUDED
#define SERVER_REQUEST_HPP_INCLUDED

#include <string>

namespace http {
namespace server3 {

/// A request received from a client.
struct request
{
	std::string method;
};

} // namespace server3
} // namespace http

#endif // SERVER_REQUEST_HPP_INCLUDED
