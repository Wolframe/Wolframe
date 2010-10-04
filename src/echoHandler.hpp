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
		echoConnection( const LocalTCPendpoint& local );
		echoConnection( const LocalSSLendpoint& local );
		~echoConnection();

		void setPeer( const RemoteTCPendpoint& remote );
		void setPeer( const RemoteSSLendpoint& remote );

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
		connectionHandler* newConnection( const LocalTCPendpoint& local );
		connectionHandler* newSSLconnection( const LocalSSLendpoint& local );
	};

} // namespace _SMERP

#endif // _ECHO_HANDLER_HPP_INCLUDED
