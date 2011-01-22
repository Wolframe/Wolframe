#include "processor.hpp"
#include "protocol.hpp"
#include "logger.hpp"

using namespace _SMERP;
using namespace _SMERP::pecho;

struct Connection::Private
{
private:   
   protocol::Parser cmdParser;

   //* typedefs for input output blocks and input iterators
   typedef protocol::InputBlock Input;                                      //< input buffer type
   typedef protocol::OutputBlock Output;                                    //< output buffer type
   typedef protocol::InputBlock::const_iterator ProtocolIterator;           //< iterator type for protocol commands
   typedef protocol::TextIterator<Input::const_iterator> ContentIterator;   //< iterator type for content   

   //* typedefs for input output buffers
   enum {BufferSize=256};
   typedef protocol::Buffer<BufferSize> LineBuffer;                         //< buffer for one line of input/output
   typedef protocol::CmdBuffer CmdBuffer;                                   //< buffer for protocol commands 
   typedef protocol::CArgBuffer <LineBuffer> ArgBuffer;

   //* typedefs for state variables and buffers
   //list of processor states
   enum State
   {
      Init,
      EnterCommand,
      ParseArguments,
      StartProcessing,
      ProcessingAfterWrite,
      Processing,
      HandleError,
      Terminate
   };
   static const char* stateName( State i)
   {
      static const char* ar[] = {"Init","EnterCommand","ParseArguments","StartProcessing","ProcessingAfterWrite","Processing","HandleError","Terminate"};
      return ar[i];
   };

   //* typedefs for the parser of the protocol
   //1. negotiation phase
   typedef protocol::CmdParser<CmdBuffer> ProtocolParser;
   //2. parser of the commands (state EnterCommand)
   struct CommandHandler
   {
   private:
      ProtocolParser m_parser;
      Instance* m_instance;
      int m_parsedCommand;
      Method::Context m_context;
      
      void init( const char** protocolCmds, Instance* instance)
      {
         if (m_instance) delete m_instance;
         m_instance = instance;
         parser.init();
         m_parsedCommand = -1;
         m_context.init();

         if (protocolCmds)
         {
            for( unsigned int ii=0; protocolCmds[ ii]; ii++)
            {
               parser.add( protocolCmds[ ii]);
            }
         }
         if (instance && instance->vmt)
         {
            m_context.data = instance->data;
            for( unsigned int ii=0; instance->vmt[ii].call && instance->vmt[ii].name; ii++)
            {
               parser.add( instance->vmt[ii].name);
            }
         }
      };

   public:   
      enum Command {unknown=-1, empty=0, caps, quit, method};

      void init( Instance* instance)
      {
         static const char* cmd[4] = {"","caps","quit",0};
         init( cmd, instance);
      };

      template <class Buffer>
      void writeCaps( Buffer& buf)
      {
         unsigned int ii;
         if (m_instance && m_instance->vmt)
         {
            for( unsigned int ii=0; instance->vmt[ii].call && instance->vmt[ii].name; ii++)
            {
               if (ii>0)
               {
                  buf.push_back( ',');
                  buf.push_back( ' ');
               }
               buf.append( instance->vmt[ii].name);
            }
         }
      };
      
      Command getCommand()
      {
         Command rt;
         m_context.init( m_instance->data);
         
         m_parsedCommand = (int)m_parser.getCommand();
         if (parserCommand >= (int)unknown && parserCommand < (int)method)
         {
            rt = (Command)parserCommand;
            m_parsedCommand = -1;
         }
         else
         {
            m_parsedCommand -= (int)method;
            rt = method;
         }
         return rt;
      };
      
      int call( int argc, const char** argv)
      {
         if (m_parsedCommand < 0) return false;
         unsigned int rt = m_instance->vmt[ m_parsedCommand].call( &m_context, argc, argv);
         if (rt != 0)
         {
            LOG_ERROR << "error " << rt << " calling '" << m_instance->vmt[ m_parsedCommand].name << "'";
         }
         else
         {
            LOG_DEBUG << "call of '" << m_instance->vmt[ m_parsedCommand].name << "'";
         }
         return rt;
      };
      
