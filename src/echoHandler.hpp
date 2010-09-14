//
// echoHandler.hpp - simple echo handler example
//

#ifndef _ECHO_HANDLER_HPP_INCLUDED
#define _ECHO_HANDLER_HPP_INCLUDED

#include "connectionHandler.hpp"

namespace _SMERP {
	/// The handler
	class echoHandler : public connectionHandler
	{
	public:
		/// Parse incoming data. The return value indicates how much of the
		/// input has been consumed.
		char *parseInput( char *begin, std::size_t bytesTransferred );

		/// Handle a request and produce a reply.
		networkOperation nextOperation();

	private:
		enum State	{
			NEW,
			CONNECTED
		};
		State		state_;
		std::string	buffer;
	};

} // namespace _SMERP

#endif // _ECHO_HANDLER_HPP_INCLUDED
