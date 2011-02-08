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

		void networkInput( const void *begin, std::size_t bytesTransferred );
		void timeoutOccured();
		void signalOccured();
		void errorOccured( NetworkSignal );

		/// Handle a request and produce a reply.
		const Network::NetworkOperation nextOperation();

	private:
		lua_State *l;
		int counter;
		int maxMemUsed;
		static const std::size_t buf_size = 8192;
		char buf[buf_size];

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
