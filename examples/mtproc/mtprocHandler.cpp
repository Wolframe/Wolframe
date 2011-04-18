/************************************************************************

 Copyright (C) 2011 Project Wolframe.
 All rights reserved.

 This file is part of Project Wolframe.

 Commercial Usage
    Licensees holding valid Project Wolframe Commercial licenses may
    use this file in accordance with the Project Wolframe
    Commercial License Agreement provided with the Software or,
    alternatively, in accordance with the terms contained
    in a written agreement between the licensee and Project Wolframe.

 GNU General Public License Usage
    Alternatively, you can redistribute this file and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Wolframe is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
///
/// \file mtprocHandler.cpp
///

#include "protocol.hpp"
#include "protocol/ioblocks.hpp"
#include "mtprocHandler.hpp"
#include "logger.hpp"
#include "dispatcher.hpp"
#include "implementation.hpp"

#ifndef _WIN32
#include <cstdio>
#else
#define snprintf _snprintf
#endif

using namespace _Wolframe;
using namespace _Wolframe::mtproc;

struct Connection::Private
{
	//* typedefs for input output blocks and input iterators
	typedef protocol::InputBlock Input;				//< input buffer type
	typedef protocol::OutputBlock Output;				//< output buffer type
	typedef protocol::InputBlock::iterator InputIterator;		//< iterator type for protocol commands

	//* typedefs for input output buffers
	typedef protocol::Buffer<128> LineBuffer;			//< buffer for one line of input/output
	typedef protocol::CmdParser<LineBuffer> ProtocolParser;		//< parser for the protocol

	//* typedefs for state variables and buffers
	//list of processor states
	enum State
	{
		Init,					//< start state, called first time in this session
		EnterCommand,				//< parse command
		Processing,				//< running the dispatcher sub state machine; execute a command without data input
		ProcessingInput,			//< running the dispatcher sub state machine; execute a command with data input
		ProtocolError,				//< a protocol error (bad command etc) appeared and the rest of the line has to be discarded
		DiscardInput,				//< reading and discarding data until end of data has been seen
		EndOfCommand,				//< cleanup after processing
		Terminate				//< terminate processing (close for network)
	};
	static const char* stateName( State i)
	{
		static const char* ar[] = {"Init","EnterCommand","Processing","ProcessingInput","ProtocolError","CommandError","EndOfCommand","Terminate"};
		return ar[i];
	}

	//* all state variables of this processor
	//1. states
	State state;					//< state of the processor (protocol main statemachine)
	CommandDispatcher commandDispatcher;		//< state of the processor in the command execution phase (protocol sub statemachine)

	//2. buffers and context
	LineBuffer buffer;				//< context (sub state) for partly parsed input lines
	Input input;					//< buffer for network read messages
	Output output;					//< buffer for network write messages
	//3. Iterators
	InputIterator itr;				//< iterator to scan protocol input
	InputIterator end;				//< iterator pointing to end of message buffer

	//3. implementation
	Implementation object;

	//* helper methods for I/O
	//helper function to send a line message with CRLF termination as C string
	Operation WriteLine( const char* str, const char* arg=0)
	{
		unsigned int ii;
		buffer.clear();
		for (ii=0; str[ii]; ii++) buffer.push_back( str[ii]);
		if (arg)
		{
			buffer.push_back( ' ');
			for (ii=0; arg[ii]; ii++) buffer.push_back( arg[ii]);
		}
		buffer.push_back( '\r');
		buffer.push_back( '\n');
		const char* msg = buffer.c_str();
		unsigned int msgsize = buffer.size();
		buffer.clear();
		return net::SendData( msg, msgsize);
	}

	void passInput()
	{
		Input::iterator eoD = input.getEoD( itr);

		if (state == ProcessingInput)
		{
			commandDispatcher.protocolInput( itr, eoD, input.gotEoD());
		}
		end = input.end();
		itr = (eoD < end) ? (eoD+1):end;
	}

	void networkInput( const void*, std::size_t nofBytes)
	{
		input.setPos( nofBytes);
		itr = input.begin();

		if (state == ProcessingInput || state == DiscardInput)
		{
			Input::iterator eoD = input.getEoD( itr);
			if (state == ProcessingInput)
			{
				commandDispatcher.protocolInput( itr, eoD, input.gotEoD());
			}
			end = input.end();
			itr = (eoD < end)? (eoD+1):end;
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
	Private( unsigned int inputBufferSize, unsigned int outputBufferSize)	:state(Init),input(inputBufferSize),output(outputBufferSize)
	{
		itr = input.begin();
		end = input.end();
		object.init();
		commandDispatcher.init( &object);
	}
	~Private()
	{
		object.done();
	}

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
						case CommandDispatcher::capa:
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
								return net::ReadData( input.ptr(), input.size());
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
				case ProcessingInput:
				{
					int returnCode = 0;
					switch (commandDispatcher.call( returnCode))
					{
						case CommandDispatcher::ReadInput:
							if (state == Processing)
							{
								state = ProcessingInput;
								passInput();
								continue;
							}
							else
							{
								input.setPos( 0);
								return net::ReadData( input.ptr(), input.size());
							}

						case CommandDispatcher::WriteOutput:
						{
							void* content;
							unsigned int contentsize;
							bool hasOutput = commandDispatcher.getOutput( &content, &contentsize);
							commandDispatcher.setOutputBuffer( output.ptr(), output.size());

							if (!hasOutput) continue; else return net::SendData( content, contentsize);
						}

						case CommandDispatcher::Close:
						{
							if (commandDispatcher.commandHasIO())
							{
								if (state == Processing) passInput();
								state = (input.gotEoD())?EndOfCommand:DiscardInput;
								return WriteLine( "\r\n.\r\nOK");
							}
							else
							{
								state = Init;
								return WriteLine( "\r\nOK");
							}
						}

						case CommandDispatcher::Error:
						{
							char ee[ 64];
							snprintf( ee, sizeof(ee), "%d", returnCode);

							if (commandDispatcher.commandHasIO())
							{
								if (state == Processing) passInput();
								state = (input.gotEoD())?EndOfCommand:DiscardInput;
								return WriteLine( "\r\n.\r\nERR", ee);
							}
							else
							{
								state = Init;
								return WriteLine( "\r\nERR", ee);
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
						return net::ReadData( input.ptr(), input.size());
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
						return net::ReadData( input.ptr(), input.size());
					}
				}

				case ProtocolError:
				{
					if (!ProtocolParser::skipLine( itr, end) || !ProtocolParser::consumeEOLN( itr, end))
					{
						input.setPos( 0);
						return net::ReadData( input.ptr(), input.size());
					}
					state = Init;
					continue;
				}

				case Terminate:
				{
					state = Terminate;
					return net::CloseConnection();
				}
			}//switch(..)
		}//for(;;)
		return net::CloseConnection();
	}
};


Connection::Connection( const net::LocalEndpoint& local, unsigned int inputBufferSize, unsigned int outputBufferSize)
{
	data = new Private( inputBufferSize, outputBufferSize);
	LOG_TRACE << "Created connection handler for " << local.toString();
}


Connection::~Connection()
{
	LOG_TRACE << "Connection handler destroyed";
	delete data;
}

void Connection::setPeer( const net::RemoteEndpoint& remote)
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
net::connectionHandler* ServerHandler::ServerHandlerImpl::newConnection( const net::LocalEndpoint& local )
{
	return new mtproc::Connection( local );
}


ServerHandler::ServerHandler( const HandlerConfiguration* ) : impl_( new ServerHandlerImpl ) {}

ServerHandler::~ServerHandler()  { delete impl_; }

net::connectionHandler* ServerHandler::newConnection( const net::LocalEndpoint& local )
{
	return impl_->newConnection( local );
}

