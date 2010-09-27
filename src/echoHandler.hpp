//
// echoHandler.hpp - simple echo handler example
//

#ifndef _ECHO_HANDLER_HPP_INCLUDED
#define _ECHO_HANDLER_HPP_INCLUDED

#include "connectionHandler.hpp"

namespace _SMERP {
	/// The server handler container
	class echoServer : public ServerHandler
	{
	public:
		connectionHandler* newConnection()	{ return new echoConnection(); }
		connectionHandler* newSSLconnection()	{ return new echoConnection(); }
	};

	/// The connection handler
	class echoConnection : public connectionHandler
	{
	public:
		void setPeer( const connectionPeer& local, const connectionPeer& remote );
		void setPeer( const SSLconnectionPeer& local, const SSLconnectionPeer& remote );

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
