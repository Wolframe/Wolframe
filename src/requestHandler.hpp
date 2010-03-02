//
// requestHandler.hpp
//

#ifndef _REQUEST_HANDLER_HPP_INCLUDED
#define _REQUEST_HANDLER_HPP_INCLUDED

#include <boost/noncopyable.hpp>
#include "reply.hpp"
#include "request.hpp"

namespace _SMERP {


/// The common handler for all incoming requests.
class requestHandler : private boost::noncopyable
{
public:
	explicit requestHandler();

	/// Handle a request and produce a reply.
	void handleRequest( const request& req, reply& rep );

private:
};

} // namespace _SMERP

#endif // _REQUEST_HANDLER_HPP_INCLUDED
