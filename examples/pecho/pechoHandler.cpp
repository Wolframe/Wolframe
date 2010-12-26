//
// pechoHandler.cpp
//
#include "protocol.hpp"
#include "pechoHandler.hpp"
#include "logger.hpp"

using namespace _SMERP;
using namespace _SMERP::pecho;

struct Connection::Private
{
   //* typedefs for input output blocks and input iterators
   typedef protocol::InputBlock Input;                                           //< input buffer type
   typedef protocol::OutputBlock Output;                                         //< output buffer type
   typedef protocol::InputBlock::iterator ProtocolIterator;                      //< iterator type for protocol commands
   typedef protocol::TextIterator<Input::iterator> ContentIterator;              //< iterator type for content

   //* typedefs for input output buffers
   typedef protocol::Buffer<128> LineBuffer;                                     //< buffer for one line of input/output
   typedef protocol::CmdBuffer CmdBuffer;
   
   //* typedefs for state variables and buffers
   //list of processor states
   enum State {Init,EnterCommand,EmptyLine,EnterMode,StartProcessing,ProcessingAfterWrite,Processing,HandleError,Terminate};
   static const char* stateName( State i)
   {
      static const char* ar[] = {"Init","EnterCommand","EmptyLine","EnterMode","StartProcessing","ProcessingAfterWrite","Processing","HandleError","Terminate"};
      return ar[i];
   };
   //substate of processing (how we do processing)
   enum Mode {Ident,Uppercase,Lowercase};
   static const char* modeName( Mode i)
   {
      static const char* ar[] = {"Ident","Uppercase","Lowercase"};
      return ar[i];
   };

   //* all state variables of this processor
   //1. states
   State state;                               //< state of the processor
   Mode mode;                                 //< selected function to process the content
   //2. buffers and context
   CmdBuffer cmdBuffer;                       //< context (sub state) for partly parsed protocol commands
   LineBuffer buffer;                         //< context (sub state) for partly parsed input lines 
   Input input;                               //< buffer for READ network messages 
   Output output;                             //< buffer for WRITE network messages
   //3. Iterators
   ProtocolIterator itr;                      //< iterator to scan protocol commands
   ContentIterator src;                       //< iterator to scan protocol content terminated with (CR)LF dor (CR)LF

   //* the parser of the protocol 
   typedef protocol::CmdParser<CmdBuffer> ProtocolParser;

   //* helper methods for I/O
   //helper function to send a line message with CRLF termination as C string
   Operation WriteLine( const char* str)
   {
      unsigned int ii;
      buffer.init();
      for (ii=0; str[ii]; ii++) buffer.push_back( str[ii]);
      buffer.push_back( '\r');
      buffer.push_back( '\n');
      const char* msg = buffer.c_str();
      buffer.init();
      return Operation( Operation::WRITE, msg, ii+2);
   };
   //output of one character with return code true/false for success/failure
   bool print( char ch)
   {
      if (ch < 0)
      {
         if (!output.print( ch)) return false;
      }
      else switch (mode)
      {
         case Ident:     if (!output.print( ch)) return false; break;
         case Uppercase: if (!output.print( toupper(ch))) return false; break;
         case Lowercase: if (!output.print( tolower(ch))) return false; break;
      }
      return true;
   };
   //echo processing: every character read from input is written to output and with an end of line recognized the
   //output is flushed. (false returned)
   //@return terminated (got content EOF) true=yes, false=no
   bool echoInput()
   {
      char ch;
      while ((ch=*src) != 0)
      {
         if (output.restsize() == 0)
         {
            return false;
         }
         if (output.restsize() == 0) return false;  //we check if there is space for output to ensure that we can do both
                                                    //  operations input&output or none of them. doing only one of them is
                                                    //  not covered by this state machine.
         if (!print(ch))
         {
            return false;                           //this does not fail because we checked
         }
         ++src;                                     //if this fails we get to the same point when reentering this procedure
      }
      return true;
   };

   //* interface
   Private()   :state(Init),mode(Ident),input(MemBlockSize),output(MemBlockSize)
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
         LOG_DATA << "\nState: " << stateName(state) << "(" << modeName(mode) << ")";
  
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
                //parsing the command:
                enum Command {empty, caps, echo, quit};
                static const char* cmd[5] = {"","caps","echo","quit",0};
                //... the empty command is for an empty line for not bothering the client with obscure error messages.
                //    the next state should read one character for sure otherwise it may result in an endless loop
                static const ProtocolParser parser(cmd);
                
