//
// smerpHandler.hpp - smerp main handler
//

#ifndef _SMERP_HANDLER_HPP_INCLUDED
#define _SMERP_HANDLER_HPP_INCLUDED

#include "connectionHandler.hpp"

extern "C" {
	#include <lua.h>
}

namespace _SMERP {
	/// The connection handler
	class echoConnection : public Network::connectionHandler
	{
	public:
		echoConnection( const Network::LocalTCPendpoint& local );
		echoConnection( const Network::LocalSSLendpoint& local );
		~echoConnection();
		
		void setPeer( const Network::RemoteTCPendpoint& remote );
		void setPeer( const Network::RemoteSSLendpoint& remote );

		/// Parse incoming data. The return value indicates how much of the
		/// input has been consumed.
		void* parseInput( const void *begin, std::size_t bytesTransferred );

		void timeoutOccured();
		void signalOccured();

		/// Handle a request and produce a reply.
		Network::NetworkOperation nextOperation();

	private:
		enum State	{
			NEW,
			HELLO,
			READING,
			ANSWERING,
			FINISHING,
			TIMEOUT,
			SIGNALLED,
			TERMINATING
		};

		State		state_;
		std::string	buffer_;
		
	private:
		lua_State *l;

		void createVM( );
		void destroyVM( );
		void printMemStats( );
	};

	/// The server handler container
	class ServerHandler::ServerHandlerImpl
	{		
	public:
		Network::connectionHandler* newConnection( const Network::LocalTCPendpoint& local );
		Network::connectionHandler* newSSLconnection( const Network::LocalSSLendpoint& local );
	};

} // namespace _SMERP

#endif // _SMERP_HANDLER_HPP_INCLUDED
