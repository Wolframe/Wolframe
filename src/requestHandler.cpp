//
// requestHandler.cpp
//

#include "requestHandler.hpp"
#include "reply.hpp"
#include "request.hpp"

#include <string>


namespace _SMERP {

requestHandler::requestHandler()
{
}

void requestHandler::handleRequest(const request& req, reply& rep)
{
    rep.content = req.content;
}


} // namespace _SMERP
