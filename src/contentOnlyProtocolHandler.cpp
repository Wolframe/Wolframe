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
#include "processor/execContext.hpp"
#include "handlerConfig.hpp"
#include "logger-v1.hpp"
#include <stdexcept>
#include <algorithm>
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace _Wolframe::cmdbind;

ContentOnlyProtocolHandler::ContentOnlyProtocolHandler()
	:m_bufferpos(0)
	,m_input(0)
	,m_inputsize(0)
	,m_output(0)
	,m_outputsize(0)
	,m_outputpos(0)
	,m_writeblock(0)
	,m_writesize(0)
	,m_writepos(0)
	,m_state(Init)
	,m_eod(false){}

void ContentOnlyProtocolHandler::setPeer( const net::RemoteEndpointR& remote)
{
	m_remoteEndpoint = remote;
}

void ContentOnlyProtocolHandler::setLocalEndPoint( const net::LocalEndpointR& local)
{
	m_localEndpoint = local;
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
	m_input = (char*)buf;
	m_inputsize = allocsize;
}

void ContentOnlyProtocolHandler::setOutputBuffer( void* buf, std::size_t size, std::size_t pos)
{
	m_output = (char*)buf;
	m_outputsize = size;
	m_outputpos = pos;
}

ProtocolHandler::Operation ContentOnlyProtocolHandler::nextOperation()
{
	try
	{
		for (;;) switch (m_state)
		{
			case Init:
				if (!execContext())
				{
					LOG_ERROR << "execution context not defined";
					return CLOSE;
				}
				m_doctypeDetector.reset( execContext()->provider()->doctypeDetector());
				if (!m_doctypeDetector.get())
				{
					if (m_cmdname.empty() || m_cmdname[m_cmdname.size()-1] != '!')
					{
						LOG_ERROR << "no document type detection defined";
						return CLOSE;
					}
					else
					{
						LOG_WARNING << "no document type detection defined, skipping it";
						m_cmdname.resize( m_cmdname.size()-1);
						m_state = InitProcessing;
						continue;
					}
				}

				m_state = DoctypeDetection;
				/* no break here ! */

			case DoctypeDetection:
				if (!m_doctypeDetector->run())
				{
					if (m_eod)
					{
						LOG_ERROR << "got EOF and document type/format is still not recognized";
						return CLOSE;
					}
					return READ;
				}
				m_doctypeInfo = m_doctypeDetector->info();
				if (!m_doctypeInfo.get())
				{
					if (m_doctypeDetector->lastError())
					{
						LOG_ERROR << "document type detection error: " << m_doctypeDetector->lastError();
						return CLOSE;
					}
					else
					{
						LOG_ERROR << "document type/format not recognized";
						return CLOSE;
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
				/*no break here!*/

			case InitProcessing:
				if (m_doctypeInfo.get())
				{
					m_commandHandler.reset( execContext()->provider()->cmdhandler( m_cmdname, m_doctypeInfo->docformat()));
				}
				else
				{
					m_commandHandler.reset( execContext()->provider()->cmdhandler( m_cmdname, ""));
				}
				if (!m_commandHandler.get())
				{
					throw std::runtime_error( std::string("no command handler defined for command '") + m_cmdname + "'");
				}
				m_commandHandler->setExecContext( execContext());
				m_commandHandler->setOutputChunkSize( (m_outputsize * 8) - (m_outputsize / 16));
				m_state = Processing;
				/* no break here ! */
	
			case Processing:
				switch (m_commandHandler->nextOperation())
				{
					case CommandHandler::CLOSE:
						if (m_eod)
						{
							m_state = Done;
							return CLOSE;
						}
						else
						{
							m_state = DiscardInput;
							return READ;
						}
					case CommandHandler::READ:
						if (m_buffer.size() > 0)
						{
							if (m_bufferpos == m_buffer.size())
							{
								m_bufferpos = 0;
								m_buffer.clear();
							}
							else
							{
								m_commandHandler->putInput( m_buffer.c_str(), m_buffer.size(), m_eod);
								m_bufferpos = m_buffer.size();
								continue;
							}
						}
						if (m_eod)
						{
							throw std::runtime_error( "unexpected end of file in processing");
						}
						else
						{
							return READ;
						}
					case CommandHandler::WRITE:
					{
						const void* voidblk;
						m_commandHandler->getOutput( voidblk, m_writesize);
						m_writeblock = (const char*)voidblk;
						m_writepos = 0;
						m_state = FlushingOutput;
						return WRITE;
					}
				}
				LOG_ERROR << "internal: illegal state in processing";
				return CLOSE;

			case FlushingOutput:
				if (m_writepos == m_writesize)
				{
					m_writepos = 0;
					m_writesize = 0;
					m_state = Processing;
					continue;
				}
				return WRITE;
	
			case DiscardInput:
				return m_eod?CLOSE:READ;

			case Done:
				LOG_ERROR << "internal: got called again after CLOSE";
				return CLOSE;
		}
		LOG_ERROR << "internal: illegal state in fetching next operation";
		return CLOSE;
	}
	catch (const std::runtime_error& err)
	{
		LOG_ERROR << "error in content protocol handler " << err.what();
		return CLOSE;
	}
}

void ContentOnlyProtocolHandler::putEOF()
{
	m_eod = true;
	m_commandHandler->putInput( "", 0, m_eod);
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

		case DiscardInput:
			//... input is discarded, respectively ignored
			break;

		case Processing:
			m_commandHandler->putInput( (const char*)chunk, chunksize, m_eod);
			break;

		case InitProcessing:
			throw std::runtime_error( "internal error: put input in init processing state");
		case FlushingOutput:
			throw std::runtime_error( "internal error: put input in flushing output state");
		case Done:
			throw std::runtime_error( "internal error: put input after CLOSE");
	}
}

void ContentOnlyProtocolHandler::getInputBlock( void*& begin, std::size_t& maxBlockSize)
{
	begin = m_input;
	maxBlockSize = m_inputsize;
}

void ContentOnlyProtocolHandler::getOutput( const void*& chunk, std::size_t& chunksize)
{
	if (m_state != FlushingOutput || m_outputpos == m_outputsize || !m_commandHandler.get()) throw std::runtime_error("internal: output fetched in non processing state");
	if (m_writesize - m_writepos > m_outputsize - m_outputpos)
	{
		chunksize = m_outputsize - m_outputpos;
	}
	else
	{
		chunksize = m_writesize - m_writepos;
	}
	std::memcpy( m_output + m_outputpos, m_writeblock + m_writepos, chunksize);
	chunk = m_output + m_outputpos;
	m_writepos += chunksize;
}

void ContentOnlyProtocolHandler::getDataLeft( const void*& begin, std::size_t& nofBytes)
{
	begin = 0;
	nofBytes = 0;
}

