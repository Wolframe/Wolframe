#ifndef _SMERP_METHODTABLE_PROCESSOR_HANDLER_HPP_INCLUDED
#define _SMERP_METHODTABLE_PROCESSOR_HANDLER_HPP_INCLUDED
#include "protocol/generator.hpp"
#include "connectionHandler.hpp"

namespace _SMERP {
namespace mtproc {

//method of the processor
struct Method
{
   //defined by the processor instance 
   struct Data;
   
   struct Context
   {
      Data* data;
      protocol::Generator* contentIterator;
      
      Context()                   :data(0),contentIterator(0){};
      void init( Data* p_data=0)  {data(p_data),contentIterator(0){};
   };
   //Method call
   //@return 0, in case of success, errorcode for client in case of error
   typedef (unsigned int*) Call( Context* context, unsigned int argc, const char* argv);
   
   const char* name;
   Call call;
};

//current instance with data of the processor
struct Instance
{
   const Method* mt;
   Method::Data* data;

   Instance()                    :mt(0),data(0){};
   Instance( const Instance& o)  :mt(o.mt),data(o.data){};
};


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

