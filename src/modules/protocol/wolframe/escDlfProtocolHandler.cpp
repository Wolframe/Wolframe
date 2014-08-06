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
///\file escDlfProtocolHandler.cpp

#include "escDlfProtocolHandler.hpp"
#include "processor/execContext.hpp"
#include "logger-v1.hpp"
#include "filter/null_filter.hpp"
#include "filter/filter.hpp"

#undef _Wolframe_LOWLEVEL_DEBUG

using namespace _Wolframe;
using namespace _Wolframe::cmdbind;
using namespace _Wolframe::langbind;

EscDlfProtocolHandler::EscDlfProtocolHandler()
	:m_state(StartProcessing)
	,m_unconsumedInput(false)
	,m_gotEoD(false)
	,m_writedata(0)
	,m_writedatasize(0)
	,m_nextmsg(0)
	,m_itrpos(0)
{}

EscDlfProtocolHandler::EscDlfProtocolHandler( const CommandHandlerR& cmdhandler_)
	:m_state(StartProcessing)
	,m_unconsumedInput(false)
	,m_gotEoD(false)
	,m_writedata(0)
	,m_writedatasize(0)
	,m_nextmsg(0)
	,m_itrpos(0)
	,m_cmdhandler(cmdhandler_)
{}

EscDlfProtocolHandler::~EscDlfProtocolHandler()
{}

const char* EscDlfProtocolHandler::interruptDataSessionMarker() const
{
	switch (m_state)
	{
		case StartProcessing:
		case Processing:
		case FlushingOutput:
		case DiscardInput: return "\r\n.\r\n";
		case Terminated: return "";
	}
	return "";
}

void EscDlfProtocolHandler::setInputBuffer( void* buf, std::size_t allocsize)
{
	m_input.set( (char*)buf, allocsize);
	m_nextmsg = 0;
}

void EscDlfProtocolHandler::setOutputBuffer( void* buf, std::size_t size, std::size_t pos)
{
	m_output = protocol::OutputBlock( buf, size, pos);
}

bool EscDlfProtocolHandler::getCommandHandlerWriteData()
{
	if (m_cmdhandler.get())
	{
		std::size_t outputstart = m_output.pos();
		if (!m_esc_stm.hasMore())
		{
			m_cmdhandler->getOutput( m_writedata, m_writedatasize);
			m_esc_stm.setNextBlock( &m_output, (const char*)m_writedata, m_writedatasize);
		}
		m_esc_stm.process();
		m_writedata = (const void*)((const char*)m_output.ptr() + outputstart);
		m_writedatasize = m_output.pos() - outputstart;
		m_output.setPos(0);
		return (m_writedatasize > 0);
	}
	else
	{
		m_writedata = 0;
		m_writedatasize = 0;
		return false;
	}
}

ProtocolHandler::Operation EscDlfProtocolHandler::nextOperation()
{
	for (;;)
	{
#ifdef _Wolframe_LOWLEVEL_DEBUG
		LOG_TRACE << "STATE EscDlfProtocolHandler " << stateName( m_state);
#endif
		switch (m_state)
		{
			case Terminated:
				return CLOSE;

			case FlushingOutput:
				if (!m_cmdhandler.get())
				{
					setLastError( "internal: command handler undefined");
					m_state = DiscardInput;
					return READ;
				}
				if (getCommandHandlerWriteData())
				{
					return WRITE;
				}
				m_state = Processing;
				continue;

			case DiscardInput:
				if (m_gotEoD)
				{
					m_writedata = "\r\n.\r\n";
					m_writedatasize = 5/*std::strlen("\r\n.\r\n")*/;
					m_state = Terminated;
					return WRITE;
				}
				if (consumeInput())
				{
					continue;
				}
				return READ;

			case StartProcessing:
			case Processing:
			{
				if (!m_cmdhandler.get())
				{
					m_state = DiscardInput;
					continue;
				}
				m_state = Processing;
				try
				{
					switch (m_cmdhandler->nextOperation())
					{
						case CommandHandler::READ:
							if (consumeInput())
							{
								continue;
							}
							if (m_gotEoD)
							{
								m_state = DiscardInput;
								continue;
							}
							return READ;
	
						case CommandHandler::WRITE:
							if (getCommandHandlerWriteData())
							{
								m_state = FlushingOutput;
								return WRITE;
							}
							else
							{
								m_state = Terminated;
								setLastError( "illegal state: got WRITE from command handler but no data");
								return CLOSE;
							}
	
						case CommandHandler::CLOSE:
							if (m_cmdhandler->lastError())
							{
								setLastError( m_cmdhandler->lastError());
								m_state = DiscardInput;
								continue;
							}
							else
							{
								m_state = DiscardInput;
								continue;
							}
					}
				}
				catch (const std::runtime_error& err)
				{
					setLastError( err.what());
					m_state = DiscardInput;
					continue;
				}
				break;
			}
		}
		m_state = Terminated;
		setLastError( "internal: illegal state");
		return CLOSE;
	}
}

bool EscDlfProtocolHandler::consumeInput()
{
	if (m_unconsumedInput)
	{
		CommandHandler* cmdh = m_cmdhandler.get();
		if (cmdh)
		{
			if (m_state != DiscardInput)
			{
				const char* data = m_input.charptr()+m_itrpos;
				std::size_t datasize = m_eoD-m_input.at(m_itrpos);
				cmdh->putInput( data, datasize, m_input.gotEoD());
			}
		}
		if (m_input.gotEoD())
		{
			m_gotEoD = true;
			m_nextmsg = m_input.skipEoD();
		}
		m_unconsumedInput = false;
		return true;
	}
	return false;
}

void EscDlfProtocolHandler::putInput( const void* chunk_, std::size_t chunksize_)
{
	/*[-]*/std::cout << "++++ EscDlfProtocolHandler::putInput [" << std::string((const char*)chunk_, chunksize_) << "]" << std::endl;

	std::size_t startidx = (const char*)chunk_ - m_input.charptr();
	if (chunksize_ + startidx > m_input.size())
	{
		throw std::logic_error( "illegal input range passed to EscDlfProtocolHandler");
	}
	m_input.setPos( chunksize_ + startidx);
	if (m_itrpos != 0)
	{
		if (startidx != m_itrpos) throw std::logic_error( "unexpected buffer start for input to cmd handler");
		startidx = 0; //... start of buffer is end last message (part of eoD marker)
	}
	m_itrpos = startidx;
	protocol::InputBlock::iterator start = m_input.at( startidx);
	m_eoD = m_input.getEoD( start);
	m_nextmsg = m_eoD - m_input.at(0);
	m_unconsumedInput = true;
	if (m_state != StartProcessing)
	{
		(void)consumeInput();
	}
}

void EscDlfProtocolHandler::getInputBlock( void*& begin, std::size_t& maxBlockSize)
{
	if (!m_input.getNetworkMessageRead( begin, maxBlockSize))
	{
		throw std::logic_error( "buffer too small");
	}
	m_itrpos = ((const char*)begin - m_input.charptr());
}

void EscDlfProtocolHandler::getOutput( const void*& chunk_, std::size_t& chunksize_)
{
	chunk_ = m_writedata;
	chunksize_ = m_writedatasize;
	m_writedatasize = 0;
}

void EscDlfProtocolHandler::getDataLeft( const void*& begin, std::size_t& nofBytes)
{
	std::size_t pos = m_nextmsg;
	begin = (const void*)(m_input.charptr() + pos);
	nofBytes = m_input.pos() - pos;
}
