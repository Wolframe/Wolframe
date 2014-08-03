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
#include "processor/execContext.hpp"
#include "handlerConfig.hpp"
#include "logger-v1.hpp"
#include <stdexcept>
#include <algorithm>
#include <boost/algorithm/string.hpp>

using namespace _Wolframe;
using namespace _Wolframe::proc;

void MainConnectionHandler::initSessionExceptionBYE()
{
	const char* termCommandStr = m_protocolHandler.get()?m_protocolHandler->interruptDataSessionMarker():"";
	m_exceptionByeMessage.append( termCommandStr?termCommandStr:"");
	m_exceptionByeMessage.append( "BYE\r\n");
	m_exceptionByeMessagePtr = m_exceptionByeMessage.c_str();
}

void MainConnectionHandler::networkInput( const void* dt, std::size_t nofBytes)
{
	try
	{
		m_protocolHandler->putInput( dt, nofBytes);
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

void MainConnectionHandler::signalOccured( NetworkSignal)
{
	LOG_TRACE << "Got signal";
	// ... on a signal we terminate immediately without sending a session exception BYE message
	m_terminated = true;
}

const net::NetworkOperation MainConnectionHandler::nextOperation()
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
		switch(m_protocolHandler->nextOperation())
		{
			case cmdbind::CommandHandler::READ:
				m_protocolHandler->getInputBlock( inpp, inppsize);
				return net::ReadData( inpp, inppsize, m_protocolHandler->execContext()->defaultTimeout());
	
			case cmdbind::CommandHandler::WRITE:
				m_protocolHandler->getOutput( outpp, outppsize);
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

MainConnectionHandler::MainConnectionHandler( const net::LocalEndpointR& local)
	:m_localEndPoint(local)
	,m_input(0)
	,m_inputsize(0)
	,m_output(0)
	,m_outputsize(0)
	,m_terminated(false)
	,m_exceptionByeMessagePtr(0)
{
	m_input = (char*)std::malloc( NeworkBufferSize);
	m_output = (char*)std::malloc( NeworkBufferSize);
	if (!m_input || !m_output)
	{
		if (m_input) std::free( m_input);
		if (m_output) std::free( m_output);
		throw std::bad_alloc();
	}
	LOG_TRACE << "Created connection handler for " << local->toString();
}

MainConnectionHandler::~MainConnectionHandler()
{
	if (m_input) std::free( m_input);
	if (m_output) std::free( m_output);
	LOG_TRACE << "Connection handler destroyed";
}

void MainConnectionHandler::setPeer( const net::RemoteEndpointR& remote)
{
	LOG_TRACE << "Peer set to " << remote->toString();
	m_remoteEndPoint = remote;
	if (m_protocolHandler.get())
	{
		m_protocolHandler->setPeer( m_remoteEndPoint);
	}
}

void MainConnectionHandler::setExecContext( proc::ExecContext* context_)
{
	std::string protocol = m_localEndPoint->config().protocol;
	if (protocol.empty())
	{
		protocol = "wolframe";
	}
	m_protocolHandler.reset( context_->provider()->protocolHandler( protocol));
	if (!m_protocolHandler.get())
	{
		throw std::runtime_error( std::string("protocol '") + protocol + "' is not defined");
	}
	m_protocolHandler->setExecContext( context_);
	m_protocolHandler->setInputBuffer( m_input, m_inputsize);
	m_protocolHandler->setOutputBuffer( m_output, m_outputsize, 0);
	m_protocolHandler->setLocalEndPoint( m_localEndPoint);
	m_protocolHandler->setPeer( m_remoteEndPoint);
}

