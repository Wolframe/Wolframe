//
// request_handler.cpp
//

#include "requestHandler.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include "reply.hpp"
#include "request.hpp"

namespace _SMERP {

request_handler::request_handler()
{
}

void request_handler::handle_request(const request& req, reply& rep)
{
    rep.content = req.content;
}


} // namespace _SMERP
