//
// connectionHandler.hpp
//

#ifndef _CONNECTION_HANDLER_HPP_INCLUDED
#define _CONNECTION_HANDLER_HPP_INCLUDED

#include <vector>
#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>

namespace _SMERP {
	/// A reply to be sent to a client.
	class outMessage
	{
	public:
		/// Virtual functions deserve virtual destructors
		virtual ~outMessage()	{}

		/// Convert the reply into a vector of buffers. The buffers do not own the
		/// underlying memory blocks, therefore the reply object must remain valid and
		/// not be changed until the write operation has completed.
		virtual std::vector<boost::asio::const_buffer> toBuffers() = 0;
	};

	struct	networkOperation
	{
		enum Operation	{
			READ,
			WRITE,
			TERMINATE
		};
		Operation	operation;
		outMessage*	msg;
	};


	/// The common handler for the connection status.
	class connectionHandler : private boost::noncopyable
	{
	public:
		connectionHandler()	{}
		virtual ~connectionHandler()	{}

		/// Parse incoming data. The return value indicates how much of the
		/// input has been consumed.
		char *parseInput( char *begin, std::size_t bytesTransferred );

		/// Handle a request and produce a reply.
		networkOperation nextOperation();

	private:
	};

} // namespace _SMERP

#endif // _CONNECTION_HANDLER_HPP_INCLUDED
