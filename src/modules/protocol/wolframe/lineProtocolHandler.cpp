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
#include "lineProtocolHandler.hpp"
#include "processor/execContext.hpp"
#include "ioblocks.hpp"
#include "logger-v1.hpp"

#undef _Wolframe_LOWLEVEL_DEBUG

using namespace _Wolframe;
using namespace _Wolframe::cmdbind;
using namespace _Wolframe::protocol;

LineProtocolHandler::LineProtocolHandler( const LineProtocolHandlerSTM* stm_, std::size_t stateidx_)
	:m_delegateHandler(0),m_delegateHandlerEnd(0),m_stm(stm_),m_argBuffer(&m_buffer),m_cmdstateidx(Init),m_stateidx(stateidx_),m_cmdidx(-1),m_resultstate(-1),m_resultitr(0),m_refuseInputFlag(false)
{
	m_itr = m_input.begin();
	m_end = m_input.end();
}

LineProtocolHandler::~LineProtocolHandler()
{
	if (m_delegateHandler) delete m_delegateHandler;
}

void LineProtocolHandler::setInputBuffer( void* buf, std::size_t allocsize)
{
	m_input = protocol::InputBlock( (char*)buf, allocsize);
	m_itr = m_end = m_input.begin();
	if (m_delegateHandler && m_cmdstateidx == ProcessingDelegation)
	{
		m_delegateHandler->setInputBuffer( buf, allocsize);
	}
}

void LineProtocolHandler::setOutputBuffer( void* buf, std::size_t size, std::size_t pos)
{
	if (size < 16) throw std::logic_error("output buffer smaller than 16 bytes");
	m_output = protocol::OutputBlock( buf, size, pos);
	if (m_delegateHandler && m_cmdstateidx == ProcessingDelegation)
	{
		m_delegateHandler->setOutputBuffer( buf, size, pos);
	}
}

void LineProtocolHandler::putInput( const void *begin, std::size_t bytesTransferred)
{
	if (m_delegateHandler && m_cmdstateidx == ProcessingDelegation)
	{
		m_delegateHandler->putInput( begin, bytesTransferred);
	}
	else
	{
		m_input.setPos( bytesTransferred + ((const char*)begin - m_input.charptr()));
		m_itr = m_input.begin() + ((const char*)begin - m_input.charptr());
		m_end = m_input.end();
	}
}

void LineProtocolHandler::getInputBlock( void*& begin, std::size_t& maxBlockSize)
{
	if (m_delegateHandler && m_cmdstateidx == ProcessingDelegation)
	{
		m_delegateHandler->getInputBlock( begin, maxBlockSize);
	}
	else if (!m_input.getNetworkMessageRead( begin, maxBlockSize))
	{
		throw std::logic_error( "buffer too small for input");
	}
}

void LineProtocolHandler::getOutput( const void*& begin, std::size_t& bytesToTransfer)
{
	if (m_delegateHandler && m_cmdstateidx == ProcessingDelegation)
	{
		m_delegateHandler->getOutput( begin, bytesToTransfer);
	}
	else
	{
		begin = m_output.ptr();
		bytesToTransfer = m_output.pos();
		m_output.setPos(0);
	}
}

void LineProtocolHandler::getDataLeft( const void*& begin, std::size_t& nofBytes)
{
	if (m_delegateHandler && m_cmdstateidx == ProcessingDelegation)
	{
		m_delegateHandler->getDataLeft( begin, nofBytes);
	}
	else
	{
		begin = (char*)(m_input.charptr() + (m_itr - m_input.begin()));
		nofBytes = m_end - m_itr;
	}
}

const char* LineProtocolHandler::interruptDataSessionMarker() const
{
	if (m_delegateHandler && m_cmdstateidx == ProcessingDelegation)
	{
		return m_delegateHandler->interruptDataSessionMarker();
	}
	return "";
}

