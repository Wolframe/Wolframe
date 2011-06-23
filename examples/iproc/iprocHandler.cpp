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

#include "iprocHandler.hpp"
#include "logger.hpp"


using namespace _Wolframe;
using namespace _Wolframe::iproc;

net::NetworkOperation Connection::WriteLine( const char* str, const char* arg)
{
	unsigned int ii;
	m_buffer.clear();
	for (ii=0; str[ii]; ii++) m_buffer.push_back( str[ii]);
	if (arg)
	{
		m_buffer.push_back( ' ');
		for (ii=0; arg[ii]; ii++) m_buffer.push_back( arg[ii]);
	}
	m_buffer.push_back( '\r');
	m_buffer.push_back( '\n');
	const char* msg = m_buffer.c_str();
	unsigned int msgsize = m_buffer.size();
	m_buffer.clear();
	m_argBuffer.clear();
	return net::SendData( msg, msgsize);
}

void Connection::passInput()
{
	Input::iterator eoD = m_input.getEoD( m_itr);
	m_inputfilter->protocolInput( m_itr.ptr(), eoD-m_itr, m_input.gotEoD());
	m_end = m_input.end();
	m_itr = (eoD < m_end) ? eoD:m_end;
}

void Connection::networkInput( const void*, std::size_t nofBytes)
{
	LOG_DATA << "ConnectionHandler got network input in state " << stateName(m_state);
	m_input.setPos( nofBytes);
	m_itr = m_input.begin();

	if (m_state == Processing || m_state == DiscardInput)
	{
		Input::iterator eoD = m_input.getEoD( m_itr);
		if (m_state == Processing)
		{
			m_inputfilter->protocolInput( m_itr.ptr(), eoD-m_itr, m_input.gotEoD());
		}
		m_end = m_input.end();
		m_itr = (eoD < m_end)? eoD:m_end;
	}
	else
	{
		m_end = m_input.end();
	}
}

void Connection::timeoutOccured()
{
	LOG_TRACE << "Got termination signal (timeout occurred)";
	m_state = Terminate;
}

void Connection::signalOccured()
{
	LOG_TRACE << "Got termination signal (signal occurred)";
	m_state = Terminate;
}

void Connection::errorOccured( NetworkSignal )
{
	LOG_TRACE << "Got termination signal (error occurred)";
	m_state = Terminate;
}

