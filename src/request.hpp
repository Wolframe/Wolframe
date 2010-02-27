//
// request.hpp
//

#ifndef SERVER_REQUEST_HPP_INCLUDED
#define SERVER_REQUEST_HPP_INCLUDED

#include <string>

namespace http {
namespace server3 {

/// A request received from a client.
struct request
{
	std::string content;
};

} // namespace server3
} // namespace http

#endif // SERVER_REQUEST_HPP_INCLUDED
