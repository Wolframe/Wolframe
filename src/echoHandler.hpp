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
		echoConnection( const connectionPeer& local );
		echoConnection( const SSLconnectionPeer& local );
		~echoConnection();

		void setPeer( const connectionPeer& remote );
		void setPeer( const SSLconnectionPeer& remote );

		/// Parse incoming data. The return value indicates how much of the
		/// input has been consumed.
		char *parseInput( char *begin, std::size_t bytesTransferred );

		/// Handle a request and produce a reply.
		networkOperation nextOperation();
	private:
		enum State	{
			NEW,
			READING,
			ANSWERING
		};
		State		state_;
		std::string	buffer;
	};


	/// The server handler container
	class echoServer : public ServerHandler
	{
	public:
		connectionHandler* newConnection( const connectionPeer& local );
		connectionHandler* newSSLconnection( const SSLconnectionPeer& local );
	};

} // namespace _SMERP

#endif // _ECHO_HANDLER_HPP_INCLUDED
