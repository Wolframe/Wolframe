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

LineCommandHandler::LineCommandHandler()
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


