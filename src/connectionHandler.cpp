//
// connectionHandler.cpp
//

#include "connectionHandler.hpp"
#include "reply.hpp"
#include "request.hpp"

#include <string>


namespace _SMERP {

void connectionHandler::handleRequest( request& req, reply& rep )
{
	std::string msg = req.getValue();
	msg += '\n';
	rep.setValue( msg );
}


} // namespace _SMERP
