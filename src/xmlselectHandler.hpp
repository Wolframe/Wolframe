//
// textwolfHandler.hpp - simple echo handler example
//

#ifndef _XMLSELECT_HANDLER_HPP_INCLUDED
#define _XMLSELECT_HANDLER_HPP_INCLUDED

#include "connectionHandler.hpp"

namespace _SMERP {
namespace xmlselect {

	/// The connection handler
	class Connection : public Network::connectionHandler
	{
	public:
		Connection( const Network::LocalTCPendpoint& local );
		Connection( const Network::LocalSSLendpoint& local );
		~Connection();

		void setPeer( const Network::RemoteTCPendpoint& remote );
		void setPeer( const Network::RemoteSSLendpoint& remote );

		/// Handle a request and produce a reply.
		Network::NetworkOperation nextOperation();
      virtual void* parseInput( const void *begin, std::size_t bytesTransferred );

   public:
      enum {MemBlockSize=4096};
      struct Private;
	private:
      Private* data;
	};


	/// The server handler container
	class Server : public Network::ServerHandler
	{
	public:
		Network::connectionHandler* newConnection( const Network::LocalTCPendpoint& local );
		Network::connectionHandler* newSSLconnection( const Network::LocalSSLendpoint& local );
	};

} // namespace xmlselect
} // namespace _SMERP

#endif // _ECHO_HANDLER_HPP_INCLUDED
