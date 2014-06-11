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
/// \file authCommandHandler.cpp
/// \brief Implementation of the authentication command handler

#include "cmdbind/authCommandHandler.hpp"

using namespace _Wolframe;
using namespace _Wolframe::cmdbind;

AuthCommandHandler::AuthCommandHandler( const boost::shared_ptr<AAAA::Authenticator>& authenticator_)
	:m_authenticator(authenticator_)
	,m_itrpos(0)
	,m_msgstart(0)
	,m_outputbuf(0)
	,m_outputbufsize(0)
	,m_outputbufpos(0)
	,m_state(Init)
	,m_writepos(0)
{}

AuthCommandHandler::~AuthCommandHandler()
{
}

const char* AuthCommandHandler::interruptDataSessionMarker() const
{
	return (m_state == FlushOutput)?"\r\n.\r\n":"";
}

void AuthCommandHandler::setInputBuffer( void* buf, std::size_t allocsize)
{
	m_input.set( (char*)buf, allocsize);
}

void AuthCommandHandler::setOutputBuffer( void* buf, std::size_t size, std::size_t pos)
{
	m_outputbuf = (char*)buf;
	m_outputbufsize = size;
	m_outputbufpos = pos;
	if (pos > size) throw std::logic_error("illegal parameter (setOutputBuffer)");
}

bool AuthCommandHandler::consumeNextMessage()
{
	protocol::InputBlock::iterator start = m_input.at( m_msgstart);
	m_eoD = m_input.getEoD( start);

	m_readbuffer.append( start.ptr(), m_eoD-start);
	if (m_input.gotEoD())
	{
		// We got end of data, so we expect the read buffer to contain the next complete message:
		m_authenticator->messageIn( m_readbuffer);
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

void AuthCommandHandler::putInput( const void *begin, std::size_t bytesTransferred)
{
	std::size_t startidx = (const char*)begin - m_input.charptr();
	if (bytesTransferred + startidx > m_input.size())
	{
		throw std::logic_error( "illegal input range passed to AuthCommandHandler");
	}
	m_input.setPos( bytesTransferred + startidx);
	if (startidx != m_itrpos) throw std::logic_error( "unexpected buffer start for input to cmd handler");

	m_msgstart = 0;
	m_state = NextOperation;
}

void AuthCommandHandler::getInputBlock( void*& begin, std::size_t& maxBlockSize)
{
	if (!m_input.getNetworkMessageRead( begin, maxBlockSize))
	{
		throw std::logic_error( "buffer too small for input");
	}
	m_itrpos = ((const char*)begin - m_input.charptr());
}

void AuthCommandHandler::getOutput( const void*& begin, std::size_t& bytesToTransfer)
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

void AuthCommandHandler::getDataLeft( const void*& begin, std::size_t& nofBytes)
{
	begin = (const void*)(m_input.charptr() + m_msgstart);
	nofBytes = m_input.pos() - m_msgstart;
}

CommandHandler::Operation AuthCommandHandler::nextOperation()
{
	for (;;)
	{
		LOG_TRACE << "STATE AuthCommandHandler " << stateName( m_state) << " " << m_authenticator->statusName( m_authenticator->status());
		switch (m_state)
		{
			case Init:
				m_state = NextOperation;
				/*no break here!*/

			case NextOperation:
				switch (m_authenticator->status())
				{
					case AAAA::Authenticator::INITIALIZED:
						throw std::logic_error("authentication protocol operation in state INITIALIZED");
					case AAAA::Authenticator::MESSAGE_AVAILABLE:
						m_writebuffer = protocol::escapeStringDLF( m_authenticator->messageOut());
						m_writebuffer.append( "\r\n.\r\n");
						m_writepos = 0;
						m_state = FlushOutput;
						continue;
					case AAAA::Authenticator::AWAITING_MESSAGE:
						if (!consumeNextMessage())
						{
							return READ;
						}
						continue;
					case AAAA::Authenticator::AUTHENTICATED:
						return CLOSE;
					case AAAA::Authenticator::INVALID_CREDENTIALS:
						setLastError( "either the username or the credentials are invalid");
						return CLOSE;
					case AAAA::Authenticator::MECH_UNAVAILABLE:
						setLastError( "the requested authentication mech is not available");
						return CLOSE;
					case AAAA::Authenticator::SYSTEM_FAILURE:
						setLastError( "unspecified authentication system error");
						return CLOSE;
				}
				setLastError( "internal: unhandled authenticator status");
				return CLOSE;

			case FlushOutput:
				if (m_writepos == m_writebuffer.size())
				{
					m_writepos = 0;
					m_writebuffer.clear();
					m_state = NextOperation;
					continue;
				}
				return WRITE;
		}
	}//for(;;)
	setLastError( "internal: unhandled state in authentication protocol");
	return CLOSE;
}



