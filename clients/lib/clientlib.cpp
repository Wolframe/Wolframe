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
	m_impl = wolfcli_createProtocolHandler( &ProtocolHandler::notifier, this);
	if (!m_impl) throw std::runtime_error( m_lasterror);
}

void ProtocolHandler::pushData( const char* data, std::size_t datasize)
{
	if (!wolfcli_protocol_pushData( m_impl, data, datasize))
	{
		throw std::bad_alloc();
	}
}

void ProtocolHandler::doRequest( RequestHandler* handler, const char* data, std::size_t datasize)
{
	if (!wolfcli_protocol_pushRequest( m_impl, data, datasize))
	{
		throw std::bad_alloc();
	}
	m_rhqueue.push_back( handler);
}

const char* ProtocolHandler::eventTypeName( wolfcli_ProtocolEventType type)
{
	switch (type)
	{
		case WOLFCLI_PROT_SEND_DATA:	return "SEND";
		case WOLFCLI_PROT_UIFORM:	return "UIFORM";
		case WOLFCLI_PROT_REQUEST:	return "REQUEST";
		case WOLFCLI_PROT_ANSWER:	return "ANSWER";
		case WOLFCLI_PROT_STATE:	return "STATE";
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

void ProtocolHandler::notifier( const wolfcli_ProtocolEvent* event)
{
	switch (event->type)
	{
		case WOLFCLI_PROT_SEND_DATA:
			sendData( event->content, event->contentsize);
			break;
		case WOLFCLI_PROT_UIFORM:
			receiveUIForm( event->content, event->contentsize);
			break;
		case WOLFCLI_PROT_REQUEST:
			throw std::logic_error("request should not get here (notifier)");
		case WOLFCLI_PROT_ANSWER:
			if (!m_rhqueue.empty()) std::logic_error("answer without request (notifier)");
			(*m_rhqueue.begin())->handleAnswer( event->content, event->contentsize);
			m_rhqueue.erase( m_rhqueue.begin());
			break;
		case WOLFCLI_PROT_STATE:
			notifyState( std::string( event->content, event->contentsize));
			break;
		case WOLFCLI_PROT_ERROR:
			m_lasterror = std::string( event->id?event->id:"") + ": " + std::string( event->content, event->contentsize);
			if (m_impl) notifyError( eventstring( event));
	}
	if (m_impl) debugMessage( eventstring( event));
}



