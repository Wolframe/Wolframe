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
      Processing,
      ProcessingAfterWrite,
      HandleError,
      Terminate
   };
   static const char* stateName( State i)
   {
      static const char* ar[] =
      {
         "Init",
         "EnterCommand",
         "ParseArguments",
         "Processing",
         "ProcessingAfterWrite",
         "HandleError",
         "Terminate"
      };
      return ar[i];
   };

   enum InputState
   {
      SRC,
      LF,
      LF_DOT,
      LF_DOT_CR,
      LF_DOT_CR_LF
   };

   //* typedefs for the parser of the protocol
   //1. negotiation phase
   typedef protocol::CmdParser<CmdBuffer> ProtocolParser;
   //2. parser of the commands with interface to processor method execution
   struct CommandHandler
   {
   private:
      ProtocolParser m_parser;
      Instance* m_instance;
      Method::Context m_context;
      unsigned int m_methodIdx;
      bool m_terminated;
      
      void resetCommand()
      {
         m_methodIdx = 0;
         m_terminated = true;
         m_context.init( m_instance?m_instance->data,0);
      };
      
      void init( const char** protocolCmds, Instance* instance)
      {
         if (m_instance) delete m_instance;
         m_instance = instance;
         parser.init();
         resetCommand();
         
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

      void getGeneratorMem( Input& input, Input::const_iterator& itr)
      {
         if (m_context.contentIterator)
         {
            void* block;
            unsigned int blocksize;
            m_context.contentIterator->getRestBlock( &block, &blocksize);
            if (blocksize > input.size())
            {
               LOG_ERROR << "cannot buffer rest of processor input block: " << blocksize << " bytes";
               blocksize = input.size();
            }
            if (input.charptr()+input.pos() == (char*)block+blocksize)
            {
               //... memory block was not copied by the processor method.
               //    So we just move our iterator to the end of the block processed by the method.
               itr = input.at( input.pos()-blocksize);
            }
            else
            {
               //... memory block is not the original, so we have to move it into the input buffer
               memmove( input.ptr(), block, blocksize);
               input.setPos( blocksize);
               itr = input.begin();
            }
         }
      };
      
      void feedGeneratorMem( Input& input, Input::const_iterator& itr)
      {
         if (m_context.contentIterator && input.pos()>itr.pos())
         {
            m_context.contentIterator->feed( input.charptr()+itr.pos(), input.pos()-itr.pos());
         }
      };
      
   public:   
      enum Command {unknown=-1, empty=0, caps, quit, method};      
      void init( Instance* instance)
      {
         static const char* cmd[4] = {"","caps","quit", 0};
         init( cmd, instance);
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
         resetCommand();
         
         int ci = m_parser.getCommand();
         if (ci >= unknown && ci < method)
         {
            return (Command)ci;
         }
         else
         {
            m_methodIdx = (unsigned int)ci - (unsigned int)method;
            return method;
         }
         return rt;
      };
      
      unsigned int call( int argc, const char** argv)
      {
         if (m_context.contentIterator && m_context.contentIterator->state == protocol::Generator::EndOfInput)
         {
            feedGeneratorMem( input, itr)
            m_context.contentIterator->state = protocol::Generator::Processing;
         }
         LOG_DEBUG << "call of '" << m_instance->vmt[ m_methodIdx].name << "'";

         unsigned int rt = m_instance->vmt[ m_methodIdx].call( &m_context, argc, argv);
         if (rt != 0)
         {
            LOG_ERROR << "error " << rt << " calling '" << m_instance->vmt[ m_methodIdx].name << "'";
            getGeneratorMem( input, itr)
            resetCommand();
         }
         else
         {
            switch (m_context.contentIterator->state)
            {
               case protocol::Generator::Init:
                  m_context.contentIterator->state = protocol::Generator::Processing;
               case protocol::Generator::Processing:
                  break;
               case protocol::Generator::Error:
               case protocol::Generator::EndOfInput:
                  getGeneratorMem( input, itr)
                  resetCommand();
                  break;
               case protocol::Generator::EndOfBuffer:
                  throw InputBlock::End;
            }
         }
         return rt;
      };

      bool hasTerminated() const
      {
         return m_terminated;
      };
   };

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

   void feedInput( const char* buf, unsigned int bufsize, bool withEOF=false)
   {
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
      if (bufsize > data->input.size())
      {
         bufsize = data->input.size();
      }
      unsigned int eatsize = 0;
      unsigned int ii = 0;

      if (ii == bufsize)
      {
         feedInput( buf, bufsize);
         return eatsize;
      }
      if (state == LF && buf[ii] == '.')
      {
         state = LF_DOT;
         goto state_LF_DOT;
      }
      goto state_SRC:

      state_LF_DOT:
      if (buf[ii] == '.')
      {
         state = LF_DOT;
         goto state_LF_DOT;
      }
      goto state_SRC:
      

      else if (state == LF_DOT) && buf[0] == '\r')
      {
         state = LF_DOT_CR;
         return 1;
      }
      else if (state == LF_DOT_CR && buf[0] == '\n')
      {
         state = LF_DOT_CR_LF;
         return 1;
      }
      
      if (buf[ii] ==  '\n')
         {
            feedInput( buf, eatsize, true);            
            state = LF_DOT_CR_LF;
            return ii+1;
         }
         state = SRC;               
         feedInput( buf, eatsize);
         return ii+1;
      }
      if (state == LF_DOT_CR)
      {
         if (bufsize > ii)
         {
            if (buf[ii] ==  '\n')
            {
               eatsize = ++ii; 
               state = LF_DOT_CR_LF;
            }
            else
            {
               state = SRC;               
               feedInput( buf, ii);
               return ii+1;
            }
         }
      }
      
      {
         case LF:           if (bufsize > ii) if (buf[ii] ==  '.') {eatsize = ++ii; state = LF_DOT;} else {state=SRC; break;
         case LF_DOT:       if (bufsize > ii && buf[ii] == '\r') {eatsize = ++ii; state = LF_DOT_CR;}
         case LF_DOT_CR:    if (bufsize > ii && buf[ii] == '\n') {eatsize = ++ii; state = LF_DOT_CR_LF;}
         case LF_DOT_CR_LF: break;
         case SRC:          break;
      }
      if (state != SRC)
      {
         
      }
      else
      {
         char* cc = memchr( buf, '\n', bufsize);
         while (cc && state == SRC)
         {
            ii = cc - buf;
            if (ii == bufsize-2)
            {
               inputState = CR_LF_DOT;
               eatsize = ii+2;
               bufsize = ii;
            }
            else if (buf[ ii+2] == '\r')
            {
               inputState = CR_LF_DOT_CR;
               eatsize = ii+3;
               bufsize = ii;
            }
            else if (buf[ ii+2] == '\n')
            {
               inputState = CR_LF_DOT_CR_LF;
               eatsize = ii+3;
               bufsize = ii;
            }
            cc = strchr( buf+ii+1, bufsize-ii-1, '\n');
         }
         if (state == SRC && buf[bufsize-1] == '\n')
         {
            inputState = CR_LF;
            eatsize = bufsize;
            bufsize = bufsize-1;
         }
      }
      memmove( data->input.charptr(), buf, bufsize);
      if (bufsize > 0)
      {
         if (state >= CR_LF_DOT_CR_LF)
         {
            if (bufsize < data->input.size())
            {
               data->input.charptr()[ bufsize] = '\0';
               bufsize++;
               state = SRC;
            }
         }
      }
      data->input.setPos( bufsize);
      itr = data->input.begin();
      return eatsize;
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

