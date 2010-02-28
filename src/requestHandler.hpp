//
// requestHandler.hpp
//

#ifndef REQUEST_HANDLER_HPP_INCLUDED
#define REQUEST_HANDLER_HPP_INCLUDED

#include <string>
#include <boost/noncopyable.hpp>

namespace _SMERP {

struct reply;
struct request;

/// The common handler for all incoming requests.
class request_handler
  : private boost::noncopyable
{
public:
  /// Construct with a directory containing files to be served.
  explicit request_handler();

  /// Handle a request and produce a reply.
  void handle_request(const request& req, reply& rep);

private:
};

} // namespace _SMERP

#endif // REQUEST_HANDLER_HPP_INCLUDED
