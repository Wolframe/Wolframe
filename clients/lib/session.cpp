/************************************************************************

 Copyright (C) 2011 - 2013 Project Wolframe.
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
///\file session.cpp
///\brief Implementation C++ client session
#include "session.hpp"
#include <stdexcept>

using namespace _Wolframe;
using namespace _Wolframe::client;

struct Session::Impl
{
	Impl( const Configuration& config, void* this_)
		:m_connection(config,&m_protocol,&connectionCallback_void,this_)
		,m_protocol(config,&protocolCallback_void,this_){}

	Connection m_connection;		//< connection handler
	Protocol m_protocol;			//< protocol statemachine
};

Session::Session( const Configuration& config)
	:m_impl(0)
{
	m_impl = new Impl( config, this);
}

Session::~Session()
{
	delete m_impl;
}

void Session::start()
{
	m_impl->m_connection.connect();
}

void Session::quit()
{
	m_impl->m_protocol.doQuit();
}

void Session::stop()
{
	m_impl->m_protocol.doQuit();
	m_impl->m_connection.post_request();
	m_impl->m_connection.stop();
}

void Session::requestCallback( void* this_, const Protocol::Event& event)
{
	switch (event.type())
	{
		case Protocol::Event::UIFORM: throw std::logic_error( "request handler cannot handle answer event UIFORM");
		case Protocol::Event::ANSWER: ((RequestHandler*)this_)->answer( event.content(), event.contentsize()); break;
		case Protocol::Event::STATE: throw std::logic_error( "request handler cannot handle answer event STATE");
		case Protocol::Event::ATTRIBUTE: throw std::logic_error( "request handler cannot handle answer event ATTRIBUTE");
		case Protocol::Event::ERROR: ((RequestHandler*)this_)->error( event.id()); break;
	}
}

bool Session::doRequest( RequestHandler* handler, const std::string& request, const char* data, std::size_t datasize)
{
	bool rt = m_impl->m_protocol.pushRequest( requestCallback, handler, request, data, datasize);
	m_impl->m_connection.post_request();
	return rt;
}

void Session::protocolCallback( const Protocol::Event& event)
{
	switch (event.type())
	{
		case Protocol::Event::UIFORM:
			receiveUIForm( event.id(), event.content(), event.contentsize());
			break;

		case Protocol::Event::ANSWER:
			std::logic_error("answer not passed to request (SessionHandler::protocolCallback)");

		case Protocol::Event::STATE:
			notifyState( event.id());
			break;

		case Protocol::Event::ATTRIBUTE:
			notifyAttribute( event.id(), event.content());
			break;

		case Protocol::Event::ERROR:
			notifyError( event.id());
			break;
	}
}

void Session::protocolCallback_void( void* this_, const Protocol::Event& event)
{
	return ((Session*)this_)->protocolCallback( event);
}


void Session::connectionCallback( const Connection::Event& event)
{
	switch (event.type())
	{
		case Connection::Event::READY:
			notifyReady();
			break;

		case Connection::Event::STATE:
			notifyState( event.content());
			break;

		case Connection::Event::FAILED:
			notifyError( event.content());
			break;

		case Connection::Event::TERMINATED:
			notifyTermination();
			break;
	}
}

void Session::connectionCallback_void( void* this_, const Connection::Event& event)
{
	return ((Session*)this_)->connectionCallback( event);
}


