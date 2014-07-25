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
///\file contentOnlyProtocolHandler.cpp
#include "contentOnlyProtocolHandler.hpp"
#include "interfaceProtocolHandler.hpp"
#include "authProtocolHandler.hpp"
#include "passwordChangeProtocolHandler.hpp"
#include "escDlfProtocolHandler.hpp"
#include "processor/execContext.hpp"
#include "interfaceProtocolHandler.hpp"
#include "handlerConfig.hpp"
#include "logger-v1.hpp"
#include <stdexcept>
#include <algorithm>
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace _Wolframe::cmdbind;

ContentOnlyProtocolHandler::ContentOnlyProtocolHandler()
	:m_remoteEndpoint(0)
	,m_localEndpoint(0)
	,m_state(Init)
	,m_eod(false){}

void ContentOnlyProtocolHandler::setPeer( const net::RemoteEndpoint& remote)
{
	m_remoteEndpoint = &remote;
}

void ContentOnlyProtocolHandler::setLocalEndPoint( const net::LocalEndpoint& local)
{
	m_localEndpoint = &local;
}

static bool IsSpace( char ch) {return ch>0 && ch <=32;}

void ContentOnlyProtocolHandler::setArgumentString( const std::string& arg)
{
	m_cmdname = boost::algorithm::trim_copy( arg);
	std::string::const_iterator ai = arg.begin(), ae = arg.end();
	for (; ai != ae; ++ai)
	{
		if (IsSpace( *ai)) throw std::runtime_error("more than one argument specified for content only protocol handler");
	}
}

void ContentOnlyProtocolHandler::setInputBuffer( void* buf, std::size_t allocsize)
{
	m_input = protocol::InputBlock( (char*)buf, allocsize);
}

void ContentOnlyProtocolHandler::setOutputBuffer( void* buf, std::size_t size, std::size_t pos)
{
	if (size < 16) throw std::logic_error("output buffer smaller than 16 bytes");
	m_output = protocol::OutputBlock( buf, size, pos);
}

ProtocolHandler::Operation ContentOnlyProtocolHandler::nextOperation()
{
	switch (m_state)
	{
		case Init:
			if (!execContext())
			{
				throw std::runtime_error("execution context not defined");
			}
			m_doctypeDetector.reset( execContext()->provider()->doctypeDetector());
			if (!m_doctypeDetector.get())
			{
				throw std::runtime_error("no document type detection defined");
			}
			m_state = DoctypeDetection;
			/* no break here ! */
		case DoctypeDetection:
			if (!m_doctypeDetector->run())
			{
				if (m_eod)
				{
					throw std::runtime_error( "got EOF and document type/format is still not recognized");
				}
				return READ;
			}
			m_doctypeInfo = m_doctypeDetector->info();
			if (!m_doctypeInfo.get())
			{
				if (m_doctypeDetector->lastError())
				{
					throw std::runtime_error( std::string("document type detection error: ") + m_doctypeDetector->lastError());
				}
				else
				{
					throw std::runtime_error( "document type/format not recognized");
				}
			}
			if (m_cmdname.size() && m_cmdname[m_cmdname.size()-1] == '!')
			{
				m_cmdname.resize( m_cmdname.size()-1);
			}
			else
			{
				m_cmdname.append( m_doctypeInfo->doctype());
			}
			m_commandHandler.reset( execContext()->provider()->cmdhandler( m_cmdname, m_doctypeInfo->docformat()));
			if (!m_commandHandler.get())
			{
				throw std::runtime_error( std::string("no command handler defined for command '") + m_cmdname + "'");
			}
			m_commandHandler->putInput( m_buffer.c_str(), m_buffer.size(), m_eod);
			m_state = Processing;
			/* no break here ! */

		case Processing:
			switch (m_commandHandler->nextOperation())
			{
				case CommandHandler::CLOSE:
					m_state = Done;
					return CLOSE;
				case CommandHandler::READ:
					if (!m_eod)
					{
						throw std::runtime_error( "unexpected end of file in processing");
					}
					else
					{
						return READ;
					}
				case CommandHandler::WRITE:
					return WRITE;
			}
			throw std::runtime_error( "internal: illegal state in processing");

		case Done:
			throw std::runtime_error( "internal: got called again after CLOSE");
	}
	throw std::runtime_error( "internal: illegal state in fetching next operation");
}

void ContentOnlyProtocolHandler::putEOF()
{
	m_eod = true;
}

void ContentOnlyProtocolHandler::putInput( const void* chunk, std::size_t chunksize)
{
	switch (m_state)
	{
		case Init:
			throw std::runtime_error( "internal error: called put input before nextOperation()");

		case DoctypeDetection:
			m_doctypeDetector->putInput( (const char*)chunk, chunksize);
			m_buffer.append( (const char*)chunk, chunksize);
			break;

		case Processing:
			m_commandHandler->putInput( (const char*)chunk, chunksize, m_eod);
			break;

		case Done:
			throw std::runtime_error( "internal error: put input after CLOSE");
	}
	throw std::runtime_error( "internal: illegal state in put input");
}

void ContentOnlyProtocolHandler::getInputBlock( void*& begin, std::size_t& maxBlockSize)
{
	if (!m_input.getNetworkMessageRead( begin, maxBlockSize))
	{
		throw std::logic_error( "buffer too small for input");
	}
}

void ContentOnlyProtocolHandler::getOutput( const void*& chunk, std::size_t& chunksize)
{
	if (m_state != Processing || !m_commandHandler.get()) throw std::runtime_error("internal: output fetched in non processing state");
	m_commandHandler->getOutput( chunk, chunksize);
}

void ContentOnlyProtocolHandler::getDataLeft( const void*& begin, std::size_t& nofBytes)
{
	begin = 0;
	nofBytes = 0;
}

