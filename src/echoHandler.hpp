//
// echoHandler.hpp - simple echo handler example
//

#ifndef _ECHO_HANDLER_HPP_INCLUDED
#define _ECHO_HANDLER_HPP_INCLUDED

#include "connectionHandler.hpp"

namespace _SMERP {
	/// The connection handler
	class echoConnection : public connectionHandler
	{
	public:
		echoConnection( const TCPendpoint& local );
		echoConnection( const SSLendpoint& local );
		~echoConnection();

		void setPeer( const TCPendpoint& remote );
		void setPeer( const SSLendpoint& remote );

		/// Parse incoming data. The return value indicates how much of the
		/// input has been consumed.
		void* parseInput( const void *begin, std::size_t bytesTransferred );

		/// Handle a request and produce a reply.
		NetworkOperation nextOperation();
	private:
		enum State	{
			NEW,
			READING,
			ANSWERING,
			FINISHING,
			TERMINATING
		};
		State		state_;
		std::string	buffer;
	};


	/// The server handler container
	class echoServer : public ServerHandler
	{
	public:
		connectionHandler* newConnection( const TCPendpoint& local );
		connectionHandler* newSSLconnection( const SSLendpoint& local );
	};

} // namespace _SMERP

#endif // _ECHO_HANDLER_HPP_INCLUDED
