//
// textwolfHandler.hpp - simple echo handler example
//

#ifndef _XMLSELECT_HANDLER_HPP_INCLUDED
#define _XMLSELECT_HANDLER_HPP_INCLUDED

#include "connectionHandler.hpp"

namespace _SMERP {
namespace xmlselect {
  
	/// The connection handler
	class Connection : public connectionHandler
	{
	public:
		Connection( const LocalTCPendpoint& local );
		Connection( const LocalSSLendpoint& local );
		~Connection();

		void setPeer( const RemoteTCPendpoint& remote );
		void setPeer( const RemoteSSLendpoint& remote );

		/// Handle a request and produce a reply.
		NetworkOperation nextOperation();
      virtual void* parseInput( const void *begin, std::size_t bytesTransferred );
      
   public:
      enum {MemBlockSize=4096};
      struct Private;
	private:
      Private* data;
	};


	/// The server handler container
	class Server : public ServerHandler
	{
	public:
		connectionHandler* newConnection( const LocalTCPendpoint& local );
		connectionHandler* newSSLconnection( const LocalSSLendpoint& local );
	};

} // namespace xmlselect
} // namespace _SMERP

#endif // _ECHO_HANDLER_HPP_INCLUDED
