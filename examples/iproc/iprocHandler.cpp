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
/// \file iprocHandler.cpp
///

#include "protocol.hpp"
#include "protocol/ioblocks.hpp"
#include "iprocHandler.hpp"
#include "logger.hpp"
#include "langbind/appObjects.hpp"
#include <boost/lexical_cast.hpp>

#if WITH_LUA
#include "langbind/luaConfig.hpp"
#include "langbind/luaAppProcessor.hpp"
typedef _Wolframe::iproc::lua::AppProcessor Processor;
#endif

using namespace _Wolframe;
using namespace _Wolframe::iproc;

struct Connection::Private
{
	//* typedefs for input output blocks and input iterators
	typedef protocol::InputBlock Input;				///< input buffer type
	typedef protocol::OutputBlock Output;				///< output buffer type
	typedef protocol::InputBlock::iterator InputIterator;		///< iterator type for protocol commands

	//* typedefs for input output buffers
	typedef protocol::Buffer<256> LineBuffer;			///< buffer for one line of input/output
	typedef protocol::CmdParser<LineBuffer> ProtocolParser;		///< parser for the protocol
	typedef protocol::CArgBuffer<LineBuffer> ArgBuffer;		///< buffer type for the command arguments

	//* typedefs for state variables and buffers
	//list of processor states
	enum State
	{
		Init,					///< start state, called first time in this session
		EnterCommand,				///< parse command
		ParseArgs,				///< parse command arguments
		Processing,				///< running a command
		ProcessingInput,			///< running a command with data input
		ProtocolError,				///< a protocol error (bad command etc) appeared and the rest of the line has to be discarded
		DiscardInput,				///< reading and discarding data until end of data has been seen
		EndOfCommand,				///< cleanup after processing
		Terminate				///< terminate processing (close for network)
	};
	static const char* stateName( State i)
	{
		static const char* ar[] = {"Init","EnterCommand","ParseArgs", "Processing","ProcessingInput","ProtocolError","CommandError","EndOfCommand","Terminate"};
		return ar[i];
	}
	enum Command {empty, capa, run, quit};
	static const char* commandName( Command c)
	{
		const char* ar[] = {"empty", "capa", "run", "quit", 0};
		return ar[c];
	}

	//* all state variables of this processor
	//1. states
	State state;					///< state of the processor (protocol main statemachine)

	//2. buffers and context
	LineBuffer buffer;				///< context (sub state) for partly parsed input lines
	ArgBuffer argBuffer;				///< buffer for the arguments
	Command cmdidx;					///< command parsed

	Input input;					///< buffer for network read messages
	Output output;					///< buffer for network write messages
	//3. Iterators
	InputIterator itr;				///< iterator to scan protocol input
	InputIterator end;				///< iterator pointing to end of message buffer

