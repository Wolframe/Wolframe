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
///\brief Client library interface
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
	virtual void handleError( const char* msg);
};

class SessionHandlerBase
{
public://callbacks implemented by the SessionHandler
	virtual void receiveUIForm( const char* data, std::size_t datasize)=0;
	virtual void notifyState( const char* msg)=0;
	virtual void notifyError( const char* msg)=0;

public://protocol
	virtual bool doRequest( RequestHandler* handler, const char* data, std::size_t datasize)=0;
	virtual void run()=0;
	virtual void quit()=0;
public://connection
	virtual bool ready() const=0;

private://protocol
	friend class ProtocolHandler;
	virtual void pushData( const char* data, std::size_t datasize)=0;
private://connection
	friend class ConnectionHandler;
	virtual void sendData( const char* data, std::size_t datasize)=0;
	virtual void recvData()=0;
	virtual void close()=0;
};


class ProtocolHandler :public virtual SessionHandlerBase
{
public:
	ProtocolHandler();
	virtual ~ProtocolHandler();

	virtual void pushData( const char* data, std::size_t datasize);
	virtual bool doRequest( RequestHandler* handler, const char* data, std::size_t datasize);
	virtual void run();
	virtual void quit();

private:
	static int eventhandler( void* this_, const wolfcli_ProtocolEvent* event);
	void eventhandler( const wolfcli_ProtocolEvent* event);
	static std::string eventstring( const wolfcli_ProtocolEvent* event);

private:
	wolfcli_ProtocolHandler m_impl;
	std::list<RequestHandler*> m_rhqueue;
};


class ConnectionHandler :public virtual SessionHandlerBase
{
public:
	ConnectionHandler();
	virtual ~ConnectionHandler();

	virtual void sendData( const char* data, std::size_t datasize);
	virtual void recvData();
	virtual bool ready() const;
	virtual void close();

private:
	static int eventhandler( void* this_, const wolfcli_ConnectionEvent* event);
	void eventhandler( const wolfcli_ConnectionEvent* event);
	static std::string eventstring( const wolfcli_ConnectionEvent* event);

private:
	wolfcli_Connection m_impl;
};


struct SessionHandler
	:public ProtocolHandler
	,public ConnectionHandler
{
	SessionHandler(){}
	virtual ~SessionHandler(){}
};

}} //namespace
#endif

