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
#include "execProtocolHandler.hpp"
#include "processor/execContext.hpp"
#include "tprocProtocolFiles.hpp"
#include "logger-v1.hpp"

#undef _Wolframe_LOWLEVEL_DEBUG

using namespace _Wolframe;
using namespace _Wolframe::cmdbind;
using namespace _Wolframe::protocol;

ExecProtocolHandler::ExecProtocolHandler( const std::vector<std::string>& rcmds_, const std::vector<Command>& cmds_)
	:m_state(Init),m_writeptr(0),m_writesize(0),m_writepos(0),m_cmdidx(-1)
{
	m_arg[0] = 0;
	m_arg[1] = 0;
	m_itr = m_input.begin();
	m_end = m_input.begin();

	for (std::vector<std::string>::const_iterator ii=rcmds_.begin(),ee=rcmds_.end(); ii!=ee; ++ii)
	{
		if (ii->size() > 0) m_parser.add( *ii);			//... do not add empty command
	}
	m_nofParentCmds = (int)m_parser.size();
	m_parser.add( "CAPA");						//... cmd[m_nofParentCmds] => CAPAbilities
	m_parser.add( "");						//... cmd[m_nofParentCmds+1] => empty command

	std::vector<Command>::const_iterator itr=cmds_.begin(), end=cmds_.end();
	for (; itr!=end; ++itr)
	{
		m_parser.add( itr->m_cmdname);
		m_cmds.push_back( itr->m_procname);
	}
}

ExecProtocolHandler::~ExecProtocolHandler()
{}

void ExecProtocolHandler::setInputBuffer( void* buf, std::size_t allocsize)
{
	m_input = InputBlock( (char*)buf, allocsize);
}

void ExecProtocolHandler::setOutputBuffer( void* buf, std::size_t size, std::size_t pos)
{
	if (size < 16) throw std::logic_error("output buffer smaller than 16 bytes");
	m_output = OutputBlock( buf, size, pos);
}

void ExecProtocolHandler::putInput( const void *begin, std::size_t bytesTransferred)
{
	if (m_protocolHandler.get())
	{
		m_protocolHandler->putInput( begin, bytesTransferred);
	}
	else
	{
		m_input.setPos( bytesTransferred + ((const char*)begin - m_input.charptr()));
		m_itr = m_input.begin() + ((const char*)begin - m_input.charptr());
		m_end = m_input.end();
	}
}

void ExecProtocolHandler::getInputBlock( void*& begin, std::size_t& maxBlockSize)
{
	if (m_protocolHandler.get())
	{
		m_protocolHandler->getInputBlock( begin, maxBlockSize);
	}
	else if (!m_input.getNetworkMessageRead( begin, maxBlockSize))
	{
		throw std::logic_error( "buffer too small for input");
	}
}

void ExecProtocolHandler::getOutput( const void*& begin, std::size_t& bytesToTransfer)
{
	if (m_protocolHandler.get())
	{
		begin = m_writeptr + m_writepos;
		bytesToTransfer = m_writesize - m_writepos;
		if (m_output.size() - m_output.pos() < bytesToTransfer)
		{
			bytesToTransfer = m_output.size() - m_output.pos();
		}
		if (!m_output.print( begin, bytesToTransfer))
		{
			throw std::bad_alloc();
		}
		m_writepos += bytesToTransfer;
	}
	begin = m_output.ptr();
	bytesToTransfer = m_output.pos();
	m_output.setPos(0);
}

void ExecProtocolHandler::getDataLeft( const void*& begin, std::size_t& nofBytes)
{
	if (m_protocolHandler.get())
	{
		m_protocolHandler->getDataLeft( begin, nofBytes);
	}
	else
	{
		begin = (char*)(m_input.charptr() + (m_itr - m_input.begin()));
		nofBytes = m_end - m_itr;
	}
}

const char* ExecProtocolHandler::getCommand( int& argc, const char**& argv)
{
	if (m_cmdidx < 0) return 0;
	if (m_cmdidx < m_nofParentCmds)
	{
		const char* rt = m_parser.getcmd( m_cmdidx);
		argc = m_argBuffer.size()?1:0;
		if (argc)
		{
			m_arg[0] = m_argBuffer.c_str();
		}
		else
		{
			m_arg[0] = 0;
		}
		argv = m_arg;
		return rt;
	}
	else
	{
		argc = 0;
		argv = 0;
		return 0;
	}
}