int LineProtocolHandler::runCommand( const char* cmd_, int argc_, const char** argv_, std::ostream& out_)
{
	char* begin=const_cast<char*>(cmd_);
	char* end=begin+strlen(cmd_)+1;
	std::string cmdbuf;
	int cmdidx = m_stm->get( m_stateidx).m_parser.getCommand( begin, end, cmdbuf);
	if (cmdidx == 0) throw std::logic_error( "protocol error: redirected to empty command");
	if (cmdidx < 0) throw std::logic_error( "protocol error: redirected to unknown command");
	return m_stm->runCommand( m_stateidx, (std::size_t)cmdidx-1, this, argc_, argv_, out_);
}

bool LineProtocolHandler::redirectInput( void* data, std::size_t datasize, cmdbind::ProtocolHandler* toh, std::ostream& out)
{
	bool rt = true;
	const void* toh_output;
	std::size_t toh_outputsize;
	const char* error;
	toh->setInputBuffer( data, datasize);
	toh->putInput( data, datasize);
	toh->setOutputBuffer( m_output.ptr(), m_output.size(), m_output.pos());

	for (;;) switch (toh->nextOperation())
	{
		case cmdbind::ProtocolHandler::READ:
			toh->setInputBuffer( m_input.ptr(), m_input.size());
			return true;
		case cmdbind::ProtocolHandler::WRITE:
			toh->getOutput( toh_output, toh_outputsize);
			out << std::string( (const char*)toh_output, toh_outputsize);
			continue;
		case cmdbind::ProtocolHandler::CLOSE:
		{
			error = toh->lastError();
			if (error)
			{
				LOG_ERROR << "error redirect input: " << error;
				rt = false;
			}
			toh->getDataLeft( toh_output, toh_outputsize);
			if (toh_outputsize)
			{
				if (toh_outputsize > m_input.size())
				{
					throw std::runtime_error("internal: data chunk to feed exceeds size of buffer. Can not happen");
				}
				std::memmove( m_input.charptr(), toh_output, toh_outputsize);
				m_input.setPos( toh_outputsize);
				m_end = m_input.begin() + toh_outputsize;
				m_itr = m_input.begin();
				m_refuseInputFlag = true;
			}
			return rt;
		}
	}
}

