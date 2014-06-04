/************************************************************************
Copyright (C) 2011 - 2014 Project Wolframe.
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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Wolframe. If not, see <http://www.gnu.org/licenses/>.

If you have questions regarding the use of this file, please contact
Project Wolframe.

************************************************************************/
///\file pechoHandler.cpp

#include "protocol/parser.hpp"
#include "protocol/ioblocks.hpp"
#include "pechoHandler.hpp"
#include "logger-v1.hpp"

using namespace _Wolframe;
using namespace _Wolframe::pecho;

struct Connection::Private
{
	//* typedefs for input output blocks and input iterators
	typedef protocol::InputBlock Input;				//< input buffer type
	typedef protocol::OutputBlock Output;				//< output buffer type
	typedef protocol::InputBlock::iterator InputIterator;		//< iterator type for protocol commands

	//* typedefs for input output buffers
	typedef protocol::CmdParser<protocol::Buffer> ProtocolParser;	//< parser for the protocol

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
	State state;			//< state of the processor
	Mode mode;			//< selected function to process the content
	//2. buffers and context
	protocol::Buffer buffer;	//< context (sub state) for partly parsed input lines
	Input input;			//< buffer for network read messages
	Output output;			//< buffer for network write messages
	//3. Iterators
	InputIterator itr;		//< iterator to scan protocol input
	InputIterator eoM;		//< iterator pointing to end of message buffer
	InputIterator eoD;		//< iterator pointing to end of data or to eoM, if not yet available

	//* helper methods for I/O
	//helper function to send a line message with CRLF termination as C string
	Operation WriteLine( const char* str)
	{
		unsigned int ii;
		buffer.clear();
		for (ii=0; str[ii]; ii++) buffer.push_back( str[ii]);
		buffer.push_back( '\r');
		buffer.push_back( '\n');
		const char* msg = buffer.c_str();
		buffer.clear();
		return net::SendData( msg, ii+2);
	}
	//output of one character with return code true/false for success/failure
	bool print( char ch)
	{
		if ((unsigned char)ch > 127)
		{
			if (!output.print( ch)) return false;
		}
		else switch (mode)
		{
			case Ident:	if (!output.print( ch)) return false; break;
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
			if (itr == eoM)
			{
				if (input.gotEoD()) return EoD; else return EoM;
			}
			else if (itr == eoD)
			{
				return EoD;
			}
			if (output.restsize() == 0) return bufferFull;
			print(*itr);
			++itr;
		}
	}

	void networkInput( const void* dt, std::size_t nofBytes)
	{
		input.setPos( nofBytes + ((const char*)dt - input.charptr()));
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
	Private( unsigned int inputBufferSize, unsigned int outputBufferSize)		:state(Init),mode(Ident),buffer(128),input(inputBufferSize),output(outputBufferSize)
	{
		itr = input.begin();
		eoD = eoM = input.end();
	}
	~Private()  {}

	const net::NetworkOperation readDataOp()
	{
		void* pp;
		std::size_t ppsize;

		if (!input.getNetworkMessageRead( pp, ppsize))
		{
			LOG_ERROR << "buffer too small to buffer end of data marker in input";
			return net::CloseConnection();
		}
		return net::ReadData( pp, ppsize);
	}

	//statemachine of the processor
	const Operation nextOperation()
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
					//parsing the command:
					enum Command {empty, capa, echo, quit};
					static const char* cmd[5] = {"","capa","echo","quit",0};
					//the empty command is for an empty line for not bothering the client with obscure error messages.
					//the next state should read one character for sure otherwise it may result in an endless loop
					static const ProtocolParser parser(cmd);

					switch (parser.getCommand( itr, eoM, buffer))
					{
						case empty:
						{
							state = EmptyLine;
							continue;
						}
						case capa:
						{
							buffer.clear();
							state = EnterCommand;
							return WriteLine( "OK capa echo[tolower|toupper] quit");
						}
						case echo:
						{
							buffer.clear();
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
								return readDataOp();
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
						return readDataOp();
					}
					if (!ProtocolParser::consumeEOL( itr, eoM))
					{
						if (itr == eoM)
						{
							return readDataOp();
						}
						else
						{
							state = Init;
							buffer.clear();
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
					//			the next state should read one character for sure
					//			otherwise it may result in an endless loop (as in EnterCommand)

					static const ProtocolParser parser(cmd);

					if (!ProtocolParser::skipSpaces( itr, eoM))
					{
						return readDataOp();
					}

					switch (parser.getCommand( itr, eoM, buffer))
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
								return readDataOp();
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
					//go on with processing, if this is clear. do not consume the first end of line because it could be
					//the first character of the EOF sequence.
					(void)input.skipEoD();
					if (!ProtocolParser::skipSpaces( itr, eoM))
					{
						return readDataOp();;
					}
					if (!ProtocolParser::isEOL( itr))
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
						return readDataOp();
					}
					if (echoState == EoD)
					{
						state = Init;
					}
					else
					{
						state = ProcessingAfterWrite;
					}
					void* pp = output.ptr();
					std::size_t ppsize = output.pos();
					if (ppsize == 0) continue;
					return net::SendData( pp, ppsize);
				}

				case HandleError:
				{
					if (!ProtocolParser::skipLine( itr, eoM) || !ProtocolParser::consumeEOL( itr, eoM))
					{
						return readDataOp();
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
		}//for(,,)
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

void Connection::signalOccured( NetworkSignal )
{
		data->signalTerminate();
}

const Connection::Operation Connection::nextOperation()
{
		return data->nextOperation();
}

/// ServerHandler PIMPL
net::ConnectionHandler* ServerHandler::ServerHandlerImpl::newConnection( const net::LocalEndpoint& local )
{
		return new pecho::Connection( local );
}


ServerHandler::ServerHandler( const HandlerConfiguration*,
			      const module::ModulesDirectory* /*modules*/  )
	: m_impl( new ServerHandlerImpl ) {}

ServerHandler::~ServerHandler()  { delete m_impl; }

net::ConnectionHandler* ServerHandler::newConnection( const net::LocalEndpoint& local )
{
		return m_impl->newConnection( local );
}


