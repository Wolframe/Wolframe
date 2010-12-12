//
// luaHandler.hpp - simple echo handler using Lua
//

#ifndef _LUA_HANDLER_HPP_INCLUDED
#define _LUA_HANDLER_HPP_INCLUDED

#include "connectionHandler.hpp"

extern "C" {
#include <lua.h>
}

namespace _SMERP {
	/// The connection handler
	class luaConnection : public Network::connectionHandler
	{
	public:
		luaConnection( const Network::LocalTCPendpoint& local );
		luaConnection( const Network::LocalSSLendpoint& local );
		~luaConnection();

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
	};


	/// The server handler container
	class luaServer : public Network::ServerHandler
	{
	private:
		lua_State *l;

	public:
		luaServer( );
		virtual ~luaServer( );
		Network::connectionHandler* newConnection( const Network::LocalTCPendpoint& local );
		Network::connectionHandler* newSSLconnection( const Network::LocalSSLendpoint& local );
	};

} // namespace _SMERP

#endif // _LUA_HANDLER_HPP_INCLUDED
