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
/*[-]*/#include <iostream>

using namespace _Wolframe;
using namespace _Wolframe::protocol;

LineCommandHandlerBase::LineCommandHandlerBase()
{
	m_itr = m_input.begin();
	m_end = m_input.end();
}

void LineCommandHandlerBase::setInputBuffer( void* buf, std::size_t allocsize, std::size_t size, std::size_t itrpos)
{
	m_input = protocol::InputBlock( (char*)buf, allocsize, size);
	m_itr = m_input.at(itrpos);
	m_end = m_input.end();
}

void LineCommandHandlerBase::setOutputBuffer( void* buf, std::size_t size, std::size_t pos)
{
	m_output = protocol::OutputBlock( buf, size, pos);
}

void LineCommandHandlerBase::putInput( const void *begin, std::size_t bytesTransferred)
{
	m_input.setPos( bytesTransferred + ((const char*)begin - m_input.charptr()));
	m_itr = m_input.begin();
	m_end = m_input.end();
}

void LineCommandHandlerBase::getInputBlock( void*& begin, std::size_t& maxBlockSize)
{
	if (!m_input.getNetworkMessageRead( begin, maxBlockSize))
	{
		throw std::logic_error( "buffer too small");
	}
}

void LineCommandHandlerBase::getOutput( const void*& begin, std::size_t& bytesToTransfer)
{
	begin = m_output.ptr();
	bytesToTransfer = m_output.pos();
	m_output.setPos(0);
}

void LineCommandHandlerBase::getDataLeft( const void*& begin, std::size_t& nofBytes)
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
			case EnterCommand:
			{
				const LineCommandHandlerSTM::State& st = (*m_stm)[ m_stateidx];
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
					m_cmdstateidx = ProtocolError;
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
						const LineCommandHandlerSTM::State& st = (*m_stm)[ m_stateidx];
						m_cmdstateidx = ProtocolError;
						m_stateidx = st.m_runUnknown( this, m_argBuffer.argc(), m_argBuffer.argv(), m_output);
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
						const LineCommandHandlerSTM::State& st = (*m_stm)[ m_stateidx];
						m_cmdstateidx = ProtocolError;
						try
						{
							m_stateidx = st.m_runUnknown( this, m_argBuffer.argc(), m_argBuffer.argv(), m_output);
						}
						catch (std::exception& e)
						{
							LOG_ERROR << "command execution thrown exception: " << e.what();
							m_cmdstateidx = Terminate;
							return CLOSED;
						}
					}
				}
				if (m_cmdidx < 0)
				{
					if (m_argBuffer.argc())
					{
						const LineCommandHandlerSTM::State& st = (*m_stm)[ m_stateidx];
						m_cmdstateidx = ProtocolError;
						try
						{
							m_stateidx = st.m_runUnknown( this, m_argBuffer.argc(), m_argBuffer.argv(), m_output);
						}
						catch (std::exception& e)
						{
							LOG_ERROR << "command execution thrown exception: " << e.what();
							m_cmdstateidx = Terminate;
							return CLOSED;
						}
						continue;
					}
					else
					{
						m_buffer.clear();
						m_argBuffer.clear();
						m_cmdstateidx = EnterCommand;
						continue;
					}
				}
				else
				{
					const LineCommandHandlerSTM::State& st = (*m_stm)[ m_stateidx];
					try
					{
						m_stateidx = st.m_cmds[ m_cmdidx-1]( this, m_argBuffer.argc(), m_argBuffer.argv(), m_output);
					}
					catch (std::exception& e)
					{
						LOG_ERROR << "command execution thrown exception: " << e.what();
						m_cmdstateidx = Terminate;
						return CLOSED;
					}
					if (m_stateidx >= m_stm->m_statear.size())
					{
						LOG_ERROR << "illegal state returned by method of statemachine";
						m_cmdstateidx = Terminate;
						return CLOSED;
					}
					else
					{
						return WRITE;
					}
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