      CommandHandler( Instance* instance)
      {
         init( instance);
      };

      CommandHandler()
      {
         init( 0);
      };

      ~CommandHandler()
      {
         if (m_instance) delete m_instance;
      };
   };

   //* all state variables of this processor
   //1. states
   State state;                               //< state of the processor
   CommandHandler commandHandler;
   unsigned int command;                      //< command of commandHandler parsed in the state EnterCommand
   
   //2. buffers and context
   CmdBuffer cmdBuffer;                       //< context (sub state) for partly parsed protocol commands
   LineBuffer buffer;                         //< context (sub state) for partly parsed input lines 
   ArgBuffer argBuffer;                       //< context (sub state) the list of arguments (array structure for the elements in buffer)
   Input input;                               //< buffer for READ network messages 
   Output output;                             //< buffer for WRITE network messages
   //3. Iterators
   ProtocolIterator itr;                      //< iterator to scan protocol commands
   ContentIterator src;                       //< iterator to scan protocol content terminated with (CR)LF dor (CR)LF


   //TODO do less code for obvious things. try to use iostreams with the buffers

   //* helper methods for I/O
   //helper function to send a line message with CRLF termination as C string
   Operation WriteLine( const char* str, const char* arg=0)
   {
      unsigned int ii;
      buffer.init();
      if (str) for (ii=0; str[ii]; ii++) buffer.push_back( str[ii]);
      if (arg) for (ii=0; arg[ii]; ii++) buffer.push_back( arg[ii]);
      buffer.push_back( '\r');
      buffer.push_back( '\n');
      const char* msg = buffer.c_str();
      buffer.init();
      return Operation( Operation::WRITE, msg, ii+2);
   };
   
   Operation WriteLine( const char* str, unsigned int errorcode)
   {
      enum {BufSize=32};
      char buf[BufSize];
      unsigned int ii=BufSize;
      buf[ --ii] = 0;
      do
      {
         buf[ --ii] = errorcode % 10;
         errorcode /= 10;
      }
      while (errorcode != 0);
      
      return WriteLine( str, buf+ii);
   };
   
public:
   Private( unsigned int inputBufferSize, unsigned int outputBufferSize)   :state(Init),argBuffer(&buffer),input(inputBufferSize),output(outputBufferSize)
   {
      itr = input.begin();
      src = &itr;
   };
   ~Private()  {};

   //statemachine of the processor
   Operation nextOperation()
   {
      try
      {
         for (;;)
         {
            switch( state)
            {
               case Init:
               {
                  //start or restart:
                  state = EnterCommand;
                  mode = Ident;
                  return WriteLine( "OK expecting command");
               }
               
               case EnterCommand:
               {
                  switch (commandHandler.getCommand( itr, cmdBuffer))
                  {
                     case CommandHandler::empty:
                     {
                        state = EnterCommand;
                        ProtocolParser::consumeEOLN( itr); //< consume the end of line for not getting into an endless loop with empty command
                        continue;
                     }   
                     case CommandHandler::caps: 
                     {
                        state = EnterCommand;
                        LineBuffer buf;
                        commandHandler.writeCaps( buf)
                        return WriteLine( "OK ", buf.c_str());
                     }
                     case CommandHandler::method: 
                     {
                        state = ParseArguments;  
                        continue;
                     }
                     case CommandHandler::quit:
                     {
                        state = Terminate;
                        return WriteLine( "BYE");
                     }
                     default:
                     {
                        state = HandleError;
                        return WriteLine( "BAD unknown command");                 
                     }
                  }
               }

               case ParseArguments:
               {
                  ProtocolParser::getLine( itr, argBuffer);
                  state = StartProcessing;
                  ProtocolParser::consumeEOLN( itr);
                  state = StartProcessing;
                  continue;
               }
               
               case StartProcessing:
               {
                  ProtocolParser::getLine( itr, argBuffer);
                  if (rt == 0)
                  {
                     state = Processing;
                     ProtocolParser::consumeEOLN( itr);
                     continue;
                  }
                  else
                  {
                     state = HandleError;                     
                     return WriteLine( "ERR ", rt);                 
                  }
               }

//TODO continue here

               case ProcessingAfterWrite:
               {
                  //do processing but first release the output buffer content that has been written in the processing state:
                  state = Processing;
                  output.release();
                  continue;
               }

               case Processing:
               {
                  void* content = output.ptr();
                  std::size_t size = output.pos();
                  return Operation( Operation::WRITE, content, size);                
               }

               case HandleError:
               {
                  //in the error case, start again after complaining (Operation::WRITE sent in previous state):
                  ProtocolParser::getLine( itr, buffer);   //< parse the rest of the line to clean the input for the next command
                  ProtocolParser::consumeEOLN( itr);
                  state = Init;
                  continue;
               }

               case Terminate:
               {
                  state = Terminate;
                  return Operation( Operation::TERMINATE);                      
               }
            }//switch(..)
         }//for(,,)
      }
      catch (Input::End)
      {
         LOG_DATA << "End of input interrupt";
         input.setPos( 0);
         return Operation( Operation::READ, input.ptr(), input.size());
      };
      return Operation( Operation::TERMINATE);
   };

