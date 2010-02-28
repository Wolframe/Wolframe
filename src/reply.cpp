//
// reply.cpp
//

#include "reply.hpp"
#include <string>


namespace _SMERP {


std::vector<boost::asio::const_buffer> reply::to_buffers()
{
  std::vector<boost::asio::const_buffer> buffers;

  buffers.push_back(boost::asio::buffer(content));
  return buffers;
}


} // namespace _SMERP
