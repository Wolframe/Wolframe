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
///\file clientlib.h
///\brief Ansi-C client library protocol interface
//
// Provide an abstraction of the wolframe client statemachine, so that the client
// can react on events with complete data (UI-forms, Requests, Answers).
// The idea is that the client creates an object that describes what type of session
// should be established and with what certificates, etc.
// When the session is established he pushes Requests and get Events with complete data
// only from the system.
//

#ifndef _WOLFRAME_PROTOCOL_CLIENTLIB_HPP_INCLUDED
#define _WOLFRAME_PROTOCOL_CLIENTLIB_HPP_INCLUDED
#include <stdint.h>
#include <stddef.h>

///\struct wolfcli_EventType
///\brief Enumeration of event types
typedef enum
{
	WOLFCLI_UIFORM=		1,	//< UI form sent from server to client in the initialization phase
	WOLFCLI_SEND_DATA=	2,	//< data with messages from the protocol to be sent to the server
	WOLFCLI_ERROR=		3,	//< error reported by the server
	WOLFCLI_STATE=		4,	//< selected state info for the client
	WOLFCLI_REQUEST=	5	//< (internal) request from client to server in a session
}
wolfcli_EventType;

///\struct wolfcli_Event
///\brief Structure for an event to the client and for internal events of the event handler
typedef struct wolfcli_Event_
{
	wolfcli_EventType type;		//< type of the event
	int context;			//< context identifier given by the caller
	char* id;			//< event identifier
	char* content;			//< data of the event
	size_t contentsize;		//< size of the event data in bytes
}
wolfcli_Event;

///\brief PIMPL for internal event handler data structure
typedef struct wolfcli_EventHandler_ wolfcli_EventHandler;

///\brief Callback function type for the notification of events to the client
typedef void (*wolfcli_EventCallback)( void* clientobject, wolfcli_Event* event);

///\brief Constructor of an event handler
wolfcli_EventHandler* wolfcli_createEventHandler(
	wolfcli_EventCallback* notifier_,		//< callback for event notification
	void* clientobject_);				//< first parameter of notifier_

///\brief Destructor of an event handler
void wolfcli_destroyEventHandler(
	wolfcli_EventHandler* handler);

///\brief Method to server the event handler with data received from the server (after run returned CALL_DATA)
int wolfcli_EventHandler_pushData(
	wolfcli_EventHandler* handler,			//< event handler
	const char* data,				//< poiner to data to push (decrypted plain data)
	size_t datasize);				//< size of data in bytes

///\brief Method to push a request to the server that will be served when the session is established
int wolfcli_EventHandler_pushRequest(
	wolfcli_EventHandler* handler,			//< event handler
	unsigned int context,				//< context identifier
	const char* data,				//< request data (decrypted plain data)
	size_t datasize);				//< size of data in bytes

typedef enum
{
	WOLFCLI_CALL_DATA,				//< the event handler is requesting more data. all input (complete lines have been consumed)
	WOLFCLI_CALL_IDLE,				//< the event handler is in an established session state and has processed all requests until now.
	WOLFCLI_CALL_ERROR,				//< a severe error (memory allocation error) has occurred and we cannot continue
	WOLFCLI_CALL_CLOSED				//< the client has to terminate becaues it cannot work anymore or the server will close the connection
} wolfcli_CallResult;

///\brief Call the event handler state machine
wolfcli_CallResult wolfcli_EventHandler_run( wolfcli_EventHandler* handler);

#endif


