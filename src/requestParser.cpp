//
// request_parser.cpp
//

#include "requestParser.hpp"
#include "request.hpp"

namespace _SMERP {

request_parser::request_parser()
  : state_(method_start)
{
}

void request_parser::reset()
{
  state_ = method_start;
}

boost::tribool request_parser::consume(request& req, char input)
{
//      return boost::indeterminate;
//  default:
    return false;
}


} // namespace _SMERP
