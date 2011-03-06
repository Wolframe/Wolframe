//
// pechoHandler.hpp - simple echo handler example with protocol
//

#ifndef _Wolframe_PROTOCOL_ECHO_HANDLER_HPP_INCLUDED
#define _Wolframe_PROTOCOL_ECHO_HANDLER_HPP_INCLUDED
#include "connectionHandler.hpp"


namespace _Wolframe {
namespace pecho {

   /// The connection handler
   class Connection : public Network::connectionHandler
   {
   public:
      typedef Network::NetworkOperation Operation;

      Connection( const Network::LocalTCPendpoint& local, unsigned int inputBufferSize=128, unsigned int outputBufferSize=128);
#ifdef WITH_SSL
      Connection( const Network::LocalSSLendpoint& local);
#endif // WITH_SSL
      virtual ~Connection();

      virtual void setPeer( const Network::RemoteTCPendpoint& remote);
#ifdef WITH_SSL
      virtual void setPeer( const Network::RemoteSSLendpoint& remote);
#endif // WITH_SSL

      /// Handle a request and produce a reply.
      virtual const Operation nextOperation();
      virtual void networkInput( const void *begin, std::size_t bytesTransferred);

      virtual void timeoutOccured();
      virtual void signalOccured();
      virtual void errorOccured( NetworkSignal);

   public:
      struct Private;
   private:
      Private* data;
   };

} // namespace pecho

/// The server handler container
   class ServerHandler::ServerHandlerImpl
   {
   public:
      Network::connectionHandler* newConnection( const Network::LocalTCPendpoint& local);
#ifdef WITH_SSL
      Network::connectionHandler* newSSLconnection( const Network::LocalSSLendpoint& local);
#endif // WITH_SSL
   };

} // namespace _Wolframe

#endif // _Wolframe_XMLSELECT_HANDLER_HPP_INCLUDED
