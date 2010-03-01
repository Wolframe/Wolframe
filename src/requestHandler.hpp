//
// requestHandler.hpp
//

#ifndef _REQUEST_HANDLER_HPP_INCLUDED
#define _REQUEST_HANDLER_HPP_INCLUDED

#include <boost/noncopyable.hpp>

namespace _SMERP {

class reply;
class request;

/// The common handler for all incoming requests.
class requestHandler
  : private boost::noncopyable
{
public:
  /// Construct with a directory containing files to be served.
  explicit requestHandler();

  /// Handle a request and produce a reply.
  void handleRequest(const request& req, reply& rep);

private:
};

} // namespace _SMERP

#endif // _REQUEST_HANDLER_HPP_INCLUDED
