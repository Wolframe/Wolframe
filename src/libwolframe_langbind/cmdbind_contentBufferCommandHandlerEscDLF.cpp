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
///\file cmdbind_contentBufferCommandHandlerEscDLF.cpp

#include "cmdbind/contentBufferCommandHandlerEscDLF.hpp"
#include "filter/outputfilter.hpp"
#include "filter/inputfilter.hpp"
#include "logger-v1.hpp"

using namespace _Wolframe;
using namespace _Wolframe::cmdbind;
using namespace _Wolframe::langbind;

ContentBufferCommandHandlerEscDLF::ContentBufferCommandHandlerEscDLF()
	:m_state(Processing)
	,m_itrpos(0)
{}

ContentBufferCommandHandlerEscDLF::~ContentBufferCommandHandlerEscDLF()
{}

void ContentBufferCommandHandlerEscDLF::setInputBuffer( void* buf, std::size_t allocsize)
{
	m_input = protocol::InputBlock( (char*)buf, allocsize);
}

void ContentBufferCommandHandlerEscDLF::setOutputBuffer( void* buf, std::size_t size, std::size_t pos)
{
	m_output = protocol::OutputBlock( buf, size, pos);
}

CommandHandler::Operation ContentBufferCommandHandlerEscDLF::nextOperation()
{
	for (;;)
	{
		LOG_TRACE << "STATE ContentBufferCommandHandlerEscDLF " << stateName( m_state);

		switch (m_state)
		{
			case Terminated:
				return CLOSE;

			case Processing:
				return READ;
		}
		LOG_ERROR << "illegal state";
		return CLOSE;
	}
}


void ContentBufferCommandHandlerEscDLF::putInput( const void *begin, std::size_t bytesTransferred)
{
	std::size_t startidx = (const char*)begin - m_input.charptr();
	m_input.setPos( bytesTransferred + startidx);
	if (m_itrpos != 0)
	{
		if (startidx != m_itrpos) throw std::logic_error( "unexpected buffer start for input to cmd handler");
		startidx = 0; //... start of buffer is end last message (part of eoD marker)
	}
	protocol::InputBlock::iterator start = m_input.at( startidx);
	m_eoD = m_input.getEoD( start);
	m_content.append( start.ptr(), m_eoD-start);
	if (m_input.gotEoD()) m_state = Terminated;
}

void ContentBufferCommandHandlerEscDLF::getInputBlock( void*& begin, std::size_t& maxBlockSize)
{
	if (!m_input.getNetworkMessageRead( begin, maxBlockSize))
	{
		throw std::logic_error( "buffer too small");
	}
	m_itrpos = ((const char*)begin - m_input.charptr());
}

void ContentBufferCommandHandlerEscDLF::getOutput( const void*& begin, std::size_t& bytesToTransfer)
{
	begin = 0;
	bytesToTransfer = 0;
}

void ContentBufferCommandHandlerEscDLF::getDataLeft( const void*& begin, std::size_t& nofBytes)
{
	std::size_t pos = m_eoD - m_input.begin();
	begin = (const void*)(m_input.charptr() + pos);
	nofBytes = m_input.pos() - pos;
}


