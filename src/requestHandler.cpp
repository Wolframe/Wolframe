//
// request_handler.cpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2010 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "requestHandler.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include "reply.hpp"
#include "request.hpp"

namespace http {
namespace server3 {

request_handler::request_handler()
{
}

void request_handler::handle_request(const request& req, reply& rep)
{
    rep.content = req.content;
}


} // namespace server3
} // namespace http
