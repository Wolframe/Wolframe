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
#include "protocol/commandHandler.hpp"
#include "logger-v1.hpp"

using namespace _Wolframe;
using namespace _Wolframe::protocol;

CommandHandler::CommandHandler()
	:m_statusCode(0),m_gotEoD(false),m_state(Processing)
{
	m_itr = m_input.begin();
	m_eoD = m_end = m_input.end();
}

void CommandHandler::passParameters( int argc, const char** argv)
{
	for (int ii=0; ii<argc; ii++)
	{
		m_argBuffer.push_back( argv[ ii]);
	}
}

void CommandHandler::passIO( const InputBlock& input, const OutputBlock& output)
{
	m_input = input;
	m_output = output;
	m_itr = m_input.begin();
	m_eoD = m_end = m_input.end();
}

CommandHandler::Operation CommandHandler::nextOperation()
{
	switch (m_state)
	{
		case Terminated:
			return CLOSED;

		case FlushingOutput:
			return WRITE;

		case DiscardInput:
			if (m_gotEoD)
			{
				m_state = Terminated;
				m_itr = m_eoD;
				m_output.release();
				if (!m_output.print( "\r\n.\r\n", 5))
				{
					throw std::logic_error( "output buffer to small for eoD marker");
				}
				return WRITE;
			}
			return READ;

		case Processing:
			if (m_itr == m_eoD)
			{
				return (m_input.gotEoD())?WRITE:READ;
			}
			else
			{
				return WRITE;
			}
	}
	LOG_ERROR << "illegal state";
	return CLOSED;
}

void CommandHandler::terminate( int cd)
{
	m_statusCode = cd;

	if (m_state == Processing)
	{
		m_state = DiscardInput;
	}
}

void CommandHandler::flushOutput()
{
	if (m_state == Processing)
	{
		m_state = FlushingOutput;
	}
}

void CommandHandler::putInput( const void *begin, std::size_t bytesTransferred)
{
	m_input.setPos( bytesTransferred + ((const char*)begin - m_input.charptr()));
	m_eoD = m_input.getEoD( m_input.begin());
	m_itr = m_input.begin();
	m_end = m_input.end();
}

void CommandHandler::getInputBlock( void*& begin, std::size_t& maxBlockSize)
{
	if (!m_input.getNetworkMessageRead( begin, maxBlockSize))
	{
		throw std::logic_error( "buffer too small");
	}
}

void CommandHandler::getOutput( const void*& begin, std::size_t& bytesToTransfer)
{
	begin = m_output.ptr();
	bytesToTransfer = m_output.pos();
	m_output.setPos(0);
	if (m_state == FlushingOutput)
	{
		m_state = Processing;
	}
}

void CommandHandler::getDataLeft( const void*& begin, std::size_t& nofBytes)
{
	begin = (char*)(m_input.charptr() + (m_itr - m_input.begin()));
	nofBytes = m_end - m_itr;
}


