//
// connectionHandler.hpp
//

#ifndef _CONNECTION_HANDLER_HPP_INCLUDED
#define _CONNECTION_HANDLER_HPP_INCLUDED

#include <vector>
#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>

namespace _SMERP {
	/// A message to be sent to a client.
	struct outputMessage
	{
	public:
		const void	*data;
		std::size_t	size;
	};

	struct	networkOperation
	{
	public:
		enum Operation	{
			READ,
			WRITE,
			TERMINATE
		};
		Operation	operation;
		outputMessage	*msg;
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
