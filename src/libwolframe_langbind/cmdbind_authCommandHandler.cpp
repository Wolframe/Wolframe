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
	,m_outputbuf(0)
	,m_outputbufsize(0)
	,m_outputbufpos(0)
	,m_state(Init)
//	,m_readpos(0)	-> see authCommandHandler.hpp
	,m_writepos(0)
{}

AuthCommandHandler::~AuthCommandHandler()
{
	if (m_authenticator.get())
	{
		m_authenticator->close();
	}
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

void AuthCommandHandler::putInput( const void *begin, std::size_t bytesTransferred)
{
	std::size_t startidx = (const char*)begin - m_input.charptr();
	if (bytesTransferred + startidx > m_input.size())
	{
		throw std::logic_error( "illegal input range passed to AuthCommandHandler");
	}
	m_input.setPos( bytesTransferred + startidx);
	if (m_itrpos != 0)
	{
		if (startidx != m_itrpos) throw std::logic_error( "unexpected buffer start for input to cmd handler");
		startidx = 0; //... start of buffer is end last message (part of eoD marker)
	}
	protocol::InputBlock::iterator start = m_input.at( startidx);
	m_eoD = m_input.getEoD( start);

	m_readbuffer.append( start.ptr(), m_eoD-start);
	if (m_input.gotEoD())
	{
		//[+] m_authenticator->putReadMessage( AAAA::Authenticator::Message( (const void*)m_readbuffer.c_str(), m_readbuffer.size()));
		m_state = ReadConsumed;
	}
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
	if (bytesToTransfer == 0) throw std::logic_error( "protocol error: empty write in authorization command handler");

	std::memcpy( m_outputbuf + m_outputbufpos, m_writebuffer.c_str() + m_writepos, bytesToTransfer);
	begin = m_outputbuf + m_outputbufpos;
	m_outputbufpos = 0;
	m_writepos += bytesToTransfer;
}

void AuthCommandHandler::getDataLeft( const void*& begin, std::size_t& nofBytes)
{
	if (m_input.gotEoD())
	{
		std::size_t pos = m_eoD - m_input.begin();
		begin = (const void*)(m_input.charptr() + pos);
		nofBytes = m_input.pos() - pos;
	}
	else
	{
		if (m_readbuffer.size() > m_input.size())
		{
			throw std::logic_error("data requested but EoD not consumed in authorization commmand handler");
		}
		std::memcpy( m_input.charptr(), m_readbuffer.c_str(), m_readbuffer.size());
		m_input.setPos( nofBytes = m_readbuffer.size());
		begin = (const void*)m_input.charptr();
	}
}

CommandHandler::Operation AuthCommandHandler::nextOperation()
{
	for (;;)
	{
		LOG_TRACE << "STATE AuthCommandHandler " << stateName( m_state);
		switch (m_state)
		{
			case Init:
				//[+] m_authenticator->init();
				m_state = NextOperation;
				/*no break here!*/

			case NextOperation:
				return CLOSE;
				//[+] switch (m_authenticator->nextOperation())
				//[+] {
				//[+] 	case READ:
				//[+] 		return READ;
				//[+]
				//[+] 	case WRITE:
				//[+] 	{
				//[+] 		AAAA::Authenticator::Message msg = m_authenticator->getWriteMessage();
				//[+] 		m_writebuffer = protocol::escapeStringDLF( std::string( (const char*)msg.ptr, msg.size));
				//[+] 		m_writebuffer.append( "\r\n.\r\n");
				//[+] 		m_writepos = 0;
				//[+] 		m_state = FlushOutput;
				//[+] 		continue;
				//[+] 	}
				//[+] 	case CLOSE:
				//[+] 		return CLOSE;
				//[+] }

			case ReadConsumed:
				m_readbuffer.clear();
				m_state = NextOperation;
				continue;

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
	return CLOSE;
}



