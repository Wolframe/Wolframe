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
/// \file baseCryptoProtocolHandler.cpp
/// \brief Implementation of the authentication command handler

#include "baseCryptoProtocolHandler.hpp"

using namespace _Wolframe;
using namespace _Wolframe::cmdbind;

BaseCryptoProtocolHandler::BaseCryptoProtocolHandler()
	:m_itrpos(0)
	,m_msgstart(0)
	,m_outputbuf(0)
	,m_outputbufsize(0)
	,m_outputbufpos(0)
	,m_state(Init)
	,m_writepos(0)
{}

BaseCryptoProtocolHandler::~BaseCryptoProtocolHandler()
{
}

const char* BaseCryptoProtocolHandler::interruptDataSessionMarker() const
{
	return (m_state == FlushOutput)?"\r\n.\r\n":"";
}

void BaseCryptoProtocolHandler::setInputBuffer( void* buf, std::size_t allocsize)
{
	m_input.set( (char*)buf, allocsize);
}

void BaseCryptoProtocolHandler::setOutputBuffer( void* buf, std::size_t size, std::size_t pos)
{
	m_outputbuf = (char*)buf;
	m_outputbufsize = size;
	m_outputbufpos = pos;
	if (pos > size) throw std::logic_error("illegal parameter (setOutputBuffer)");
}

bool BaseCryptoProtocolHandler::consumeNextMessage()
{
	protocol::InputBlock::iterator start = m_input.at( m_msgstart);
	m_eoD = m_input.getEoD( start);

	m_readbuffer.append( start.ptr(), m_eoD-start);
	if (m_input.gotEoD())
	{
		// We got end of data, so we expect the read buffer to contain the next complete message:
		processMessage( m_readbuffer);
		m_readbuffer.clear();

		// Skip to the start of the next message:
		m_msgstart = m_input.skipEoD();

		// We have a new complete message consumed:
		return true;
	}
	else
	{
		return false;
	}
}

void BaseCryptoProtocolHandler::putInput( const void *begin, std::size_t bytesTransferred)
{
	std::size_t startidx = (const char*)begin - m_input.charptr();
	if (bytesTransferred + startidx > m_input.size())
	{
		throw std::logic_error( "illegal input range passed to BaseCryptoProtocolHandler");
	}
	m_input.setPos( bytesTransferred + startidx);
	if (startidx != m_itrpos) throw std::logic_error( "unexpected buffer start for input to cmd handler");

	m_msgstart = 0;
	m_state = NextOperation;
}

void BaseCryptoProtocolHandler::getInputBlock( void*& begin, std::size_t& maxBlockSize)
{
	if (!m_input.getNetworkMessageRead( begin, maxBlockSize))
	{
		throw std::logic_error( "buffer too small for input");
	}
	m_itrpos = ((const char*)begin - m_input.charptr());
}

void BaseCryptoProtocolHandler::getOutput( const void*& begin, std::size_t& bytesToTransfer)
{
	bytesToTransfer = m_outputbufsize - m_outputbufpos;
	if (bytesToTransfer > m_writebuffer.size() - m_writepos)
	{
		bytesToTransfer = m_writebuffer.size() - m_writepos;
	}
	if (bytesToTransfer == 0) throw std::logic_error( "protocol error: empty write in authentication command handler");

	std::memcpy( m_outputbuf + m_outputbufpos, m_writebuffer.c_str() + m_writepos, bytesToTransfer);
	begin = m_outputbuf + m_outputbufpos;
	m_outputbufpos = 0;
	m_writepos += bytesToTransfer;
}

void BaseCryptoProtocolHandler::getDataLeft( const void*& begin, std::size_t& nofBytes)
{
	begin = (const void*)(m_input.charptr() + m_msgstart);
	nofBytes = m_input.pos() - m_msgstart;
}

void BaseCryptoProtocolHandler::pushOutput( const std::string& msg)
{
	m_writebuffer = protocol::escapeStringDLF( msg);
	m_writebuffer.append( "\r\n.\r\n");
	m_writepos = 0;
	m_state = FlushOutput;
	
}

bool BaseCryptoProtocolHandler::endOfOutput()
{
	if (m_writepos == m_writebuffer.size())
	{
		m_writepos = 0;
		m_writebuffer.clear();
		return true;
	}
	return false;
}



