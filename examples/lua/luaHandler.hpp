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
		char *data_start;
		std::size_t data_size;
		std::string out_buf;
		unsigned idle_timeout;

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
