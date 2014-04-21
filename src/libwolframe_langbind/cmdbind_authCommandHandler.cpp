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

AuthCommandHandler::AuthCommandHandler( AAAA::Authenticator* authenticator_)
	:m_state(Init)
	,m_writedata(0)
	,m_writedatasize(0)
	,m_writedata_chksum(0)
	,m_writedata_chkpos(0)
	,m_authenticator(authenticator_)
	,m_itrpos(0)
{}

AuthCommandHandler::~AuthCommandHandler()
{
	if (m_authenticator) delete m_authenticator;
}

const char* AuthCommandHandler::interruptDataSessionMarker() const
{
	switch (m_state)
	{
		case Processing:
		case FlushingOutput:
		case DiscardInput: return "\r\n.\r\n";
		case Terminated: return "";
	}
	return "";
}

void AuthCommandHandler::setInputBuffer( void* buf, std::size_t allocsize)
{
	m_input = InputBlock( (char*)buf, allocsize);
}

void AuthCommandHandler::setOutputBuffer( void* buf, std::size_t size, std::size_t pos)
{
	m_output = OutputBlock( buf, size, pos);
}

void AuthCommandHandler::getOutputWriteData()
{
	m_writedata = m_output.ptr();
	m_writedatasize = m_output.pos();

	langbind::OutputFilter::calculateCheckSum( m_writedata_chksum, 0, (const char*)m_writedata, m_writedatasize);
	m_writedata_chkpos += m_writedatasize;

	m_escapeBuffer.process( m_output.charptr(), m_output.size(), m_writedatasize);
	m_output.setPos(0);
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
		m_authenticator->putMessage( Message( m_readbuffer.c_str(), m_readbuffer.size()));
		m_readbuffer.clear();
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
	begin = m_writedata;
	bytesToTransfer = m_writedatasize;
}

void AuthCommandHandler::getDataLeft( const void*& begin, std::size_t& nofBytes)
{
	std::size_t pos = m_eoD - m_input.begin();
	begin = (const void*)(m_input.charptr() + pos);
	nofBytes = m_input.pos() - pos;
}

CommandHandler::Operation AuthCommandHandler::nextOperation()
{
	for (;;)
	{
		LOG_TRACE << "STATE AuthCommandHandler " << stateName( m_state);
		switch (m_state)
		{
			case Init:
			case NextOperation:
			case FlushOutput:
			case Terminate:
		}
	}//for(;;)
	return CLOSE;
}



