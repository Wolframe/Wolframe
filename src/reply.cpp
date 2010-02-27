//
// reply.cpp
//

#include "reply.hpp"
#include <string>
#include <boost/lexical_cast.hpp>

namespace http {
namespace server3 {


std::vector<boost::asio::const_buffer> reply::to_buffers()
{
  std::vector<boost::asio::const_buffer> buffers;

  buffers.push_back(boost::asio::buffer(content));
  return buffers;
}


} // namespace server3
} // namespace http
