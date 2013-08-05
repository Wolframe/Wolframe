//
// luaHandler.hpp - wolframe main handler
//

#ifndef _Wolframe_HANDLER_HPP_INCLUDED
#define _Wolframe_HANDLER_HPP_INCLUDED

#include "system/connectionHandler.hpp"
#include "handlerConfig.hpp"

extern "C" {
	#include "lua.h"
}

namespace _Wolframe {
	/// The connection handler
	class luaConnection : public net::ConnectionHandler
	{
	public:
		luaConnection( const net::LocalEndpoint& local, const luaConfig config );

		~luaConnection();

		void setPeer( const net::RemoteEndpoint& remote );

		void networkInput( const void *begin, std::size_t bytesTransferred );
		void signalOccured( NetworkSignal );

		/// Handle a request and produce a reply.
		const net::NetworkOperation nextOperation();

	private:
		lua_State *l;
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
		ServerHandlerImpl( const HandlerConfiguration *config );

		net::ConnectionHandler* newConnection( const net::LocalEndpoint& local );

	private:
		luaConfig config_;
	};

} // namespace _Wolframe

#endif // _Wolframe_HANDLER_HPP_INCLUDED
