//
// luaHandler.hpp - simple LUA handler example
//

#ifndef _LUA_HANDLER_HPP_INCLUDED
#define _LUA_HANDLER_HPP_INCLUDED

#include "connectionHandler.hpp"

extern "C" {
#include <lua.h>
}

namespace _SMERP {
	/// The connection handler
	class luaConnection : public connectionHandler
	{
	public:
		luaConnection( const LocalTCPendpoint& local );
		luaConnection( const LocalSSLendpoint& local );
		~luaConnection();

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
	class luaServer : public ServerHandler
	{
	private:
		lua_State *l;

	public:
		luaServer( );
		virtual ~luaServer( );

		connectionHandler* newConnection( const LocalTCPendpoint& local );
		connectionHandler* newSSLconnection( const LocalSSLendpoint& local );
	};

} // namespace _SMERP

#endif // _LUA_HANDLER_HPP_INCLUDED