ProtocolHandler::Operation ExecProtocolHandler::nextOperation()
{
	for (;;)
	{
#ifdef _Wolframe_LOWLEVEL_DEBUG
		LOG_TRACE << "STATE ExecProtocolHandler " << stateName( m_state);
		// to blurry log message. Helped in the beginning, but got now useless
#endif
		switch( m_state)
		{
			case Init:
			{
				//start:
				m_state = EnterCommand;
				m_argBuffer.clear();
				m_output.print( "OK enter cmd\r\n");
				m_capastr = m_parser.capabilities();
				m_capaitr = 0;
				return WRITE;
			}

			case EnterCommand:
			{
				//the empty command is for an empty line for not bothering the client with obscure error messages.
				//the next state should read one character for sure otherwise it may result in an endless loop
				m_cmdidx = m_parser.getCommand( m_itr, m_end, m_argBuffer);
				if (m_cmdidx < 0)
				{
					if (m_itr == m_end)
					{
						return READ;
					}
					else
					{
						m_state = ProtocolError;
						m_output.print( "ERR unknown command\r\n");
						return WRITE;
					}
				}
				else
				{
					m_argBuffer.clear();
					m_state = ParseArgs;
					continue;
				}
			}

			case ParseArgs:
			{
				if (!CmdParser<Buffer>::getLine( m_itr, m_end, m_argBuffer))
				{
					if (m_itr == m_end)
					{
						return READ;
					}
					else
					{
						m_state = ProtocolError;
						m_output.print( "ERR unexpected arguments\r\n");
						return WRITE;
					}
				}
				m_state = ParseArgsEOL;
				continue;
			}

			case ParseArgsEOL:
			{
				if (!CmdParser<Buffer>::consumeEOL( m_itr, m_end))
				{
					if (m_itr == m_end)
					{
						return READ;
					}
					else
					{
						m_state = ProtocolError;
						m_output.print( "ERR unknown command\r\n");
						return WRITE;
					}
				}
				if (m_cmdidx < m_nofParentCmds)
				{
					m_state = Terminate;
					return CLOSE;
				}
				else if (m_cmdidx == m_nofParentCmds)
				{
					///.. CAPAbilities
					m_capastr = m_parser.capabilities() + "\r\n";
					m_capaitr = 0;
					m_state = PrintCapabilities;
					continue;
				}
				else if (m_cmdidx == m_nofParentCmds+1)
				{
					///.. empty command
					if (m_argBuffer.size())
					{
						m_state = ProtocolError;
						m_output.print( "ERR empty command with arguments\r\n");
						return WRITE;
					}
					else
					{
						m_argBuffer.clear();
						m_state = EnterCommand;
						continue;
					}
				}
				else if (m_cmdidx == (int)m_parser.size())
				{
					m_state = ProtocolError;
					m_output.print( "ERR unknown command\r\n");
					return WRITE;
				}
				else
				{
					try
					{
						const std::string& procname = m_cmds[ m_cmdidx - m_nofParentCmds - 2];
						if (!execContext() || !execContext()->provider())
						{
							LOG_ERROR << "Processor provider undefined";
							m_output.print( "ERR command not defined\r\n");
							m_state = EnterCommand;
							m_argBuffer.clear();
							return WRITE;
						}
						cmdbind::CommandHandlerR cmdhnd( execContext()->provider()->cmdhandler( procname, ""));
						if (!cmdhnd.get())
						{
							LOG_ERROR << "Command handler not found for '" << procname << "'";
							m_output.print( "ERR command not defined\r\n");
							m_state = EnterCommand;
							m_argBuffer.clear();
							return WRITE;

						}
						cmdhnd->setExecContext( execContext());
						m_protocolHandler.reset( new EscDlfProtocolHandler( cmdhnd));
						m_protocolHandler->setExecContext( execContext());
						m_protocolHandler->setArgumentString( m_argBuffer);
						m_state = Processing;
						m_protocolHandler->setInputBuffer( m_input.ptr(), m_input.size());
						m_protocolHandler->putInput( m_itr.ptr(), m_end-m_itr);
						m_protocolHandler->setOutputBuffer( m_output.ptr(), m_output.size(), m_output.pos());
					}
					catch (std::exception& e)
					{
						LOG_ERROR << "Command handler creation thrown exception: " << e.what();
						setLastError( "failed to execute command");
						return CLOSE;
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
						case READ:
							return READ;
						break;
						case WRITE:
							m_state = FlushingOutput;
							m_protocolHandler->getOutput( content, m_writesize);
							m_writeptr = (const char*)content;
							m_writepos = 0;
							return WRITE;
						break;
						case CLOSE:
							m_protocolHandler->getDataLeft( content, contentsize);
							pos = (const char*)content - m_input.charptr();
							m_input.setPos( pos + contentsize);
							m_itr = m_input.at( pos);
							m_end = m_input.end();
							err = m_protocolHandler->lastError();
							m_protocolHandler.reset();
							m_state = EnterCommand;
							if (err)
							{
								m_output.print( "ERR ");
								m_output.print( err);
								m_output.print( "\r\n");
								return WRITE;
							}
							else
							{
								m_output.print( "OK\r\n");
								return WRITE;
							}
						break;
					}
				}
				catch (std::exception& e)
				{
					LOG_ERROR << "Command execution thrown exception: " << e.what();
					setLastError( "failed to execute command");
					return CLOSE;
				}
			}
			case FlushingOutput:
				if (m_writepos == m_writesize)
				{
					m_state = Processing;
					m_writeptr = 0;
					m_writesize = 0;
					m_writepos = 0;
					continue;
				}
				return WRITE;

			case ProtocolError:
			{
				if (!CmdParser<Buffer>::skipLine( m_itr, m_end)
				||  !CmdParser<Buffer>::consumeEOL( m_itr, m_end))
				{
					return READ;
				}
				m_state = Init;
				continue;
			}

			case PrintCapabilities:
			{
				std::size_t rr = m_output.restsize();
				if (m_capaitr == m_capastr.size())
				{
					m_state = Init;
					continue;
				}
				if (rr > m_capastr.size() - m_capaitr)
				{
					rr = m_capastr.size() - m_capaitr;
				}
				if (!m_output.print( m_capastr.c_str()+m_capaitr, rr))
				{
					throw std::logic_error( "protocol error");
				}
				m_capaitr += rr;
				return WRITE;
			}

			case Terminate:
			{
				return CLOSE;
			}
		}//switch(..)
	}//for(;;)

	return CLOSE;
}


