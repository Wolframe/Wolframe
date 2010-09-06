//
// connectionHandler.hpp
//

#ifndef _CONNECTION_HANDLER_HPP_INCLUDED
#define _CONNECTION_HANDLER_HPP_INCLUDED

#include <vector>
#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>

#include "request.hpp"
#include "reply.hpp"


namespace _SMERP {
	/// A request received from a client.
	class inMessage
	{
	public:
	};

	/// A reply to be sent to a client.
	class outMessage
	{
	public:
		/// Convert the reply into a vector of buffers. The buffers do not own the
		/// underlying memory blocks, therefore the reply object must remain valid and
		/// not be changed until the write operation has completed.
		virtual std::vector<boost::asio::const_buffer> toBuffers() = 0;

		/// Virtual functions deserve virtual destructors
		virtual ~outMessage();
	};


	/// The common handler for the connection status.
	class connectionHandler : private boost::noncopyable
	{
	public:
		connectionHandler()	{}
		/// Handle a request and produce a reply.
		void handleRequest( request& req, reply& rep );

	private:
	};

} // namespace _SMERP

#endif // _CONNECTION_HANDLER_HPP_INCLUDED
