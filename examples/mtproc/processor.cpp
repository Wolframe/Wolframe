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
   typedef protocol::InputBlock Input;                          //< input buffer type
   typedef protocol::OutputBlock Output;                        //< output buffer type
   typedef protocol::InputBlock::const_iterator InputIterator;  //< iterator type for protocol commands

   //* typedefs for input output buffers
   enum {BufferSize=256};
   typedef protocol::Buffer<BufferSize> LineBuffer;             //< buffer for one line of input/output
   typedef protocol::CmdBuffer CmdBuffer;                       //< buffer for protocol commands 
   typedef protocol::CArgBuffer <LineBuffer> ArgBuffer;         //< buffer for argc,argv argument parsing
   typedef protocol::CmdParser<CmdBuffer> ProtocolParser;       //< parser for the protocol

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
      ProcessingEoD,
      HandleError,
      Terminate
   };
   static const char* stateName( State i)
   {
      static const char* ar[] =
      {
         "Init","EnterCommand","ParseArguments","StartProcessing","ProcessingAfterWrite","Processing","ProcessingEoD","HandleError","Terminate"
      };
      return ar[i];
   };

   //* typedefs for the parser of the protocol
   //1. negotiation phase
   typedef protocol::CmdParser<CmdBuffer> ProtocolParser;
   //2. parser of the commands with interface to processor method execution

   //* all state variables of this processor
   //1. states
   State state;                               //< state of the processor
   CommandHandler commandHandler;
   
   //2. buffers and context
   CmdBuffer cmdBuffer;                       //< context (sub state) for partly parsed protocol commands
   LineBuffer writeLineBuffer;                //< context (sub state) for partly parsed input lines 
   LineBuffer readLineBuffer;                 //< context (sub state) for partly parsed input lines 
   ArgBuffer argBuffer;                       //< context (sub state) the list of arguments (array structure for the elements in buffer)
   InputState inputState;                     //< state of read buffer (feedInput) in detecting end of content
   Input input;                               //< buffer for READ network messages 
   Output output;                             //< buffer for WRITE network messages
   //3. Iterators
   ProtocolIterator itr;                      //< iterator to scan protocol commands
   ContentIterator src;                       //< iterator to scan protocol content terminated with (CR)LF dor (CR)LF

   //* helper methods for I/O
   //helper function to send a line message with CRLF termination as C string
   Operation WriteLine( const char* fmt, ...)
   {
      unsigned int ii;
      writeLineBuffer.init();
      va_list al;
      va_start( al, fmt);
      char line[ LineBuffer::Size];
      unsigned int len = vsnprintf( line, LineBuffer::Size, fmt, al);
      if (len > LineBuffer::Size) len = LineBuffer::Size-3;
      line[ len] = 0;
      writeLineBuffer.append( line);
      writeLineBuffer.push_back( '\r');
      writeLineBuffer.push_back( '\n');
      const char* msg = writeLineBuffer.c_str();
      return Operation( Operation::WRITE, msg, writeLineBuffer.size());
   };
   
public:
   Private( unsigned int inputBufferSize, unsigned int outputBufferSize)   :state(Init),argBuffer(&readLineBuffer),input(inputBufferSize),output(outputBufferSize),inputState(SRC)
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
                  readLineBuffer.init();
                  writeLineBuffer.init();
                  state = EnterCommand;
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
                        return WriteLine( "OK %s", buf.c_str());
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
                  continue;
               }
               
               case Processing:
               {
                  int rt = commandHandler.call( argBuffer.argc(), argBuffer.argv());                  
                  if (rt == 0)
                  {
                      
                     if (commandHandler.hasTerminated())
                     {
                        state = Init;
                        continue;
                     }
                     else
                     {
                        void* content = output.ptr();
                        std::size_t size = output.pos();
                        return Operation( Operation::WRITE, content, size);                
                     }
                  }
                  else
                  {
                     state = Terminate;
                     return WriteLine( "ERR ", rt);                 
                  }
               }

               case ProcessingAfterWrite:
               {
                  //do processing but first release the output buffer content that has been written in the processing state:
                  state = Processing;
                  output.release();
                  continue;
               }

               case HandleError:
               {
                                                                   //in the error case of non content processing, start again after complaining (Operation::WRITE sent in previous state):
                  ProtocolParser::getLine( itr, readLineBuffer);   //< parse the rest of the line to clean the input for the next command
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

   void feedInput( const char* buf, unsigned int bufsize)
   {
      if (bufsize > data->input.size()) bufsize = data->input.size();
      if (bufsize > 0) memmove( data->input.charptr(), buf, bufsize);

      if (withEOF && bufsize < data->input.size())
      {
         data->input.charptr()[ bufsize] = 0;
         data->input.setPos( ++bufsize);
      }
      else
      {
         data->input.setPos( bufsize);
      }
      itr = data->input.begin();
      return bufsize;
   };

   unsigned int parseInput( const char* buf, unsigned int bufsize)
   {
      data->input.setPos( 0);
      if (bufsize > data->input.size()) bufsize = data->input.size();
      if (bufsize == 0) return 0;

      unsigned int eatsize = 0;
      unsigned int ii = 0;

      while (ii<bufsize)
      {
         if (state == SRC)
         {
            char* cc = memchr( buf+ii, '\n', bufsize-ii);
            if (cc)
            {
               ii = cc - buf;
               state = LF;
            }
            else
            {
               ii = bufsize;
               feedInput( buf+eatsize, ii-eatsize);
            }
         }
         else if (state == LF)
         {
            if (buf[ii] == '.')
            {
               state = LF_DOT;
               feedInput( buf+eatsize, ii-eatsize);
               ii++;
               eatsize=ii;
            } 
         }
         else if (state == LF_DOT)
         {
            if (buf[ii] == '\r')
            {
               feedInput( "", 1); //< feed EOD
               state = LF_DOT_CR;
               ii++; 
            }
            else if (buf[ii] == '\n')
            {
               feedInput( "", 1); //< feed EOD
               state = LF_DOT_CR_LF;
               ii++;
            }
            else
            {
               state = SRC;
            }
         else if (state == LF_DOT_CR)
         {
            if (buf[ii] == '\n')
            {
               ii++; 
            }
            state = LF_DOT_CR_LF;
         }
      }
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
   std::size_t nn = data->parseInput( (const char*)bytes, nofBytes);
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

