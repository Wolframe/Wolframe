//
// pechoHandler.cpp
//
#include "protocol.hpp"
#include "pechoHandler.hpp"
#include "logger.hpp"

using namespace _Wolframe;
using namespace _Wolframe::pecho;

struct Connection::Private
{
   //* typedefs for input output blocks and input iterators
   typedef protocol::InputBlock Input;                          //< input buffer type
   typedef protocol::OutputBlock Output;                        //< output buffer type
   typedef protocol::InputBlock::iterator InputIterator;        //< iterator type for protocol commands

   //* typedefs for input output buffers
   typedef protocol::Buffer<128> LineBuffer;                    //< buffer for one line of input/output
   typedef protocol::CmdBuffer CmdBuffer;                       //< buffer for protocol commands
   typedef protocol::CmdParser<CmdBuffer> ProtocolParser;       //< parser for the protocol

   //* typedefs for state variables and buffers
   //list of processor states
   enum State
   {
      Init,
      EnterCommand,
      EmptyLine,
      EnterMode,
      StartProcessing,
      ProcessingAfterWrite,
      Processing,
      HandleError,
      Terminate
   };
   static const char* stateName( State i)
   {
      static const char* ar[] = {"Init","EnterCommand","EmptyLine","EnterMode","StartProcessing","ProcessingAfterWrite","Processing","HandleError","Terminate"};
      return ar[i];
   }
   //substate of processing (how we do processing)
   enum Mode {Ident,Uppercase,Lowercase};
   static const char* modeName( Mode i)
   {
      static const char* ar[] = {"Ident","Uppercase","Lowercase"};
      return ar[i];
   }

   //* all state variables of this processor
   //1. states
   State state;                               //< state of the processor
   Mode mode;                                 //< selected function to process the content
   //2. buffers and context
   CmdBuffer cmdBuffer;                       //< context (sub state) for partly parsed protocol commands
   LineBuffer buffer;                         //< context (sub state) for partly parsed input lines
   Input input;                               //< buffer for network read messages
   Output output;                             //< buffer for network write messages
   //3. Iterators
   InputIterator itr;                         //< iterator to scan protocol input
   InputIterator eoM;                         //< iterator pointing to end of message buffer
   InputIterator eoD;                         //< iterator pointing to end of data or to eoM, if not yet available

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
      return Network::SendData( msg, ii+2);
   }
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
   }
   //echo processing: every character read from input is written to output
   //@return EchoState
   enum EchoState {EoD, EoM, bufferFull};
   EchoState echoInput()
   {
      for (;;)
      {
	 if (itr == eoD)
	 {
	    if (itr == eoM) return EoM; else return EoD;
	 }
	 if (output.restsize() == 0) return bufferFull;
	 print(*itr);
	 ++itr;
      }
   }

   void networkInput( const void*, std::size_t nofBytes)
   {
       input.setPos( nofBytes);
       itr = input.begin();
       if (state == Processing || state == StartProcessing)
       {
	  eoD = input.getEoD( itr);
	  eoM = input.end();
       }
       else
       {
	  eoD = eoM = input.end();
       }
   }

   void signalTerminate()
   {
	state = Terminate;
   }

   //* interface
   Private( unsigned int inputBufferSize, unsigned int outputBufferSize)   :state(Init),mode(Ident),input(inputBufferSize),output(outputBufferSize)
   {
      itr = input.begin();
      eoD = eoM = input.end();
   }
   ~Private()  {}

   //statemachine of the processor
   const Operation nextOperation()
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

		switch (parser.getCommand( itr, eoM, cmdBuffer))
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
		      if (itr == eoM)
		      {
			 input.setPos( 0);
			 return Network::ReadData( input.ptr(), input.size());
		      }
		      else
		      {
			 state = HandleError;
			 return WriteLine( "BAD unknown command");
		      }
		   }
		}
	    }

	    case EmptyLine:
	    {
	       if (!ProtocolParser::skipSpaces( itr, eoM))
	       {
		  input.setPos( 0);
		  return Network::ReadData( input.ptr(), input.size());
	       }
	       if (!ProtocolParser::consumeEOLN( itr, eoM))
	       {
		  if (itr == eoM)
		  {
		     input.setPos( 0);
		     return Network::ReadData( input.ptr(), input.size());
		  }
		  else
		  {
		     state = Init;
		     buffer.init();
		     return WriteLine( "BAD command line");
		  }
	       }
	       else
	       {
		  state = EnterCommand;
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
	       //    the next state should read one character for sure
	       //    otherwise it may result in an endless loop (as in EnterCommand)

	       static const ProtocolParser parser(cmd);

	       if (!ProtocolParser::skipSpaces( itr, eoM))
	       {
		  input.setPos( 0);
		  return Network::ReadData( input.ptr(), input.size());
	       }

	       switch (parser.getCommand( itr, eoM, cmdBuffer))
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
		     if (itr == eoM)
		     {
			input.setPos( 0);
			return Network::ReadData( input.ptr(), input.size());
		     }
		     else
		     {
			state = HandleError;
			return WriteLine( "BAD unknown command argument");
		     }
		  }
	       }
	    }

	    case StartProcessing:
	    {
	       //read the rest of the line and reject more arguments than expected.
	       //go on with processing, if this is clear. do not cosnsume the first end of line because it could be
	       //the first character of the EOF sequence.
	       input.resetEoD();
	       if (!ProtocolParser::skipSpaces( itr, eoM))
	       {
		  input.setPos( 0);
		  return Network::ReadData( input.ptr(), input.size());
	       }
	       if (!ProtocolParser::isEOLN( itr))
	       {
		  state = Init;
		  return WriteLine( "BAD too many arguments");
	       }
	       else
	       {
		  state = Processing;
		  eoD = input.getEoD( itr);
		  eoM = input.end();
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
		EchoState echoState = echoInput();
		if (echoState == EoM)
		{
		   input.setPos( 0);
		   return Network::ReadData( input.ptr(), input.size());
		}

		if (echoState == EoD)
		{
		   input.resetEoD();
		   state = Init;
		}
		else
		{
		   state = ProcessingAfterWrite;
		}
		void* content = output.ptr();
		std::size_t size = output.pos();
		if (size == 0) continue;
		return Network::SendData( content, size);
	    }

	    case HandleError:
	    {
		if (!ProtocolParser::skipLine( itr, eoM) || !ProtocolParser::consumeEOLN( itr, eoM))
		{
		   input.setPos( 0);
		   return Network::ReadData( input.ptr(), input.size());
		}
		state = Init;
		continue;
	    }

	    case Terminate:
	    {
		state = Terminate;
		return Network::CloseConnection();
	    }
	 }//switch(..)
      }//for(,,)
      return Network::CloseConnection();
   }
};


