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
///\file wolfilter/src/echo_cmdhandler.cpp
///\brief Implementation wolfilter test command handler

#include "wolfilter/src/echo_cmdhandler.hpp"
#include <string>
#include <cstring>
#include <stdexcept>
#include "cmdbind/commandHandler.hpp"

EchoCommandHandler::EchoCommandHandler()
	:m_state(Init)
	,m_outputitr(0)
	,m_inpos(0)
	,m_cmd(0)
	,m_taglevel(0){}

void EchoCommandHandler::setInputBuffer( void* buf, std::size_t allocsize)
{
	m_in.set( buf, allocsize);
}

void EchoCommandHandler::setOutputBuffer( void* buf, std::size_t size, std::size_t pos)
{
	m_out.set( buf, size);
	m_out.setPos( pos);
}

cmdbind::CommandHandler::Operation EchoCommandHandler::nextOperation()
{
	char* cc;
	char* ee;

	for (;;)
	{
		switch (m_state)
		{
			case Init:
				m_state = ParseCommand;
				/*no break here!*/

			case ParseCommand:
				if (m_inpos >= m_in.pos()) return cmdbind::CommandHandler::READ;
				m_cmd = m_in.charptr()[ m_inpos++];
				if (m_cmd == '\n') continue;
				if (m_cmd < ' ' && m_cmd >= 0) throw std::runtime_error("illegal control character or null byte in command");
				m_state = Process;
				/*no break here!*/

			case Process:
				cc = m_in.charptr() + m_inpos;
				ee = m_in.charptr() + m_in.pos();
				if (cc == ee) return cmdbind::CommandHandler::READ;

				while (cc < ee)
				{
					if (*cc < ' ' && *cc > '\0')
					{
						if (*cc == '\n')
						{
							++cc;
							m_inpos = cc - m_in.charptr();
							bool res = executeCommand( m_cmd, m_inputline);
							m_state = ParseCommand;
							m_inputline.clear();
							if (res)
							{
								m_state = OnTerminate;
							}
							else
							{
								flushOutput();
								if (m_outputline.size() > 0) m_state = FlushOutput;
							}
							break;
						}
						else
						{
							throw std::runtime_error( "illegal control character passed to command handler");
						}
					}
					else
					{
						m_inputline.push_back( *cc);
					}
					++cc;
				}
				m_inpos = cc - m_in.charptr();
				continue;

			case FlushOutput:
				flushOutput();
				if (m_outputline.size() == 0) m_state = Process;
				return cmdbind::CommandHandler::WRITE;

			case OnTerminate:
				flushOutput();
				if (m_outputline.size() == 0 && m_out.pos() == 0) return cmdbind::CommandHandler::CLOSE;
				return cmdbind::CommandHandler::WRITE;
		}
	}
}

void EchoCommandHandler::putInput( const void* begin, std::size_t bytesTransferred)
{
	std::size_t startidx = (const char*)begin - m_in.charptr();
	if (startidx != 0) throw std::logic_error( "unexpected start of input in command handler");
	m_in.setPos( bytesTransferred + startidx);
	m_inpos = 0;
}

void EchoCommandHandler::getInputBlock( void*& begin, std::size_t& maxBlockSize)
{
	m_inpos = 0;
	m_in.setPos(0);
	begin = m_in.ptr();
	maxBlockSize = m_in.size();
}

void EchoCommandHandler::flushOutput()
{
	std::size_t restoutput = m_outputline.size() - m_outputitr;
	if (restoutput)
	{
		std::size_t restbuf = m_out.restsize();
		if (restbuf < restoutput)
		{
			restoutput = restbuf;
		}
		std::memcpy( m_out.charptr() + m_out.pos(), m_outputline.c_str() + m_outputitr, restoutput);
		m_out.incr( restoutput);

		m_outputitr += restoutput;
		if (m_outputitr == m_outputline.size())
		{
			m_outputitr = 0;
			m_outputline.clear();
		}
	}
}

void EchoCommandHandler::getOutput( const void*& begin, std::size_t& bytesToTransfer)
{
	begin = m_out.ptr();
	bytesToTransfer = m_out.pos();
	m_out.setPos(0);
}

void EchoCommandHandler::getDataLeft( const void*& begin, std::size_t& nofBytes)
{
	nofBytes = m_in.pos() - m_inpos;
	begin = (const void*)(m_in.charptr() + m_inpos);
}

void EchoCommandHandler::pushResult( char cmd, const std::string& arg)
{
	m_outputline.clear();
	m_outputline.push_back(cmd);
	m_outputline.append(arg);
	m_outputline.push_back('\n');
}

bool EchoCommandHandler::executeCommand( char cmd, const std::string& arg)
{
	switch (cmd)
	{
		case '<':
			--m_taglevel;
			pushResult( cmd, arg);
			return (m_taglevel == 0);
		case '>':
			++m_taglevel;
			pushResult( cmd, arg);
			return false;
		case '@':
		case '=':
			pushResult( cmd, arg);
			return false;
		default:
			throw std::runtime_error( "unknown command for echo command handler");
	}
}

