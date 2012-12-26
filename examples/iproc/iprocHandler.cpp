/************************************************************************

 Copyright (C) 2011, 2012 Project Wolframe.
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
#include "logger-v1.hpp"
#include <stdexcept>

using namespace _Wolframe;
using namespace _Wolframe::iproc;

const net::NetworkOperation Connection::WriteLine( const char* str, const char* arg)
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

void Connection::networkInput( const void* dt, std::size_t nofBytes)
{
	m_input.setPos( nofBytes + ((const char*)dt - m_input.charptr()));
	m_itr = m_input.begin();
	m_end = m_input.end();

	if (m_cmdhandler.get())
	{
		m_cmdhandler.get()->putInput( dt, nofBytes);
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

	if (m_cmdhandler.get())
	{
		m_cmdhandler.get()->getInputBlock( pp, ppsize);
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
				m_state = EnterCommand;
				m_buffer.clear();
				m_argBuffer.clear();
				return WriteLine( "OK expecting command");
			}

			case EnterCommand:
			{
				//the empty command is for an empty line for not bothering the client with obscure error messages.
				//the next state should read one character for sure otherwise it may result in an endless loop
				m_cmdidx = m_parser.getCommand( m_itr, m_end, m_buffer);
				switch (m_cmdidx)
				{
					case empty:
					case capa:
					case quit:
					{
						m_state = ParseArgs;
						continue;
					}
					default:
					{
						if (m_cmdidx >= NofCommands && (unsigned int)(m_cmdidx - NofCommands) < m_config->commands().size())
						{
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
				m_buffer.clear();
				if (!protocol::CmdParser<protocol::Buffer>::getLine( m_itr, m_end, m_argBuffer))
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
				if (!protocol::CmdParser<protocol::Buffer>::consumeEOL( m_itr, m_end))
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
						if (m_argBuffer.argc())
						{
							m_state = ProtocolError;
							return WriteLine( "BAD command");
						}
						else
						{
							m_buffer.clear();
							m_argBuffer.clear();
							m_state = EnterCommand;
							continue;
						}
					case capa:
						if (m_argBuffer.argc())
						{
							m_state = ProtocolError;
							return WriteLine( "BAD command arguments");
						}
						else
						{
							return WriteLine( "OK capa quit", m_parser.capabilities().c_str());
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
					default:
						try
						{
							const char* procname = m_config->commands()[ m_cmdidx - NofCommands].m_procname.c_str();
							if (!m_provider)
							{
								LOG_ERROR << "No processor provider set";
								return net::CloseConnection();
							}
							cmdbind::IOFilterCommandHandler* hnd = m_provider->iofilterhandler( procname);
							if (!hnd)
							{
								LOG_ERROR << "io filter command handler not found for '" << procname << "'";
								return net::CloseConnection();
							}
							langbind::Filter* flt = m_provider->filter( "char", "");
							m_inputfilter = flt->inputfilter();
							m_outputfilter = flt->outputfilter();
							if (!flt)
							{
								LOG_ERROR << "filter 'char' not defined";
								return net::CloseConnection();
							}
							delete flt;
							hnd->setFilter( m_inputfilter);
							hnd->setFilter( m_outputfilter);
							m_cmdhandler.reset( hnd);
							if (!m_cmdhandler.get())
							{
								LOG_ERROR << "Command handler not found for '" << procname << "'";
								return net::CloseConnection();
							}
							m_cmdhandler->passParameters( procname, m_argBuffer.argc(), m_argBuffer.argv());
							m_state = Processing;
							m_cmdhandler->setInputBuffer( m_input.ptr(), m_input.size());
							m_cmdhandler->putInput( m_itr.ptr(), m_end-m_itr);
							m_cmdhandler->setOutputBuffer( m_output.ptr(), m_output.size(), m_output.pos());
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
					switch (m_cmdhandler->nextOperation())
					{
						case cmdbind::CommandHandler::READ:
							return readDataOp();
						break;
						case cmdbind::CommandHandler::WRITE:
							m_cmdhandler->getOutput( content, contentsize);
							return net::SendData( content, contentsize);
						break;
						case cmdbind::CommandHandler::CLOSE:
							m_cmdhandler->getDataLeft( content, contentsize);
							pos = (const char*)content - m_input.charptr();
							m_input.setPos( pos + contentsize);
							m_itr = m_input.at( pos);
							m_end = m_input.end();

							err = m_cmdhandler.get()->lastError();
							m_cmdhandler.reset();
							m_state = EnterCommand;
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

Connection::Connection( const net::LocalEndpoint& local, const Configuration* config)
	:m_state(Init)
	,m_buffer(256)
	,m_argBuffer(&m_buffer)
	,m_input(config->input_bufsize())
	,m_output(config->output_bufsize())
	,m_config(config)
	,m_cmdidx( -1)
	,m_provider(0)
{
	m_itr = m_input.begin();
	m_end = m_input.end();

	m_parser = protocol::CmdParser<protocol::Buffer>( &commandName);
	std::vector<Configuration::Command>::const_iterator itr=m_config->commands().begin(), end=m_config->commands().end();
	for (; itr!=end; ++itr)
	{
		m_parser.add( itr->m_cmdname);
	}
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

net::ConnectionHandler* ServerHandler::ServerHandlerImpl::newConnection( const net::LocalEndpoint& local)
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

net::ConnectionHandler* ServerHandler::newConnection( const net::LocalEndpoint& local)
{
	return m_impl->newConnection( local);
}