ProtocolHandler::Operation LineProtocolHandler::nextOperation()
{
	for (;;)
	{
#ifdef _Wolframe_LOWLEVEL_DEBUG
		LOG_TRACE << "STATE LineProtocolHandler " << commandStateName( m_cmdstateidx);
		// to blurry log message. Helped in the beginning, but got now useless
#endif
		/*[-]*/std::cout << "STATE LineProtocolHandler " << commandStateName( m_cmdstateidx) << std::endl;
		switch( m_cmdstateidx)
		{
			case Init:
				//\todo Rethink model of protocol processing delegation. Got too many if's
				m_argBuffer.clear();
				if (m_output.pos()) return WRITE;
				m_cmdstateidx = EnterCommand;
				m_cmdidx = -1;
				m_resultstate = -1;
				m_resultstr.clear();
				m_resultitr = 0;
				if (m_delegateHandler)
				{
					const void* data = (const void*)(m_input.charptr() + (m_itr-m_input.begin()));
					std::size_t datasize = m_end-m_itr;
					if (datasize > m_input.size())
					{
						throw std::runtime_error("internal: data chunk to feed exceeds size of buffer. Can not happen");
					}
					std::memmove( m_input.charptr(), data, datasize);
					m_delegateHandler->setInputBuffer( m_input.ptr(), m_input.size());
					m_end = m_input.begin() + (m_end-m_itr);
					m_itr = m_input.begin();
					m_delegateHandler->putInput( m_itr.ptr(), m_end-m_itr);
					m_delegateHandler->setOutputBuffer( m_output.ptr(), m_output.size(), m_output.pos());
					m_cmdstateidx = ProcessingDelegation;
				}
				continue;

			case ProcessingDelegation:
				if (m_delegateHandler)
				{
					ProtocolHandler::Operation delegateRes = m_delegateHandler->nextOperation();
					if (delegateRes == CLOSE)
					{
						/*[-]*/std::cout << "GOT CLOSE" << std::endl;
						try
						{
							std::ostringstream out;
							const void* r_begin;
							std::size_t r_nofBytes;
							m_delegateHandler->getDataLeft( r_begin, r_nofBytes);
							ProtocolHandler* l_delegateHandler = m_delegateHandler;
							DelegateHandlerEnd l_delegateHandlerEnd = m_delegateHandlerEnd;
							m_delegateHandler = 0;
							m_delegateHandlerEnd = 0;
							m_cmdstateidx = ProcessOutput;
							m_resultstate = (*l_delegateHandlerEnd)( (void*)this, l_delegateHandler, out);
							if (!m_refuseInputFlag)
							{
								putInput( r_begin, r_nofBytes);
							}
							m_resultstr = out.str();
							m_resultitr = 0;
							continue;
						}
						catch (std::exception& e)
						{
							LOG_ERROR << "command delegation termination method thrown exception: " << e.what();
							m_cmdstateidx = Terminate;
							return CLOSE;
						}
					}
					else
					{
						return delegateRes;
					}
				}
				else
				{
					LOG_ERROR << "protocol error";
					m_cmdstateidx = Terminate;
					return CLOSE;
				}

			case EnterCommand:
			{
				m_refuseInputFlag = false;
				const LineProtocolHandlerSTM::State& st = (*m_stm).get( m_stateidx);
				int ci = st.m_parser.getCommand( m_itr, m_end, m_buffer);
				if (ci == -1)
				{
					if (m_itr == m_end)
					{
						return READ;
					}
					else
					{
						m_output.print( "ERR unknown command\r\n");
						m_cmdstateidx = ProtocolError;
						m_buffer.clear();
						return WRITE;
					}
				}
				else
				{
					/*[-]*/std::cout << "COMMAND '" << m_buffer << "'" << std::endl;
					m_cmdidx = ci;
					m_cmdstateidx = ParseArgs;
					m_argBuffer.clear();
					continue;
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
						m_output.print( "ERR bad arguments\r\n");
						m_cmdstateidx = ProtocolError;
						return WRITE;
					}
				}
				else
				{
					m_cmdstateidx = ParseArgsEOL;
					continue;
				}
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
						m_output.print( "ERR bad command line\r\n");
						m_cmdstateidx = ProtocolError;
						return WRITE;
					}
				}
				else if (m_cmdidx == 0)
				{
					if (m_argBuffer.argc() > 0)
					{
						///...line beginning with space
						m_output.print( "ERR command line starting with spaces\r\n");
						m_cmdstateidx = ProtocolError;
						return WRITE;
					}
					else
					{
						///...empty line
						m_cmdstateidx = EnterCommand;
						continue;
					}
				}
				try
				{
					std::ostringstream out;
					m_resultstate = m_stm->runCommand( m_stateidx, (std::size_t)m_cmdidx-1, this, m_argBuffer.argc(), m_argBuffer.argv(), out);
					m_cmdstateidx = ProcessOutput;
					m_resultstr = out.str();
					m_resultitr = 0;
					continue;
				}
				catch (std::exception& e)
				{
					LOG_ERROR << "command execution thrown exception: " << e.what();
					m_cmdstateidx = Terminate;
					return CLOSE;
				}
			}

			case ProcessOutput:
			{
				std::size_t rr = m_output.restsize();
				if (m_resultitr == m_resultstr.size())
				{
					if (m_resultstate < 0)
					{
						m_cmdstateidx = Terminate;
						return CLOSE;
					}
					else
					{
						m_stateidx = (std::size_t)m_resultstate;
						m_cmdstateidx = Init;
						continue;
					}
				}
				if (rr > m_resultstr.size() - m_resultitr)
				{
					rr = m_resultstr.size() - m_resultitr;
				}
				if (!m_output.print( m_resultstr.c_str()+m_resultitr, rr))
				{
					throw std::logic_error( "protocol error");
				}
				m_resultitr += rr;
				return WRITE;
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
				return CLOSE;
			}
		}//switch(..)
	}//for(;;)
}