   unsigned int feedInput( const char* buf, unsigned int bufsize)
   {
      if (bufsize > data->input.size())
      {
         bufsize = data->input.size();
      }
      data->input.setPos( bufsize);
      memcpy( data->input.charptr(), buf, bufsize);
   };

   NetworkOperation nextOperation()
   {
   };

   void timeoutOccured()
   {
   };

   void signalOccured()
   {
   };

   void errorOccured( NetworkError)
   {
   };
};


Connection::Connection( const Network::LocalTCPendpoint& local, unsigned int inputBufferSize, unsigned int outputBufferSize)
{
   data = new Private( inputBufferSize, outputBufferSize);
}

Connection::Connection( const Network::LocalSSLendpoint& local)
{
   data = new Private(4096,4096);
}

Connection::~Connection()
{
   delete data;
}

void Connection::setPeer( const Network::RemoteTCPendpoint& remote)
{
   data->newConnection( remote.host.c_str(), remote.port, remote.toString.c_str());
}

void Connection::setPeer( const Network::RemoteSSLendpoint& remote)
{
   data->newConnection( remote.host.c_str(), remote.port, remote.toString.c_str());
}

void* Connection::parseInput( const void* bytes, std::size_t nofBytes)
{
   std::size_t nn = data->feedInput( (const char*)bytes, nofBytes);
   return (void*)(data->input.charptr() + nn);
}

Connection::Operation Connection::nextOperation()
{
   return data->nextOperation();
}

void Connection::timeoutOccured()
{
   data->timeoutOccured();
};

void Connection::signalOccured()
{
   data->signalOccured();
};

void Connection::errorOccured( NetworkError e)
{
   data->errorOccured( e);
};

void Connection::initInstance( Instance* instance)
{
   data->commandHandler.init( instance);
};

Network::connectionHandler* ServerHandler::ServerHandlerImpl::newConnection( const Network::LocalTCPendpoint& local )
{
   return new Connection( local );
}

Network::connectionHandler* ServerHandler::ServerHandlerImpl::newSSLconnection( const Network::LocalSSLendpoint& local )
{
   return new Connection( local );
}

ServerHandler::ServerHandler() : impl_( new ServerHandlerImpl ) {}

ServerHandler::~ServerHandler()  { delete impl_; }

Network::connectionHandler* ServerHandler::newConnection( const Network::LocalTCPendpoint& local )
{
   return impl_->newConnection( local );
}

Network::connectionHandler* ServerHandler::newSSLconnection( const Network::LocalSSLendpoint& local )
{
   return impl_->newSSLconnection( local );
}
