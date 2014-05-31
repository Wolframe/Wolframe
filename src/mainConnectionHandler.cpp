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
///\file mainConnectionHandler.cpp
#include "mainConnectionHandler.hpp"
#include "mainCommandHandler.hpp"
#include "cmdbind/discardInputCommandHandlerEscDLF.hpp"
#include "cmdbind/authCommandHandler.hpp"
#include "processor/execContext.hpp"
#include "interfaceCommandHandler.hpp"
#include "handlerConfig.hpp"
#include "logger-v1.hpp"
#include <stdexcept>
#include <algorithm>
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace _Wolframe::proc;

void Connection::initSessionExceptionBYE()
{
	const char* termCommandStr = m_cmdhandler.interruptDataSessionMarker();
	m_exceptionByeMessage.append( termCommandStr?termCommandStr:"");
	m_exceptionByeMessage.append( "BYE\r\n");
	m_exceptionByeMessagePtr = m_exceptionByeMessage.c_str();
}

void Connection::networkInput( const void* dt, std::size_t nofBytes)
{
	try
	{
		m_cmdhandler.putInput( dt, nofBytes);
		return;
	}
	catch (const std::runtime_error& err)
	{
		LOG_ERROR << "must terminate connection because of uncaught runtime error exception in command handler (putInput): " << err.what();
	}
	catch (const std::bad_alloc& err)
	{
		LOG_ERROR << "must terminate connection because of uncaught out of memory exception in command handler (putInput): " << err.what();
	}
	catch (const std::logic_error& err)
	{
		LOG_ERROR << "must terminate connection because of uncaught logic error exception in command handler (putInput): " << err.what();
	}
	initSessionExceptionBYE();
	m_terminated = true;
}

void Connection::signalOccured( NetworkSignal)
{
	LOG_TRACE << "Got signal";
	// ... on a signal we terminate immediately without sending a session exception BYE message
	m_terminated = true;
}

const net::NetworkOperation Connection::nextOperation()
{
	void* inpp;
	std::size_t inppsize;
	const void* outpp;
	std::size_t outppsize;
	if (m_terminated)
	{
		if (m_exceptionByeMessagePtr)
		{
			m_exceptionByeMessagePtr = 0;
			return net::SendData( m_exceptionByeMessage.c_str(), m_exceptionByeMessage.size());
		}
		return net::CloseConnection();
	}
	try
	{
		switch(m_cmdhandler.nextOperation())
		{
			case cmdbind::CommandHandler::READ:
				m_cmdhandler.getInputBlock( inpp, inppsize);
				return net::ReadData( inpp, inppsize);
	
			case cmdbind::CommandHandler::WRITE:
				m_cmdhandler.getOutput( outpp, outppsize);
				return net::SendData( outpp, outppsize);
	
			case cmdbind::CommandHandler::CLOSE:
				return net::CloseConnection();
		}
		return net::CloseConnection();
	}
	catch (const std::runtime_error& err)
	{
		LOG_ERROR << "must terminate connection because of uncaught runtime error exception in command handler (nextOperation): " << err.what();
	}
	catch (const std::bad_alloc& err)
	{
		LOG_ERROR << "must terminate connection because of uncaught out of memory exception in command handler (nextOperation): " << err.what();
	}
	catch (const std::logic_error& err)
	{
		LOG_ERROR << "must terminate connection because of uncaught logic error exception in command handler (nextOperation): " << err.what();
	}
	initSessionExceptionBYE();
	m_exceptionByeMessagePtr = 0;
	m_terminated = true;
	return net::SendData( m_exceptionByeMessage.c_str(), m_exceptionByeMessage.size());
}

Connection::Connection( const net::LocalEndpoint& local)
	:m_cmdhandler()
	,m_terminated(false)
	,m_exceptionByeMessagePtr(0)
{
	m_cmdhandler.setInputBuffer( m_input.ptr(), m_input.size());
	m_cmdhandler.setOutputBuffer( m_output.ptr(), m_output.size());
	LOG_TRACE << "Created connection handler for " << local.toString();
}

Connection::~Connection()
{
	LOG_TRACE << "Connection handler destroyed";
}

void Connection::setPeer( const net::RemoteEndpoint& remote)
{
	LOG_TRACE << "Peer set to " << remote.toString();
	m_cmdhandler.setPeer( remote);
}