const net::NetworkOperation Connection::nextOperation()
{
	for (;;)
	{
		LOG_DATA << "ConnectionHandler State: " << stateName(m_state);

		switch( m_state)
		{
			case Init:
			{
				//start:
				m_state = EnterCommand;
				m_buffer.clear();
				m_argBuffer.clear();
				return WriteLine( "OK expecting command");
			}

			case EnterCommand:
			{
				m_functionName = 0;
				m_functionHasIO = false;

				//the empty command is for an empty line for not bothering the client with obscure error messages.
				//the next state should read one character for sure otherwise it may result in an endless loop
				static const ProtocolParser parser(&commandName);
				m_cmdidx = (Command)parser.getCommand( m_itr, m_end, m_buffer);
				switch (m_cmdidx)
				{
					case empty:
					case capa:
					case run:
					case quit:
					{
						m_state = ParseArgs;
						continue;
					}
					default:
					{
						if (m_itr == m_end)
						{
							m_input.setPos( 0);
							return net::ReadData( m_input.ptr(), m_input.size());
						}
						else
						{
							m_state = ProtocolError;
							return WriteLine( "BAD unknown command");
						}
					}
				}
			}

			case ParseArgs:
			{
				if (!ProtocolParser::getLine( m_itr, m_end, m_argBuffer))
				{
					if (m_itr == m_end)
					{
						m_input.setPos( 0);
						return net::ReadData( m_input.ptr(), m_input.size());
					}
					else
					{
						m_state = ProtocolError;
						return WriteLine( "BAD arguments");
					}
				}
				switch (m_cmdidx)
				{
					case empty:
						if (m_argBuffer.argc())
						{
							m_state = ProtocolError;
							return WriteLine( "BAD command");
						}
						else if (ProtocolParser::consumeEOLN( m_itr, m_end))
						{
							m_buffer.clear();
							m_argBuffer.clear();
							m_state = EnterCommand;
							continue;
						}
						else if (m_itr == m_end)
						{
							m_input.setPos( 0);
							return net::ReadData( m_input.ptr(), m_input.size());
						}
						else
						{
							m_state = ProtocolError;
							return WriteLine( "BAD command");
						}
					case capa:
						if (m_argBuffer.argc())
						{
							m_state = ProtocolError;
							return WriteLine( "BAD command arguments");
						}
						else
						{
							return WriteLine( "OK capa run quit");
							m_state = EnterCommand;
							continue;
						}
					case quit:
						if (m_argBuffer.argc())
						{
							m_state = ProtocolError;
							return WriteLine( "BAD command arguments");
						}
						else
						{
							m_state = Terminate;
							return WriteLine( "BYE");
						}
					case run:
						if (!m_processor.getCommand( "run", m_functionName, m_functionHasIO))
						{
							LOG_ERROR << "Command for 'run' not defined in configuration";
							m_state = ProtocolError;
							return WriteLine( "BAD command not defined");
						}
						if (m_functionHasIO)
						{
							m_inputfilter.reset( m_system.createDefaultInputFilter());
							m_formatoutput.reset( m_system.createDefaultFormatOutput());
							passInput();
							m_formatoutput->init( m_output.ptr(), m_output.size());
							m_processor.setIO( m_inputfilter, m_formatoutput);
						}
						else
						{
							m_inputfilter.reset();
							m_formatoutput.reset();
						}
						m_state = Processing;
						continue;
				}
			}

			case Processing:
			{
				const char** argv = m_argBuffer.argv( m_functionName);
				unsigned int argc = m_argBuffer.argc( m_functionName);

				switch (m_processor.call( argc, argv))
				{
					case lua::AppProcessor::YieldRead:
						m_input.setPos( 0);
						return net::ReadData( m_input.ptr(), m_input.size());

					case lua::AppProcessor::YieldWrite:
					{
						void* content = m_formatoutput->ptr();
						unsigned int contentsize = m_formatoutput->pos();
						if (!m_functionHasIO)
						{
							LOG_WARNING << "output of function '" << m_functionName << "' that has no IO configured is ignored";
							contentsize = 0;
						}
						else if (contentsize == 0)
						{
							LOG_ERROR << "buffer too small for one output element";
							m_state = DiscardInput;
							return WriteLine( "\r\n.\r\nERR");
						}
						m_formatoutput->init( m_output.ptr(), m_output.size());
						return net::SendData( content, contentsize);
					}

					case lua::AppProcessor::Ok:
					{
						if (m_functionHasIO)
						{
							m_state = FlushOutput;
							void* content = m_formatoutput->ptr();
							unsigned int contentsize = m_formatoutput->pos();

							m_formatoutput->init( m_output.ptr(), m_output.size());
							if (contentsize)
							{
								if (!m_functionHasIO)
								{
									LOG_WARNING << "output of function '" << m_functionName << "' that has no IO configured is ignored";
									contentsize = 0;
								}
								else
								{
									m_formatoutput->init( m_output.ptr(), m_output.size());
									return net::SendData( content, contentsize);
								}
							}
							continue;
						}
						else
						{
							m_state = Init;
							return WriteLine( "\r\nOK");
						}
					}

					case lua::AppProcessor::Error:
					{
						if (m_functionHasIO)
						{
							m_state = DiscardInput;
							return WriteLine( "\r\n.\r\nERR");
						}
						else
						{
							m_state = Init;
							return WriteLine( "\r\nERR");
						}
					}
				}
			}

			case FlushOutput:
			{
				m_state = DiscardInput;
				return WriteLine( "\r\n.\r\nOK");
			}

			case DiscardInput:
			{
				if (m_input.gotEoD())
				{
					m_state = EnterCommand;
					continue;
				}
				else
				{
					m_input.setPos( 0);
					return net::ReadData( m_input.ptr(), m_input.size());
				}
			}

			case ProtocolError:
			{
				if (!ProtocolParser::skipLine( m_itr, m_end) || !ProtocolParser::consumeEOLN( m_itr, m_end))
				{
					m_input.setPos( 0);
					return net::ReadData( m_input.ptr(), m_input.size());
				}
				m_state = Init;
				continue;
			}

			case Terminate:
			{
				return net::CloseConnection();
			}
		}//switch(..)
	}//for(;;)
	return net::CloseConnection();
}


Connection::Connection( const net::LocalEndpoint& local, const lua::Configuration* config)
	:m_state(Init)
	,m_argBuffer(&m_buffer)
	,m_input(config->input_bufsize())
	,m_output(config->output_bufsize())
	,m_processor(m_system, config)
	,m_functionName(0)
	,m_functionHasIO(false)
{
	m_itr = m_input.begin();
	m_end = m_input.end();
	LOG_TRACE << "Created connection handler for " << local.toString();
}

Connection::~Connection()
{
	LOG_TRACE << "Connection handler destroyed";
}

void Connection::setPeer( const net::RemoteEndpoint& remote)
{
	LOG_TRACE << "Peer set to " << remote.toString();
}

net::connectionHandler* ServerHandler::ServerHandlerImpl::newConnection( const net::LocalEndpoint& local )
{
	return new iproc::Connection( local, m_config->m_appConfig);
}

ServerHandler::ServerHandler( const HandlerConfiguration* cfg )
	:impl_( new ServerHandlerImpl( cfg) ) {}

ServerHandler::~ServerHandler()
{
	delete impl_;
}

net::connectionHandler* ServerHandler::newConnection( const net::LocalEndpoint& local )
{
	return impl_->newConnection( local );
}

