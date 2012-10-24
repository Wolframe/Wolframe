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
///\brief C client library protocol interface
//
// Provide an abstraction of the wolframe client statemachine, so that the client
// can process complete data items (UI-forms, requests, answers).
// The idea is that the client creates an object that describes what type of session
// should be established.
// When the session is established the client pushes requests. Callbacks are used to notify
// the client with the answers and resulting events with complete data.
//

#ifndef _WOLFRAME_CLIENTLIB_HPP_INCLUDED
#define _WOLFRAME_CLIENTLIB_HPP_INCLUDED
#include <stdint.h>
#include <stddef.h>

///\defgroup protocol
///@{

///\struct wolfcli_ProtocolEventType
///\brief Enumeration of protocol event types
typedef enum
{
	WOLFCLI_PROT_UIFORM=	1,	//< UI form sent from server to client in the initialization phase
	WOLFCLI_PROT_SEND_DATA=	2,	//< data with messages from the protocol to be sent to the server
	WOLFCLI_PROT_ERROR=	3,	//< error reported by the server
	WOLFCLI_PROT_STATE=	4,	//< selected state info for the client
	WOLFCLI_PROT_REQUEST=	5	//< (internal) request from client to server in a session
}
wolfcli_ProtocolEventType;

///\struct wolfcli_ProtocolEvent
///\brief Structure for a protocl event to the client and for internal events of the event handler
typedef struct wolfcli_ProtocolEvent_
{
	wolfcli_ProtocolEventType type;	//< type of the protocol event
	int context;			//< context identifier given by the caller
	char* id;			//< protocol event identifier
	char* content;			//< data of the protocol event
	size_t contentsize;		//< size of the event data in bytes
}
wolfcli_ProtocolEvent;

///\brief PIMPL for internal protocol event handler data structure
typedef struct wolfcli_ProtocolHandlerStruct* wolfcli_ProtocolHandler;

///\brief Callback function type for the notification of protocol events to the client
typedef void (*wolfcli_ProtocolEventCallback)( void* clientobject, wolfcli_ProtocolEvent* event);

///\brief Constructor of a protocol event handler
wolfcli_ProtocolHandler wolfcli_createProtocolHandler(
	wolfcli_ProtocolEventCallback notifier_,	//< callback for event notification
	void* clientobject_);				//< first parameter of notifier_

///\brief Destructor of a protocol event handler
void wolfcli_destroyProtocolHandler(
	wolfcli_ProtocolHandler handler);

///\brief Method to the protocol event handler with data received from the server (after run returned CALL_DATA)
int wolfcli_protocol_pushData(
	wolfcli_ProtocolHandler handler,		//< protocol event handler
	const char* data,				//< poiner to data to push (decrypted plain data)
	size_t datasize);				//< size of data in bytes

///\brief Method to push a request to the server that will be served when the session is established
int wolfcli_protocol_pushRequest(
	wolfcli_ProtocolHandler handler,		//< protocol event handler
	unsigned int context,				//< context identifier
	const char* data,				//< request data (decrypted plain data)
	size_t datasize);				//< size of data in bytes

typedef enum
{
	WOLFCLI_CALL_DATA,				//< the protocol event handler is requesting more data. all input (complete lines have been consumed)
	WOLFCLI_CALL_IDLE,				//< the protocol event handler is in an established session state and has processed all requests until now.
	WOLFCLI_CALL_ERROR,				//< an internal error (memory allocation error) has occurred
	WOLFCLI_CALL_CLOSED				//< the client has to terminate becaues it cannot work anymore or the server will close the connection
} wolfcli_CallResult;

///\brief Call the protocol event handler state machine to establish the session and to process the client requests to the server
wolfcli_CallResult wolfcli_protocol_run( wolfcli_ProtocolHandler handler);

///@} end group protocol





///\defgroup connection
///@{

///\struct wolfcli_ConnectionEventType
///\brief Enumeration of connection event types
typedef enum
{
	WOLFCLI_COMM_CONNECTED= 0,
	WOLFCLI_COMM_READ=	1,	//< connection is ready to read
	WOLFCLI_COMM_WRITE=	2,	//< connection is ready to write
	WOLFCLI_COMM_TIMEOUT=	3,	//< connection got timeout
	WOLFCLI_COMM_CLOSED=	4,	//< connection closed
	WOLFCLI_COMM_ERROR=	5	//< connection error
}
wolfcli_ConnectionEventType;

///\brief PIMPL for internal connection data structure
typedef struct wolfcli_ConnectionStruct wolfcli_Connection;

///\brief Callback function for notifying connection events
typedef void (*wolfcli_ConnectionEventCallback)(
	wolfcli_Connection conn,
	void* clientobject,
	wolfcli_ConnectionEventType event);

///\brief Create a connection (plain tcp)
wolfcli_Connection wolfcli_createConnection(
	const char* address,
	const char* name,
	void* clientobject,
	wolfcli_ConnectionEventCallback notifier);

///\brief Create a connection (with SSL layer)
wolfcli_Connection wolfcli_createConnection_SSL(
	const char* address,
	const char* name,
	void* clientobject,
	wolfcli_ConnectionEventCallback notifier,
	const char* CA_cert_file,
	const char* client_cert_file,
	const char* client_cert_key);

void wolfcli_destroyConnection(
	wolfcli_Connection conn);

size_t wolfcli_connection_read(
	wolfcli_Connection conn,
	char* buf,
	size_t bufsize);

size_t wolfcli_connection_write(
	wolfcli_Connection conn,
	const char* data,
	size_t datasize);

///@} end group connection

#endif


