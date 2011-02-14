#ifndef _SMERP_METHODTABLE_PROCESSOR_HANDLER_HPP_INCLUDED
#define _SMERP_METHODTABLE_PROCESSOR_HANDLER_HPP_INCLUDED
#include "dispatcher.hpp"
#include "connectionHandler.hpp"

namespace _SMERP {
namespace mtproc {

/// The connection handler
class Connection : public Network::connectionHandler
{
   public:
      typedef Network::NetworkOperation Operation;
      
      Connection( const Network::LocalTCPendpoint& local, unsigned int inputBufferSize=128, unsigned int outputBufferSize=128);
      Connection( const Network::LocalSSLendpoint& local);
      virtual ~Connection();
      
      virtual void setPeer( const Network::RemoteTCPendpoint& remote);
      virtual void setPeer( const Network::RemoteSSLendpoint& remote);
      
      /// Handle a request and produce a reply.
      virtual const Operation nextOperation();
      virtual void networkInput( const void *begin, std::size_t bytesTransferred);
      
      virtual void timeoutOccured();
      virtual void signalOccured();
      virtual void errorOccured( NetworkSignal);
      
      void initInstance( Instance* instance);
      
   public:
      struct Private;
   private:
      Private* data;
};

}//namespace mtproc

/// The server handler container
class ServerHandler::ServerHandlerImpl
{
   public:
      Network::connectionHandler* newConnection( const Network::LocalTCPendpoint& local);
      Network::connectionHandler* newSSLconnection( const Network::LocalSSLendpoint& local);
};

}//namespace

