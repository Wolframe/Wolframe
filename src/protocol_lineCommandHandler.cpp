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
#include "protocol/lineCommandHandler.hpp"
#include "protocol/ioblocks.hpp"
#include "logger-v1.hpp"

using namespace _Wolframe;
using namespace _Wolframe::protocol;

LineCommandHandler::LineCommandHandler( const LineCommandHandlerSTM* stm_)
	:m_stm(stm_),m_argBuffer(&m_buffer),m_cmdstateidx(Init),m_stateidx(0),m_cmdidx(-1)
{
	m_itr = m_input.begin();
	m_end = m_input.end();
}

void LineCommandHandler::setInputBuffer( void* buf, std::size_t allocsize, std::size_t size, std::size_t itrpos)
{
	m_input = protocol::InputBlock( (char*)buf, allocsize, size);
	m_itr = m_input.at(itrpos);
	m_end = m_input.end();
}

void LineCommandHandler::setOutputBuffer( void* buf, std::size_t size, std::size_t pos)
{
	if (size < 16) throw std::logic_error("output buffer smaller than 16 bytes");
	m_output = protocol::OutputBlock( buf, size, pos);
}

void LineCommandHandler::putInput( const void *begin, std::size_t bytesTransferred)
{
	m_input.setPos( bytesTransferred + ((const char*)begin - m_input.charptr()));
	m_itr = m_input.begin();
	m_end = m_input.end();
}

void LineCommandHandler::getInputBlock( void*& begin, std::size_t& maxBlockSize)
{
	if (!m_input.getNetworkMessageRead( begin, maxBlockSize))
	{
		throw std::logic_error( "buffer too small");
	}
}

void LineCommandHandler::getOutput( const void*& begin, std::size_t& bytesToTransfer)
{
	begin = m_output.ptr();
	bytesToTransfer = m_output.pos();
	m_output.setPos(0);
}

void LineCommandHandler::getDataLeft( const void*& begin, std::size_t& nofBytes)
{
	begin = (char*)(m_input.charptr() + (m_itr - m_input.begin()));
	nofBytes = m_end - m_itr;
}

CommandHandler::Operation LineCommandHandler::nextOperation()
{
	for (;;)
	{
		switch( m_cmdstateidx)
		{
			case Init:
				m_argBuffer.clear();
				if (m_output.pos()) return WRITE;
				m_cmdstateidx = EnterCommand;
				/* no break here !*/

			case EnterCommand:
			{
				const LineCommandHandlerSTM::State& st = (*m_stm).get( m_stateidx);
				m_cmdidx = st.m_parser.getCommand( m_itr, m_end, m_buffer)-1;
				if (m_cmdidx < (int)st.m_cmds.size())
				{
					m_cmdstateidx = ParseArgs;
					continue;
				}
				else if (m_itr == m_end)
				{
					return READ;
				}
				else
				{
					m_output.print( "BAD command\r\n", 5);
					m_cmdstateidx = ProtocolError;
					return WRITE;
				}
			}

			case ParseArgs:
			{
				if (!protocol::Parser::getLine( m_itr, m_end, m_argBuffer))
				{
					if (m_itr == m_end)
					{
						return READ;
					}
					else
					{
						m_output.print( "BAD argument\r\n", 5);
						m_cmdstateidx = ProtocolError;
						return WRITE;
					}
				}
				m_cmdstateidx = ParseArgsEOL;
				continue;
			}

			case ParseArgsEOL:
			{
				if (!protocol::Parser::consumeEOL( m_itr, m_end))
				{
					if (m_itr == m_end)
					{
						return READ;
					}
					else
					{
						m_output.print( "BAD line\r\n", 5);
						m_cmdstateidx = ProtocolError;
						return WRITE;
					}
				}
				if (m_cmdidx < 0)
				{
					m_output.print( "BAD command\r\n", 5);
					m_cmdstateidx = ProtocolError;
					return WRITE;

					if (m_argBuffer.argc())
					{
						m_output.print( "BAD command\r\n", 5);
						m_cmdstateidx = ProtocolError;
						return WRITE;
					}
					else
					{
						m_cmdstateidx = Init;
						continue;
					}
				}
				else
				{
					std::ostringstream out;
					try
					{
						int rt = m_stm->runCommand( m_stateidx, (std::size_t)m_cmdidx, this, m_argBuffer.argc(), m_argBuffer.argv(), out);
						if (rt < 0)
						{
							m_cmdstateidx = Terminate;
							if (!m_output.print( out.str().c_str(), out.str().size()))
							{
								LOG_ERROR << "internal: output buffer to small (" << out.str().size() << ")";
								return CLOSED;
							}
							return WRITE;
						}
					}
					catch (std::exception& e)
					{
						LOG_ERROR << "command execution thrown exception: " << e.what();
						m_cmdstateidx = Terminate;
						return CLOSED;
					}
					return (out.str().size())?WRITE:READ;
				}
			}

			case ProtocolError:
			{
				if (!protocol::Parser::skipLine( m_itr, m_end) || !protocol::Parser::consumeEOL( m_itr, m_end))
				{
					return READ;
				}
				m_buffer.clear();
				m_argBuffer.clear();
				m_cmdstateidx = EnterCommand;
				continue;
			}

			case Terminate:
			{
				return CLOSED;
			}
		}//switch(..)
	}//for(;;)
}


