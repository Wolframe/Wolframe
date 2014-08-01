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
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
///\file iprocHandler.cpp
///\brief Implementation of a simple protocol based command handler calling a lus script
#include "iprocHandler.hpp"
#include "processor/procProvider.hpp"
#include "cmdbind/ioFilterCommandHandler.hpp"
#include "iprocProtocolFiles.hpp"
#include "logger-v1.hpp"
#include <stdexcept>

using namespace _Wolframe;
using namespace _Wolframe::iproc;

const net::NetworkOperation Connection::WriteLine( const char* str, const char* arg)
{
	unsigned int ii;
	m_argBuffer.clear();
	for (ii=0; str[ii]; ii++) m_argBuffer.push_back( str[ii]);
	if (arg)
	{
		m_argBuffer.push_back( ' ');
		for (ii=0; arg[ii]; ii++) m_argBuffer.push_back( arg[ii]);
	}
	m_argBuffer.push_back( '\r');
	m_argBuffer.push_back( '\n');
	const char* msg = m_argBuffer.c_str();
	unsigned int msgsize = m_argBuffer.size();
	return net::SendData( msg, msgsize);
}

void Connection::networkInput( const void* dt, std::size_t nofBytes)
{
	m_input.setPos( nofBytes + ((const char*)dt - m_input.charptr()));
	m_itr = m_input.begin();
	m_end = m_input.end();

	if (m_protocolHandler.get())
	{
		m_protocolHandler.get()->putInput( dt, nofBytes);
	}
}


void Connection::signalOccured( NetworkSignal)
{
	LOG_TRACE << "Got signal";
	m_state = Terminate;
}

const net::NetworkOperation Connection::readDataOp()
{
	void* pp;
	std::size_t ppsize;

	if (m_protocolHandler.get())
	{
		m_protocolHandler->getInputBlock( pp, ppsize);
	}
	else if (!m_input.getNetworkMessageRead( pp, ppsize))
	{
		LOG_ERROR << "buffer too small to buffer end of data marker in input";
		throw std::logic_error( "buffer too small");
	}
	return net::ReadData( pp, ppsize);
}

