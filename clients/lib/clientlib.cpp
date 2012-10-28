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
///\file clientlib.cpp
///\brief Implementation C++ client library

#include "clientlib.hpp"
#include <stdexcept>

using namespace _Wolframe;
using namespace _Wolframe::client;

ProtocolHandler::ProtocolHandler()
	:m_impl(0)
{
	m_impl = wolfcli_createProtocolHandler( &ProtocolHandler::eventhandler, this);
	if (!m_impl) throw std::bad_alloc();
}

ProtocolHandler::~ProtocolHandler()
{
	if (m_impl) wolfcli_destroyProtocolHandler( m_impl);
}

void ProtocolHandler::pushData( const char* data, std::size_t datasize)
{
	if (!wolfcli_protocol_pushData( m_impl, data, datasize))
	{
		throw std::bad_alloc();
	}
}

bool ProtocolHandler::doRequest( RequestHandler* handler, const char* data, std::size_t datasize)
{
	if (!wolfcli_protocol_open( m_impl)) return false;
	m_rhqueue.push_back( handler);
	if (!wolfcli_protocol_pushRequest( m_impl, data, datasize))
	{
		m_rhqueue.pop_back();
		throw std::bad_alloc();
	}
	return true;
}

static const char* eventTypeName( wolfcli_ProtocolEventType type)
{
	switch (type)
	{
		case WOLFCLI_PROT_SEND:		return "SEND";
		case WOLFCLI_PROT_UIFORM:	return "UIFORM";
		case WOLFCLI_PROT_ANSWER:	return "ANSWER";
		case WOLFCLI_PROT_STATE:	return "STATE";
		case WOLFCLI_PROT_REQERR:	return "REQERR";
		case WOLFCLI_PROT_ERROR:	return "ERROR";
	}
	return "(null)";
}

std::string ProtocolHandler::eventstring( const wolfcli_ProtocolEvent* event)
{
	std::ostringstream msg;
	msg << eventTypeName( event->type) << " " << (event->id?event->id:"") << " '" << std::string( event->content, event->contentsize) << "'";
	return msg.str();
}

static wolfcli_ProtocolEvent* exceptionEvent( wolfcli_ProtocolEvent* rt, const wolfcli_ProtocolEvent* event, const char* wht)
{
	if (event->type == WOLFCLI_PROT_REQERR)
	{
		rt->type = WOLFCLI_PROT_REQERR;
	}
	else
	{
		rt->type = WOLFCLI_PROT_ERROR;
	}
	rt->content = wht;
	rt->contentsize = std::strlen( wht);
	return rt;
}

int ProtocolHandler::eventhandler( void* this_, const wolfcli_ProtocolEvent* event)
{
	try
	{
		((ProtocolHandler*)this_)->eventhandler( event);
		return 0;
	}
	catch (const std::exception& e)
	{
		wolfcli_ProtocolEvent ee;
		((ProtocolHandler*)this_)->eventhandler( exceptionEvent( &ee, event, e.what()));
		return -1;
	}
}

void ProtocolHandler::eventhandler( const wolfcli_ProtocolEvent* event)
{
	switch (event->type)
	{
		case WOLFCLI_PROT_SEND:
			sendData( event->content, event->contentsize);
			break;
		case WOLFCLI_PROT_UIFORM:
			receiveUIForm( event->content, event->contentsize);
			break;
		case WOLFCLI_PROT_REQERR:
			if (!m_rhqueue.empty()) std::logic_error("request error without request (ProtocolHandler::eventhandler)");
			(*m_rhqueue.begin())->handleError( event->content);
			m_rhqueue.erase( m_rhqueue.begin());
			break;
		case WOLFCLI_PROT_ANSWER:
			if (!m_rhqueue.empty()) std::logic_error("answer without request (ProtocolHandler::eventhandler)");
			(*m_rhqueue.begin())->handleAnswer( event->content, event->contentsize);
			m_rhqueue.erase( m_rhqueue.begin());
			break;
		case WOLFCLI_PROT_STATE:
			notifyState( event->id);
			break;
		case WOLFCLI_PROT_ERROR:
			notifyError( event->content);
			break;
	}
}

void ProtocolHandler::quit()
{
	wolfcli_protocol_quit( m_impl);
}

void ProtocolHandler::run()
{
	if (!ready()) throw std::logic_error( "called run in not ready state");
	switch (wolfcli_protocol_run( m_impl))
	{
		case WOLFCLI_CALL_DATA:
			recvData();
			break;
		case WOLFCLI_CALL_IDLE:
			notifyState( "idle");
			break;
		case WOLFCLI_CALL_ERROR:
			close();
			throw std::runtime_error( "error in session");
		case WOLFCLI_CALL_CLOSED:
			close();
			break;
	}
}


ConnectionHandler::~ConnectionHandler()
{
	if (m_impl) wolfcli_destroyConnection( m_impl);
}

void ConnectionHandler::sendData( const char* data, std::size_t datasize)
{
	if (!m_impl) throw std::runtime_error( "no connection (sendData)");
	if (!wolfcli_connection_write( m_impl, data, datasize))
	{
		throw std::runtime_error( "send data failed");
	}
}

void ConnectionHandler::recvData()
{
	if (!m_impl) throw std::runtime_error( "no connection (recvData)");
	if (!wolfcli_connection_read( m_impl))
	{
		throw std::runtime_error( "recv data failed");
	}
}

bool ConnectionHandler::ready() const
{
	return m_impl && wolfcli_connection_state( m_impl) == WOLFCLI_CONNSTATE_READY;
}

void ConnectionHandler::close()
{
	if (m_impl) wolfcli_destroyConnection( m_impl);
	m_impl = 0;
}

static const char* eventTypeName( wolfcli_ConnectionEventType type)
{
	switch (type)
	{
		case WOLFCLI_CONN_DATA:		return "DATA";
		case WOLFCLI_CONN_STATE:	return "STATE";
		case WOLFCLI_CONN_ERROR:	return "ERROR";
	}
	return "(unknown)";
}

std::string ConnectionHandler::eventstring( const wolfcli_ConnectionEvent* event)
{
	std::ostringstream msg;
	msg << eventTypeName( event->type) << " '" << std::string( event->content, event->contentsize) << "'";
	return msg.str();
}

static wolfcli_ConnectionEvent* exceptionEvent( wolfcli_ConnectionEvent* rt, const char* wht)
{
	rt->type = WOLFCLI_CONN_ERROR;
	rt->content = wht;
	rt->contentsize = std::strlen( wht);
	return rt;
}

int ConnectionHandler::eventhandler( void* this_, const wolfcli_ConnectionEvent* event)
{
	try
	{
		((ConnectionHandler*)this_)->eventhandler( event);
		return 0;
	}
	catch (const std::exception& e)
	{
		wolfcli_ConnectionEvent ee;
		((ConnectionHandler*)this_)->eventhandler( exceptionEvent( &ee, e.what()));
		return -1;
	}
}

void ConnectionHandler::eventhandler( const wolfcli_ConnectionEvent* event)
{
	switch (event->type)
	{
		case WOLFCLI_CONN_DATA:
			pushData( event->content, event->contentsize);
			break;
		case WOLFCLI_CONN_STATE:
			notifyState( event->content);
			break;
		case WOLFCLI_CONN_ERROR:
			notifyError( event->content);
			break;
	}
}



