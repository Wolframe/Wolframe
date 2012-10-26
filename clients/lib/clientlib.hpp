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
///\file clientlib.hpp
///\brief C++ client library interface

#ifndef _WOLFRAME_CLIENTLIB_HPP_INCLUDED
#define _WOLFRAME_CLIENTLIB_HPP_INCLUDED
#include "clientlib.h"
#include <string>
#include <list>
#include <cstring>
#include <iostream>
#include <sstream>

namespace _Wolframe {
namespace client {

struct RequestHandler
{
	virtual ~RequestHandler(){}
	virtual void handleAnswer( const char* data, std::size_t datasize);
	virtual void handleError( const std::string& msg);
};

struct SessionHandler
{
	virtual void doRequest( RequestHandler* handler, const char* data, std::size_t datasize)=0;
	virtual void receiveUIForm( const char* data, std::size_t datasize)=0;
	virtual void pushData( const char* data, std::size_t datasize)=0;
	virtual void sendData( const char* data, std::size_t datasize)=0;
	virtual void debugMessage( const std::string& msg)=0;
	virtual void notifyState( const std::string& msg)=0;
	virtual void notifyError( const std::string& msg)=0;
};

class ProtocolHandler :virtual SessionHandler
{
public:
	ProtocolHandler();

	virtual ~ProtocolHandler(){}

	virtual void pushData( const char* data, std::size_t datasize);
	virtual void doRequest( RequestHandler* handler, const char* data, std::size_t datasize);

private:
	static const char* eventTypeName( wolfcli_ProtocolEventType type);
	static std::string eventstring( const wolfcli_ProtocolEvent* event);
	static void notifier( void* this_, const wolfcli_ProtocolEvent* event)
	{
		((ProtocolHandler*)this_)->notifier( event);
	}
	void notifier( const wolfcli_ProtocolEvent* event);

private:
	wolfcli_ProtocolHandler m_impl;
	std::string m_lasterror;
	std::list<RequestHandler*> m_rhqueue;
};


class ConnectionHandler :virtual SessionHandler
{
public:
	ConnectionHandler();

	virtual ~ConnectionHandler(){}

	virtual void pushData( const char* data, std::size_t datasize);
	virtual void doRequest( RequestHandler* handler, const char* data, std::size_t datasize);
private:
};


}} //namespace
#endif

