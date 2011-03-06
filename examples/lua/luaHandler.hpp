//
// luaHandler.hpp - wolframe main handler
//

#ifndef _Wolframe_HANDLER_HPP_INCLUDED
#define _Wolframe_HANDLER_HPP_INCLUDED

#include "connectionHandler.hpp"
#include "handlerConfig.hpp"

extern "C" {
	#include <lua.h>
}

namespace _Wolframe {
	class luaConfig {
	public:
		std::string script;
		bool debug;
	};

	/// The connection handler
	class luaConnection : public Network::connectionHandler
	{
	public:
		luaConnection( const Network::LocalTCPendpoint& local, const luaConfig config );
#ifdef WITH_SSL
		luaConnection( const Network::LocalSSLendpoint& local, const luaConfig config );
#endif // WITH_SSL
		~luaConnection();

		void setPeer( const Network::RemoteTCPendpoint& remote );
#ifdef WITH_SSL
		void setPeer( const Network::RemoteSSLendpoint& remote );
#endif // WITH_SSL
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
		const luaConfig config;
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
		ServerHandlerImpl( const HandlerConfiguration *config ) {
			config_.script = config->luaConfig->script;
			config_.debug = config->luaConfig->debug;
		}

		Network::connectionHandler* newConnection( const Network::LocalTCPendpoint& local );
#ifdef WITH_SSL
		Network::connectionHandler* newSSLconnection( const Network::LocalSSLendpoint& local );
#endif // WITH_SSL

	private:
		luaConfig config_;
	};

} // namespace _Wolframe

#endif // _Wolframe_HANDLER_HPP_INCLUDED