Connection::Connection( const Network::LocalTCPendpoint& local, unsigned int inputBufferSize, unsigned int outputBufferSize)
{
   data = new Private( inputBufferSize, outputBufferSize);
   LOG_TRACE << "Created connection handler for " << local.toString();
}

#ifdef WITH_SSL
Connection::Connection( const Network::LocalSSLendpoint& local)
{
   data = new Private(8,8);
   LOG_TRACE << "Created connection handler (SSL) for " << local.toString();
}
#endif // WITH_SSL

Connection::~Connection()
{
   LOG_TRACE << "Connection handler destroyed";
   delete data;
}

void Connection::setPeer( const Network::RemoteTCPendpoint& remote)
{
   LOG_TRACE << "Peer set to " << remote.toString();
}

#ifdef WITH_SSL
void Connection::setPeer( const Network::RemoteSSLendpoint& remote)
{
   LOG_TRACE << "Peer set to " << remote.toString();
}
#endif // WITH_SSL

void Connection::networkInput( const void* bytes, std::size_t nofBytes)
{
   data->networkInput( bytes, nofBytes);
}

void Connection::timeoutOccured()
{
   data->signalTerminate();
}

void Connection::signalOccured()
{
   data->signalTerminate();
}

void Connection::errorOccured( NetworkSignal )
{
   data->signalTerminate();
}

const Connection::Operation Connection::nextOperation()
{
   return data->nextOperation();
}

/// ServerHandler PIMPL
Network::connectionHandler* ServerHandler::ServerHandlerImpl::newConnection( const Network::LocalTCPendpoint& local )
{
   return new pecho::Connection( local );
}

#ifdef WITH_SSL
Network::connectionHandler* ServerHandler::ServerHandlerImpl::newSSLconnection( const Network::LocalSSLendpoint& local )
{
   return new pecho::Connection( local );
}
#endif

ServerHandler::ServerHandler( const HandlerConfiguration* ) : impl_( new ServerHandlerImpl ) {}

ServerHandler::~ServerHandler()  { delete impl_; }

Network::connectionHandler* ServerHandler::newConnection( const Network::LocalTCPendpoint& local )
{
   return impl_->newConnection( local );
}

#ifdef WITH_SSL
Network::connectionHandler* ServerHandler::newSSLconnection( const Network::LocalSSLendpoint& local )
{
   return impl_->newSSLconnection( local );
}
#endif

