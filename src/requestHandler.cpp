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

void requestHandler::handleRequest( request& req, reply& rep )
{
	std::string msg = req.getValue();
	msg += '\n';
	rep.setValue( msg );
}


} // namespace _SMERP
