//
// mtprocHandler.cpp
//
#include "protocol.hpp"
#include "protocol/ioblocks.hpp"
#include "mtprocHandler.hpp"
#include "logger.hpp"
#include "dispatcher.hpp"
#include "implementation.hpp"

using namespace _Wolframe;
using namespace _Wolframe::mtproc;

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
      Init,                     //< start state, called first time in this session
      EnterCommand,             //< parse command
      Processing,               //< running the dispatcher sub state machine; execute a command
      ProtocolError,            //< a protocol error (bad command etc) appeared and the rest of the line has to be discarded
      DiscardInput,             //< reading and discarding data until end of data has been seen
      EndOfCommand,             //< cleanup after processing
      Terminate                 //< terminate processing (close for network)
   };
   static const char* stateName( State i)
   {
      static const char* ar[] = {"Init","EnterCommand","Processing","ProtocolError","CommandError","EndOfCommand","Terminate"};
      return ar[i];
   }

   //* all state variables of this processor
   //1. states
   State state;                               //< state of the processor (protocol main statemachine)
   CommandDispatcher commandDispatcher;       //< state of the processor in the command execution phase (protocol sub statemachine)

   //2. buffers and context
   CmdBuffer cmdBuffer;                       //< context (sub state) for partly parsed protocol commands
   LineBuffer buffer;                         //< context (sub state) for partly parsed input lines
   Input input;                               //< buffer for network read messages
   Output output;                             //< buffer for network write messages
   //3. Iterators
   InputIterator itr;                         //< iterator to scan protocol input
   InputIterator end;                         //< iterator pointing to end of message buffer

   //3. implementation
   Method::Data object;

   //* helper methods for I/O
   //helper function to send a line message with CRLF termination as C string
   Operation WriteLine( const char* str, const char* arg=0)
   {
      unsigned int ii;
      buffer.init();
      for (ii=0; str[ii]; ii++) buffer.push_back( str[ii]);
      if (arg)
      {
	 buffer.push_back( ' ');
	 for (ii=0; arg[ii]; ii++) buffer.push_back( arg[ii]);
      }
      buffer.push_back( '\r');
      buffer.push_back( '\n');
      const char* msg = buffer.c_str();
      buffer.init();
      return Network::SendData( msg, ii+2);
   }

   void networkInput( const void*, std::size_t nofBytes)
   {
       input.setPos( nofBytes);
       itr = input.begin();

       if (state == Processing || state == DiscardInput)
       {
	  Input::iterator eoD = input.getEoD( itr);
	  if (state == Processing)
	  {
	     commandDispatcher.protocolInput( itr, eoD, input.gotEoD());
	  }
	  end = input.end();
	  if (eoD < end)
	  {
	     itr = eoD+1;
	  }
	  else
	  {
	     itr = end;
	  }
       }
       else
       {
	  end = input.end();
       }
   }

   void signalTerminate()
   {
       state = Terminate;
   }

   //* interface
   Private( unsigned int inputBufferSize, unsigned int outputBufferSize)   :state(Init),input(inputBufferSize),output(outputBufferSize)
   {
       itr = input.begin();
       end = input.end();
       commandDispatcher.init( &object);
   }
   ~Private()  {}

   //statemachine of the processor
   const Operation nextOperation()
   {
      for (;;)
      {
	 LOG_DATA << "Handler State: " << stateName(state);

	 switch( state)
	 {
	    case Init:
	    {
		//start:
		state = EnterCommand;
		return WriteLine( "OK expecting command");
	    }

	    case EnterCommand:
	    {
		switch (commandDispatcher.getCommand( itr, end))
		{
		   case CommandDispatcher::empty:
		   {
		      state = EnterCommand;
		      continue;
		   }
		   case CommandDispatcher::caps:
		   {
		      state = EnterCommand;
		      return WriteLine( "OK", commandDispatcher.getCapabilities());
		   }
		   case CommandDispatcher::quit:
		   {
		      state = Terminate;
		      return WriteLine( "BYE");
		   }
		   case CommandDispatcher::method:
		   {
		      input.resetEoD();
		      state = Processing;
		      continue;
		   }
		   case CommandDispatcher::unknown:
		   {
		      if (itr == end)
		      {
			 input.setPos( 0);
			 return Network::ReadData( input.ptr(), input.size());
		      }
		      else
		      {
			 state = ProtocolError;
			 return WriteLine( "BAD error in command line");
		      }
		   }
		}
	    }

	    case Processing:
	    {
		int returnCode = 0;
		switch (commandDispatcher.call( returnCode))
		{
		   case CommandDispatcher::ReadInput:
		   {
		      input.setPos( 0);
		      return Network::ReadData( input.ptr(), input.size());
		   }
		   case CommandDispatcher::WriteOutput:
		   {
		      void* content;
		      unsigned int contentsize;
		      bool hasOutput = commandDispatcher.getOutput( &content, &contentsize);
		      commandDispatcher.setOutputBuffer( output.ptr(), output.size());

		      if (!hasOutput) continue;

		      return Network::SendData( content, contentsize);
		   }
		   case CommandDispatcher::Close:
		   {
		      if (returnCode == 0)
		      {
			 state = (input.gotEoD())?EndOfCommand:DiscardInput;
			 return WriteLine( "\r\n.\r\nOK");
		      }
		      else
		      {
			 state = DiscardInput;
			 char ee[ 64];
			 snprintf( ee, sizeof(ee), "%d", returnCode);
			 return WriteLine( "\r\n.\r\nERR", ee);
		      }
		   }
		}
	    }

	    case DiscardInput:
	    {
	       if (input.gotEoD())
	       {
		  state = EndOfCommand;
		  continue;
	       }
	       else
	       {
		  input.setPos( 0);
		  return Network::ReadData( input.ptr(), input.size());
	       }
	    }

	    case EndOfCommand:
	    {
	       itr = input.getStart( itr);
	       if (input.gotEoD_LF())
	       {
		  state = EnterCommand;
		  continue;
	       }
	       else if (itr < end)
	       {
		  state = Init;
		  return WriteLine( "BAD end of line in protocol after end of data");
	       }
	       else
	       {
		  input.setPos( 0);
		  return Network::ReadData( input.ptr(), input.size());
	       }
	    }

	    case ProtocolError:
	    {
		if (!ProtocolParser::skipLine( itr, end) || !ProtocolParser::consumeEOLN( itr, end))
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

Connection::Connection( const Network::LocalSSLendpoint& local)
{
   data = new Private(8,8);
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

void Connection::networkInput( const void* bytes, std::size_t nofBytes)
{
   data->networkInput( bytes, nofBytes);
}

void Connection::initObject( Instance* instance)
{
   data->commandDispatcher.init( instance);
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
   return new mtproc::Connection( local );
}

Network::connectionHandler* ServerHandler::ServerHandlerImpl::newSSLconnection( const Network::LocalSSLendpoint& local )
{
   return new mtproc::Connection( local );
}

ServerHandler::ServerHandler( const HandlerConfiguration* ) : impl_( new ServerHandlerImpl ) {}

ServerHandler::~ServerHandler()  { delete impl_; }

Network::connectionHandler* ServerHandler::newConnection( const Network::LocalTCPendpoint& local )
{
   return impl_->newConnection( local );
}

Network::connectionHandler* ServerHandler::newSSLconnection( const Network::LocalSSLendpoint& local )
{
   return impl_->newSSLconnection( local );
}