	//3. implementation
	app::System app_system;				///< interface to system functions and loaded resources
	app::Input app_input;				///< network input interface for the interpreter
	app::Output app_output;				///< network output interface for the interpreter
	Processor processor;				///< the interpreter state
	const char* functionName;			///< name of the method to execute
	bool functionHasIO;				///< true if the method to execute does content data processing (input/output)

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
		argBuffer.clear();
		return net::SendData( msg, msgsize);
	}

	void passInput()
	{
		Input::iterator eoD = input.getEoD( itr);

		if (state == ProcessingInput)
		{
			app_input.m_inputfilter->protocolInput( itr.ptr(), eoD-itr, input.gotEoD());
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
				app_input.m_inputfilter->protocolInput( itr.ptr(), eoD-itr, input.gotEoD());
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

	Private( const lua::Configuration* config)
		:state(Init)
		,argBuffer(&buffer)
		,input(config->input_bufsize())
		,output(config->output_bufsize())
		,processor(&app_system, config, app_input,app_output)
		,functionName(0)
		,functionHasIO(false)
	{
		itr = input.begin();
		end = input.end();
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
					buffer.clear();
					argBuffer.clear();
					return WriteLine( "OK expecting command");
				}

				case EnterCommand:
				{
					functionName = 0;
					functionHasIO = false;

					//the empty command is for an empty line for not bothering the client with obscure error messages.
					//the next state should read one character for sure otherwise it may result in an endless loop
					static const ProtocolParser parser(&commandName);
					cmdidx = (Command)parser.getCommand( itr, end, buffer);
					switch (cmdidx)
					{
						case empty:
						case capa:
						case run:
						case quit:
						{
							state = ParseArgs;
							continue;
						}
						default:
						{
							if (itr == end)
							{
								input.setPos( 0);
								return net::ReadData( input.ptr(), input.size());
							}
							else
							{
								state = ProtocolError;
								return WriteLine( "BAD unknown command");
							}
						}
					}
				}

				case ParseArgs:
				{
					if (!ProtocolParser::getLine( itr, end, argBuffer))
					{
						if (itr == end)
						{
							input.setPos( 0);
							return net::ReadData( input.ptr(), input.size());
						}
						else
						{
							state = ProtocolError;
							return WriteLine( "BAD arguments");
						}
					}
					switch (cmdidx)
					{
						case empty:
							if (argBuffer.argc())
							{
								state = ProtocolError;
								return WriteLine( "BAD command");
							}
							else
							{
								buffer.clear();
								argBuffer.clear();
								state = EnterCommand;
								continue;
							}
						case capa:
							if (argBuffer.argc())
							{
								state = ProtocolError;
								return WriteLine( "BAD command arguments");
							}
							else
							{
								return WriteLine( "OK capa run quit");
								state = Init;
								continue;
							}
						case quit:
							if (argBuffer.argc())
							{
								state = ProtocolError;
								return WriteLine( "BAD command arguments");
							}
							else
							{
								state = Terminate;
								continue;
							}
						case run:
							if (!processor.getCommand( "run", functionName, functionHasIO))
							{
								LOG_ERROR << "Command for 'run' not defined in configuration";
								state = ProtocolError;
								return WriteLine( "BAD command not defined");
							}
							state = Processing;
							continue;
					}
				}

				case Processing:
				case ProcessingInput:
				{
					int returnCode = 0;
					const char** argv = argBuffer.argv( functionName);
					unsigned int argc = argBuffer.argc( functionName);

					switch (processor.call( argc, argv, functionHasIO))
					{
						case Processor::YieldRead:
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

						case Processor::YieldWrite:
						{
							void* content = app_output.m_formatoutput->ptr();
							unsigned int contentsize = app_output.m_formatoutput->pos();
							if (!functionHasIO)
							{
								LOG_WARNING << "output of function '" << functionName << "' that has no IO configured is ignored";
								contentsize = 0;
							}
							app_output.m_formatoutput->init( output.ptr(), output.size());

							if (contentsize == 0)
							{
								continue;
							}
							else
							{
								return net::SendData( content, contentsize);
							}
						}

						case Processor::Ok:
						{
							if (functionHasIO)
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

						case Processor::Error:
						{
							std::string ee = boost::lexical_cast<std::string>( returnCode);

							if (functionHasIO)
							{
								if (state == Processing) passInput();
								state = (input.gotEoD())?EndOfCommand:DiscardInput;
								return WriteLine( "\r\n.\r\nERR", ee.c_str());
							}
							else
							{
								state = Init;
								return WriteLine( "\r\nERR", ee.c_str());
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


Connection::Connection( const net::LocalEndpoint& local, const AppConfiguration* config)
{
	data = new Private( config);
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
	return new iproc::Connection( local, m_config->m_appConfig);
}


ServerHandler::ServerHandler( const HandlerConfiguration* cfg ) : impl_( new ServerHandlerImpl( cfg) ) {}

ServerHandler::~ServerHandler()  { delete impl_; }

net::connectionHandler* ServerHandler::newConnection( const net::LocalEndpoint& local )
{
	return impl_->newConnection( local );
}