const net::NetworkOperation Connection::nextOperation()
{
	for (;;)
	{
		switch( m_state)
		{
			case Init:
			{
				//start:
				m_state = StartCommand;
				m_argBuffer.clear();
				return WriteLine( "OK expecting command");
			}

			case StartCommand:
				m_state = EnterCommand;
				m_argBuffer.clear();
				/*no break here!*/
			case EnterCommand:
			{
				//the empty command is for an empty line for not bothering the client with obscure error messages.
				//the next state should read one character for sure otherwise it may result in an endless loop
				m_cmdidx = m_parser.getCommand( m_itr, m_end, m_argBuffer);
				switch (m_cmdidx)
				{
					case empty:
					case capa:
					case quit:
					{
						m_argBuffer.clear();
						m_state = ParseArgs;
						continue;
					}
					default:
					{
						if (m_cmdidx >= NofCommands && (unsigned int)(m_cmdidx - NofCommands) < m_config->commands().size())
						{
							m_argBuffer.clear();
							m_state = ParseArgs;
							continue;
						}
						else if (m_itr == m_end)
						{
							return readDataOp();
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
				if (!protocol::CmdParser<std::string>::getLine( m_itr, m_end, m_argBuffer))
				{
					if (m_itr == m_end)
					{
						return readDataOp();
					}
					else
					{
						m_state = ProtocolError;
						return WriteLine( "BAD arguments");
					}
				}
				m_state = ParseArgsEOL;
				continue;
			}

			case ParseArgsEOL:
			{
				if (!protocol::CmdParser<std::string>::consumeEOL( m_itr, m_end))
				{
					if (m_itr == m_end)
					{
						return readDataOp();
					}
					else
					{
						m_state = ProtocolError;
						return WriteLine( "BAD command");
					}
				}
				switch (m_cmdidx)
				{
					case empty:
						if (m_argBuffer.size())
						{
							m_state = ProtocolError;
							return WriteLine( "BAD command");
						}
						else
						{
							m_state = StartCommand;
							continue;
						}
					case capa:
						if (m_argBuffer.size())
						{
							m_state = ProtocolError;
							return WriteLine( "BAD command arguments");
						}
						else
						{
							m_state = StartCommand;
							return WriteLine( "OK capa quit", m_parser.capabilities().c_str());
							continue;
						}
					case quit:
						if (m_argBuffer.size())
						{
							m_state = ProtocolError;
							return WriteLine( "BAD command arguments");
						}
						else
						{
							m_state = Terminate;
							return WriteLine( "BYE");
						}
					default:
						try
						{
							const char* procname = m_config->commands()[ m_cmdidx - NofCommands].m_procname.c_str();
							if (!m_execContext)
							{
								LOG_ERROR << "No execution context set";
								return net::CloseConnection();
							}
							const proc::ProcessorProviderInterface* provider = m_execContext->provider();
							if (!provider)
							{
								LOG_ERROR << "No procesor provider defined";
								return net::CloseConnection();
							}
							cmdbind::CommandHandlerR chnd( provider->cmdhandler( procname, ""));
							if (!chnd.get())
							{
								LOG_ERROR << "command handler not found for '" << procname << "'";
								return net::CloseConnection();
							}
							cmdbind::IOFilterCommandHandler* iochnd = dynamic_cast<cmdbind::IOFilterCommandHandler*>( chnd.get());
							if (!iochnd)
							{
								LOG_ERROR << "command handler for '" << procname << "' is not an iofilter command handler";
								return net::CloseConnection();
							}
							chnd->setExecContext( m_execContext);
							m_protocolHandler.reset( new cmdbind::EscDlfProtocolHandler( chnd));
							const langbind::FilterType* fltp = provider->filterType( "char");
							if (!fltp)
							{
								LOG_ERROR << "failed to load filter 'char' (not defined)";
								return net::CloseConnection();
							}
							langbind::FilterR flt( fltp->create());
							if (!flt)
							{
								LOG_ERROR << "filter 'char' not defined";
								return net::CloseConnection();
							}
							m_inputfilter = flt->inputfilter();
							m_outputfilter = flt->outputfilter();
							iochnd->setInputFilter( m_inputfilter);
							iochnd->setOutputFilter( m_outputfilter);
							m_protocolHandler->setArgumentString( m_argBuffer);
							m_state = Processing;
							m_protocolHandler->setInputBuffer( m_input.ptr(), m_input.size());
							m_protocolHandler->putInput( m_itr.ptr(), m_end-m_itr);
							m_protocolHandler->setOutputBuffer( m_output.ptr(), m_output.size(), m_output.pos());
						}
						catch (std::exception& e)
						{
							LOG_ERROR << "Command handler creation thrown exception: " << e.what();
							return net::CloseConnection();
						}
						continue;
				}
			}

			case Processing:
			{
				const void* content;
				std::size_t contentsize;
				std::size_t pos;
				const char* err;

				try
				{
					switch (m_protocolHandler->nextOperation())
					{
						case cmdbind::ProtocolHandler::READ:
							return readDataOp();
						break;
						case cmdbind::ProtocolHandler::WRITE:
							m_protocolHandler->getOutput( content, contentsize);
							return net::SendData( content, contentsize);
						break;
						case cmdbind::ProtocolHandler::CLOSE:
							m_protocolHandler->getDataLeft( content, contentsize);
							pos = (const char*)content - m_input.charptr();
							m_input.setPos( pos + contentsize);
							m_itr = m_input.at( pos);
							m_end = m_input.end();

							err = m_protocolHandler.get()->lastError();
							m_protocolHandler.reset();
							m_state = StartCommand;
							if (err)
							{
								return WriteLine( "ERR", err);
							}
							else
							{
								return WriteLine( "OK");
							}
						break;
					}
				}
				catch (std::exception& e)
				{
					LOG_ERROR << "Command execution thrown exception: " << e.what();
					return net::CloseConnection();
				}
			}

			case ProtocolError:
			{
				m_argBuffer.clear();
				if (!protocol::CmdParser<protocol::Buffer>::skipLine( m_itr, m_end)
				||  !protocol::CmdParser<protocol::Buffer>::consumeEOL( m_itr, m_end))
				{
					return readDataOp();
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

Connection::Connection( const net::LocalEndpointR& local, const Configuration* config)
	:m_state(Init)
	,m_input(config->input_bufsize())
	,m_output(config->output_bufsize())
	,m_config(config)
	,m_cmdidx( -1)
	,m_execContext(0)
{
	m_itr = m_input.begin();
	m_end = m_input.end();

	m_parser = protocol::CmdParser<std::string>( &commandName);
	std::vector<Configuration::Command>::const_iterator itr=m_config->commands().begin(), end=m_config->commands().end();
	for (; itr!=end; ++itr)
	{
		m_parser.add( itr->m_cmdname);
	}
	LOG_TRACE << "Created connection handler for " << local->toString();
}

Connection::~Connection()
{
	LOG_TRACE << "Connection handler destroyed";
}

void Connection::setPeer( const net::RemoteEndpointR& remote)
{
	LOG_TRACE << "Peer set to " << remote->toString();
}

net::ConnectionHandler* ServerHandler::ServerHandlerImpl::newConnection( const net::LocalEndpointR& local)
{
	return new iproc::Connection( local, m_config->m_appConfig);
}

ServerHandler::ServerHandler( const HandlerConfiguration* cfg,
			      const module::ModulesDirectory* /*modules*/)
	: m_impl( new ServerHandlerImpl( cfg)) {}

ServerHandler::~ServerHandler()
{
	delete m_impl;
}

net::ConnectionHandler* ServerHandler::newConnection( const net::LocalEndpointR& local)
{
	return m_impl->newConnection( local);
}


