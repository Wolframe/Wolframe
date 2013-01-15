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
///\file clientlib.h
///\brief C client library interface

#ifndef _WOLFRAME_CLIENTLIB_H_INCLUDED
#define _WOLFRAME_CLIENTLIB_H_INCLUDED
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

///\defgroup protocol
///\brief Interface to the client protocol handling.
// Provides an abstraction of the wolframe client protocol statemachine
// to process complete data items.
///@{

///\struct wolfcli_ProtocolEventType
///\brief Enumeration of protocol event types
typedef enum
{
	WOLFCLI_PROT_SEND=	1,	//< data with messages from the protocol to be sent to the server
	WOLFCLI_PROT_UIFORM=	2,	//< UI form sent from server to client in the initialization phase
	WOLFCLI_PROT_ANSWER=	3,	//< answer of a requset from the server to the client in a session
	WOLFCLI_PROT_STATE=	4,	//< selected state info for the client
	WOLFCLI_PROT_REQERR=	5,	//< error in request (negative answer)
	WOLFCLI_PROT_ERROR=	6	//< error reported by the server
}
wolfcli_ProtocolEventType;

///\struct wolfcli_ProtocolEvent
///\brief Structure for a protocl event to the client and for internal events of the event handler
typedef struct wolfcli_ProtocolEvent_
{
	wolfcli_ProtocolEventType type;	//< type of the protocol event
	const char* id;			//< protocol event identifier
	const char* content;		//< data of the protocol event
	size_t contentsize;		//< size of the event data in bytes
}
wolfcli_ProtocolEvent;

///\brief PIMPL for internal protocol event handler data structure
typedef struct wolfcli_ProtocolHandlerStruct* wolfcli_ProtocolHandler;

///\brief Callback function type for the notification of protocol events to the client
typedef int (*wolfcli_ProtocolEventCallback)( void* clientobject, const wolfcli_ProtocolEvent* event);

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
	const char* data,				//< request data (decrypted plain data)
	size_t datasize);				//< size of data in bytes

///\brief Return 1, if requests can be processed
int wolfcli_protocol_open( wolfcli_ProtocolHandler handler);

///\brief Quit session when all requests are processed
void wolfcli_protocol_quit( wolfcli_ProtocolHandler handler);



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
///\brief Interface to the client connection handling
///@{

///\struct wolfcli_ConnectionEventType
///\brief Enumeration of connection event types
typedef enum
{
	WOLFCLI_CONN_DATA=	1,	//< connection has data to read
	WOLFCLI_CONN_STATE=	2,	//< state of connection
	WOLFCLI_CONN_ERROR=	3	//< connection error
}
wolfcli_ConnectionEventType;

///\struct wolfcli_ConnectionEvent
///\brief Structure for a connection event
typedef struct wolfcli_ConnectionEvent_
{
	wolfcli_ConnectionEventType type;	//< type of the connection event
	const char* content;			//< data of the connection event
	size_t contentsize;			//< size of the event data in bytes
}
wolfcli_ConnectionEvent;

///\brief PIMPL for internal connection data structure
typedef struct wolfcli_ConnectionStruct* wolfcli_Connection;

///\brief Callback function for notifying connection events
typedef int (*wolfcli_ConnectionEventCallback)(
	void* clientobject,
	const wolfcli_ConnectionEvent* event);

///\brief Create a connection (plain TCP/IP)
wolfcli_Connection wolfcli_createConnection(
	const char* address,
	const char* name,
	unsigned short connect_timeout,
	unsigned short read_timeout,
	void* clientobject,
	wolfcli_ConnectionEventCallback notifier);

#ifdef WITH_SSL
///\brief Create a connection (with SSL layer)
wolfcli_Connection wolfcli_createConnection_SSL(
	const char* address,
	const char* name,
	unsigned short connect_timeout,
	unsigned short read_timeout,
	void* clientobject,
	wolfcli_ConnectionEventCallback notifier,
	const char* CA_cert_file,
	const char* client_cert_file,
	const char* client_cert_key);
#endif

void wolfcli_destroyConnection( wolfcli_Connection conn);

typedef enum
{
	WOLFCLI_CONNSTATE_NULL=		0,
	WOLFCLI_CONNSTATE_INIT=		1,	//< connection is in initialization
	WOLFCLI_CONNSTATE_OPEN=		2,	//< connection context is created
	WOLFCLI_CONNSTATE_READY=	3,	//< connection is ready
	WOLFCLI_CONNSTATE_CLOSED=	4	//< connection has been closed
}
wolfcli_ConnectionState;

wolfcli_ConnectionState wolfcli_connection_state( wolfcli_Connection conn);

int wolfcli_connection_read( wolfcli_Connection conn);

int wolfcli_connection_write(
	wolfcli_Connection conn,
	const char* data,
	size_t datasize);

///@} end group connection

#ifdef __cplusplus
}//extern "C"
#endif

#endif