                switch (parser.getCommand( itr, cmdBuffer))
                {             
                   case empty:
                   {
                      state = EmptyLine;
                      continue;
                   }   
                   case caps: 
                   {
                      state = EnterCommand;  
                      return WriteLine( "OK caps echo[tolower|toupper] quit");
                   }
                   case echo: 
                   {
                      state = EnterMode;  
                      continue;
                   }
                   case quit:
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

            case EmptyLine:
            {
                //this state is for reading until the end of the line. there is no buffering below,
                //so we have to the next line somehow:
                ProtocolParser::getLine( itr, buffer);
                if (buffer.size() > 0)
                {
                   state = Init;
                   buffer.init();
                   //a line starting with a space that is not an empty line leads to an error:
                   return WriteLine( "BAD command line");
                }
                else
                {
                   //here is an empty line, so we jump back to the line promt:
                   state = EnterCommand;
                   ProtocolParser::consumeEOLN( itr); //< consume the end of line for not getting into an endless loop with empty command
                   continue;
                }
            }
            
            case EnterMode:
            {
                //here we parse the 1st (and only or missing) argument of the 'echo' command.
                //it defines the way of processing the input lines:
                enum Command {none, tolower, toupper};
                static const char* cmd[4] = {"","tolower","toupper",0}; 
                //... the empty command is for no arguments meaning a simple ECHO
                //    the next state should read one character for sure otherwise it may result in an endless loop (as in EnterCommand)
                static const ProtocolParser parser(cmd);
                
                ProtocolParser::skipSpaces( itr);
                
                switch (parser.getCommand( itr, cmdBuffer))
                {             
                   case none:
                   {
                       //... no argument => simple echo
                       mode = Ident;
                       state = StartProcessing; 
                       continue;
                   }
                   case tolower:
                   {
                       mode = Lowercase;
                       state = StartProcessing;
                       continue;
                   }
                   case toupper:
                   {
                       mode = Uppercase;
                       state = StartProcessing;
                       continue;
                   }
                   default:
                   {
                      state = HandleError;
                      return WriteLine( "BAD unknown argument");
                   }
                }
            }
            
            case StartProcessing:
            {
                //read the rest of the line and reject more arguments than expected. 
                //go on with processing, if this is clear. do not cosnsume the first end of line because it could be
                //the first character of the EOF sequence.
                ProtocolParser::skipSpaces( itr);
                if (!ProtocolParser::isEOLN( itr))
                {
                   state = Init;
                   return WriteLine( "BAD too many arguments");
                }
                else
                {
                   state = Processing;
                   return WriteLine( "OK enter data");
                }
            }
            
            case ProcessingAfterWrite:
            {
                //do processing but first release the output buffer content that has been written in the processing state:
                state = Processing;
                output.release();
                continue;
            }

            case Processing:
            {
                //do the ECHO with some filter function or pure:
                bool eof = echoInput(); 
                if (eof)
                {
                   state = Init;                         //we got to the echo contents EOF = "\n.\n"
                }
                else
                {
                   state = ProcessingAfterWrite;         //we a flushing the output buffer and have to release it when entering next time
                }                
                void* content = output->ptr;
                std::size_t size = output->filled;
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
         return Operation( Operation::READ, input->ptr, input->size);
      };
      return Operation( Operation::TERMINATE);
   };
};


Connection::Connection( const Network::LocalTCPendpoint& local)
{
   data = new Private();
   LOG_TRACE << "Created connection handler for " << local.toString();
}

Connection::Connection( const Network::LocalSSLendpoint& local)
{
   data = new Private();
   LOG_TRACE << "Created connection handler (SSL) for " << local.toString();
}

Connection::~Connection()
{
   LOG_TRACE << "Connection handler destroyed";
   delete data;
}

void Connection::setPeer( const Network::RemoteTCPendpoint& remote)
{
   LOG_TRACE << "Peer set to " << remote.toString();
}

void Connection::setPeer( const Network::RemoteSSLendpoint& remote)
{
   LOG_TRACE << "Peer set to " << remote.toString();
}

void* Connection::parseInput( const void *begin, std::size_t bytesTransferred)
{
   data->input->filled = bytesTransferred;
   return (void*)(((char*)begin) + bytesTransferred);
}

Connection::Operation Connection::nextOperation()
{
   return data->nextOperation();
}


/// ServerHandler PIMPL
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
