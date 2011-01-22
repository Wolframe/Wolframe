#ifndef _SMERP_VMTPROCESSOR_HANDLER_HPP_INCLUDED
#define _SMERP_VMTPROCESSOR_HANDLER_HPP_INCLUDED
#include "protocol/generator.hpp"
#include "connectionHandler.hpp"

namespace _SMERP {
namespace vmtproc {

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
   const Method* vmt;
   Method::Data* data;

   Instance()                    :vmt(0),data(0){};
   virtual ~Instance(){};

private:
   Instance( const Instance&){};
   void operator=( Instance&){};
};

//connection handler of the processor
class Connection : public Network::connectionHandler
{
   public:
      typedef Network::NetworkOperation Operation;
      
      Connection( const Network::LocalTCPendpoint& local, unsigned int inputBufferSize=128, unsigned int outputBufferSize=128);
      Connection( const Network::LocalSSLendpoint& local );
      ~Connection();
      
      virtual void setPeer( const Network::RemoteTCPendpoint& remote );
      virtual void setPeer( const Network::RemoteSSLendpoint& remote );
      
      /// Handle a request and produce a reply.
      virtual Operation nextOperation();
      virtual void* parseInput( const void *begin, std::size_t bytesTransferred );

      void initInstance( Instance* instance);
      
   public:
      struct Private;
   private:
      Private* data;
};

}//namespace vmtproc

/// The server handler container
class ServerHandler::ServerHandlerImpl
{
   public:
      Network::connectionHandler* newConnection( const Network::LocalTCPendpoint& local );
      Network::connectionHandler* newSSLconnection( const Network::LocalSSLendpoint& local );
};

}//namespace

