//
// reply.cpp
//

#include "reply.hpp"
#include <string>


namespace _SMERP {


std::vector<boost::asio::const_buffer> reply::toBuffers()
{
  std::vector<boost::asio::const_buffer> buffers;

  buffers.push_back( boost::asio::buffer( content_ ));
  return buffers;
}


} // namespace _SMERP
