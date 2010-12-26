//
// xmlselectHandler.hpp - simple echo handler example
//

#ifndef _SMERP_XMLSELECT_HANDLER_HPP_INCLUDED
#define _SMERP_XMLSELECT_HANDLER_HPP_INCLUDED

#include "connectionHandler.hpp"

namespace _SMERP {
namespace xmlselect {

	/// The connection handler
	class Connection : public Network::connectionHandler
	{
	public:
      typedef Network::NetworkOperation Operation;

		Connection( const Network::LocalTCPendpoint& local );
		Connection( const Network::LocalSSLendpoint& local );
		~Connection();

		void setPeer( const Network::RemoteTCPendpoint& remote );
		void setPeer( const Network::RemoteSSLendpoint& remote );

		/// Handle a request and produce a reply.
		Operation nextOperation();
      virtual void* parseInput( const void *begin, std::size_t bytesTransferred );

   public:
      enum {MemBlockSize=4096};
      struct Private;
	private:
      Private* data;
	};

} // namespace xmlselect

	/// The server handler container
	class ServerHandler::ServerHandlerImpl
	{
	public:
		Network::connectionHandler* newConnection( const Network::LocalTCPendpoint& local );
		Network::connectionHandler* newSSLconnection( const Network::LocalSSLendpoint& local );
	};

} // namespace _SMERP

#endif // _SMERP_XMLSELECT_HANDLER_HPP_INCLUDED
