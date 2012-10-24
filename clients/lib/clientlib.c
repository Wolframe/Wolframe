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
///\file clientlib.c
///\brief C client library protocol interface
#include "clientlib.h"
#include <stdlib.h>
#include <string.h>

typedef struct wolfcli_EventQueueItem_
{
	wolfcli_Event* event;
	struct wolfcli_EventQueueItem_* next;
	struct wolfcli_EventQueueItem_* prev;
}
wolfcli_EventQueueItem;

typedef struct wolfcli_EventQueue_
{
	wolfcli_EventQueueItem* head;
	wolfcli_EventQueueItem* tail;
}
wolfcli_EventQueue;

typedef struct wolfcli_Buffer_
{
	char* ptr;
	size_t size;
	size_t allocsize;
}
wolfcli_Buffer;

typedef enum
{
	WOLFCLI_STATE_INIT,
	WOLFCLI_STATE_AUTH,
	WOLFCLI_STATE_AUTH_WAIT,
	WOLFCLI_STATE_LOAD_UIFORM,
	WOLFCLI_STATE_LOAD_UIFORM_DATA,
	WOLFCLI_STATE_SESSION,
	WOLFCLI_STATE_SESSION_ANSWER,
	WOLFCLI_STATE_CLOSED
} wolfcli_EventHandlerState;

struct wolfcli_EventHandler_
{
	wolfcli_EventCallback* eventNotifier;
	void* clientobject;
	wolfcli_EventQueue requestqueue;
	wolfcli_Buffer buffer;
	size_t bufferpos;
	wolfcli_EventHandlerState state;
	wolfcli_Buffer statearg;
	wolfcli_Buffer docbuffer;
};

static void destroyEvent( wolfcli_Event* event)
{
	if (event != NULL)
	{
		if (event->content != NULL) free( event->content);
		if (event->id != NULL) free( event->content);
		free( event);
	}
}

static wolfcli_Event* createEvent(
	wolfcli_EventType type_,
	int context_,
	const char* id_,
	const void* content_,
	size_t contentsize_)
{
	wolfcli_Event* rt = (wolfcli_Event*)calloc( 1, sizeof( wolfcli_Event));
	if (rt == NULL) goto ERROR;
	if (content_ != NULL)
	{
		rt->content = (char*)malloc( rt->contentsize = contentsize_);
		if (rt->content == NULL) goto ERROR;
		memcpy( rt->content, content_, contentsize_);
	}
	if (id_ != NULL)
	{
		size_t idsize = strlen( id_)+1;
		rt->id = (char*)malloc( idsize);
		if (rt->id == NULL) goto ERROR;
		memcpy( rt->id, id_, idsize);
	}
	rt->type = type_;
	rt->context = context_;
	return rt;
ERROR:
	destroyEvent( rt);
	return NULL;
}

static void initQueue( wolfcli_EventQueue* que)
{
	que->head = NULL;
	que->tail = NULL;
}

static void resetQueue( wolfcli_EventQueue* que)
{
	wolfcli_EventQueueItem* itm = que->head;
	while (itm != NULL)
	{
		wolfcli_EventQueueItem* next = itm->next;
		destroyEvent( itm->event);
		free( itm);
		itm = next;
	}
	que->head = NULL;
	que->tail = NULL;
}

static int queuePut( wolfcli_EventQueue* que, wolfcli_Event* event_)
{
	wolfcli_EventQueueItem* item = (wolfcli_EventQueueItem*)calloc( 1, sizeof( wolfcli_EventQueueItem));
	if (!item) return 0;
	item->event = event_;
	item->next = que->head;
	item->prev = NULL;
	if (que->head)
	{
		que->head->prev = item;
	}
	else
	{
		que->tail = item;
	}
	que->head = item;
	return 1;
}

static wolfcli_Event* queueGet( wolfcli_EventQueue* que)
{
	wolfcli_Event* rt;
	wolfcli_EventQueueItem* item = que->tail;
	if (que->tail == NULL) return 0;
	que->tail = que->tail->prev;
	if (que->tail == NULL)
	{
		que->head = NULL;
	}
	else
	{
		que->tail->next = NULL;
	}
	rt = item->event;
	free( item);
	return rt;
}

static void initBuffer( wolfcli_Buffer* buffer, size_t allocsize_)
{
	buffer->ptr = NULL;
	buffer->size = 0;
	buffer->allocsize = 0;
	if (allocsize_)
	{
		buffer->ptr = (char*)malloc( allocsize_);
		if (buffer->ptr) buffer->allocsize = allocsize_;
	}
}

static void resetBuffer( wolfcli_Buffer* buffer, size_t allocsize_)
{
	if (buffer->ptr != NULL) free( buffer->ptr);
	initBuffer( buffer, allocsize_);
}

#define BUFFER_INITSIZE (1<<14)
static int bufferAppend( wolfcli_Buffer* buffer, const char* data, size_t datasize)
{
	char* newptr;
	size_t newallocsize = buffer->allocsize?buffer->allocsize:BUFFER_INITSIZE;
	while (newallocsize < buffer->size + datasize)
	{
		size_t mm = newallocsize *= 2;
		if (mm < newallocsize) return 0;
		newallocsize = mm;
	}
	newptr = (char*)realloc( buffer->ptr, newallocsize);
	if (newptr == NULL) return 0;
	buffer->ptr = newptr;
	buffer->allocsize = newallocsize;
	memcpy( buffer->ptr + buffer->size, data, datasize);
	buffer->size += datasize;
	return 1;
}

static int bufferAppendString( wolfcli_Buffer* buffer, const char* str)
{
	return bufferAppend( buffer, str, strlen(str)+1);
}

static void bufferConsume( wolfcli_Buffer* buffer, size_t pos)
{
	if (pos == 0 || buffer->size/4 > pos) return;
	size_t newallocsize;
	memmove( buffer->ptr, buffer->ptr+pos, buffer->size - pos);
	buffer->size = buffer->size - pos;
	newallocsize = buffer->allocsize;
	if (buffer->size < BUFFER_INITSIZE/2)
	{
		newallocsize = BUFFER_INITSIZE;
	}
	if (buffer->allocsize > newallocsize)
	{
		char* newptr = (char*)realloc( buffer->ptr, newallocsize);
		if (newptr)
		{
			buffer->ptr = newptr;
			buffer->allocsize = newallocsize;
		}
	}
}

wolfcli_EventHandler* wolfcli_createEventHandler(
	wolfcli_EventCallback* notifier_,
	void* clientobject_)
{
	wolfcli_EventHandler* handler = (wolfcli_EventHandler*) calloc( 1, sizeof(wolfcli_EventHandler));
	if (!handler) return NULL;
	handler->eventNotifier = notifier_;
	handler->clientobject = clientobject_;
	initQueue( &handler->requestqueue);
	initBuffer( &handler->buffer, 0);
	handler->bufferpos = 0;
	handler->state = WOLFCLI_STATE_INIT;
	initBuffer( &handler->statearg, 0);
	initBuffer( &handler->docbuffer, 0);
	return handler;
}

void wolfcli_destroyEventHandler( wolfcli_EventHandler* handler)
{
	resetQueue( &handler->requestqueue);
	resetBuffer( &handler->buffer, 0);
	handler->bufferpos = 0;
	handler->state = WOLFCLI_STATE_INIT;
	resetBuffer( &handler->statearg, 0);
	resetBuffer( &handler->docbuffer, 0);
	free( handler);
}

typedef struct Line_
{
	char* ptr;
	size_t size;
} Line;

static int getLine( Line* line, char* baseptr, size_t* pos, size_t size, int withCR)
{
	size_t ii,nn;
	if (*pos == size) return 0;
	line->ptr = baseptr + *pos;
	nn = size - *pos;
	for (ii = 0; ii < nn && line->ptr[ii] != '\n'; ++ii);
	if (ii < nn)
	{
		if (!withCR && ii > 0 && line->ptr[ii-1] == '\r')
		{
			line->size = ii-1;
		}
		else
		{
			line->size = ii;
		}
		line->ptr[ line->size] = 0;
		*pos = ii+1;
		return 1;
	}
	return 0;
}

#define MAX_LINESPLIT_SIZE 64
typedef struct LineSplit_
{
	const char* ptr[ MAX_LINESPLIT_SIZE];
	size_t size;
} LineSplit;

static int getLineSplit_space( LineSplit* split, Line* line, size_t max_nof_elements)
{
	size_t ii;
	if (max_nof_elements > MAX_LINESPLIT_SIZE) return 0;
	for (ii=0; ii < MAX_LINESPLIT_SIZE; ++ii) split->ptr[ ii] = NULL;
	split->size = 0;
	for (ii=0; ii < line->size && line->ptr[ii] >= 0 && line->ptr[ii] <= 32; ++ii);
	if (ii == line->size) return 1;
	split->ptr[ line->size++] = line->ptr + ii;

	for (; ii < line->size; ++ii)
	{
		if (line->ptr[ii] >= 0 && line->ptr[ii] <= 32)
		{
			line->ptr[ii] = 0;
			if (split->size == max_nof_elements) return 1;
			for (; ii < line->size && line->ptr[ii] > 0 && line->ptr[ii] < 32; ++ii);
			if (ii == line->size) return 1;
			split->ptr[ ++split->size] = line->ptr + ii;
		}
	}
	return 1;
}

typedef enum
{
	DOCSTATE_EOD,
	DOCSTATE_READING,
	DOCSTATE_ERROR
} DocState;

static DocState getContentUnescaped( wolfcli_Buffer* docbuffer, char* baseptr, size_t* pos, size_t size)
{
	Line line;
	while (getLine( &line, baseptr, pos, size, 0))
	{
		if (line.ptr[0] == '.')
		{
			if (line.size == 1) return DOCSTATE_EOD;
			if (!bufferAppend( docbuffer, line.ptr+1, line.size-1)) return DOCSTATE_ERROR;
		}
		else
		{
			if (!bufferAppend( docbuffer, line.ptr, line.size)) return DOCSTATE_ERROR;
		}
	}
	return DOCSTATE_READING;
}

static int getContentEscaped( wolfcli_Buffer* docbuffer, char* ptr, size_t size)
{
	Line line;
	size_t pos = 0;
	while (getLine( &line, ptr, &pos, size, 1))
	{
		if (line.ptr[0] == '.')
		{
			if (!bufferAppend( docbuffer, line.ptr, 1)) return 0;
			if (!bufferAppend( docbuffer, line.ptr, line.size)) return 0;
		}
		else
		{
			if (!bufferAppend( docbuffer, line.ptr, line.size)) return 0;
		}
	}
	if (!bufferAppend( docbuffer, "\r\n.\r\n", 5)) return 0;
	return 1;
}

int wolfcli_EventHandler_pushData( wolfcli_EventHandler* handler, const char* data, size_t datasize)
{
	return bufferAppend( &handler->buffer, data, datasize);
}

static int sendLine( wolfcli_EventHandler* handler, const char* msg, const char* arg)
{
	wolfcli_Event* event = NULL;
	char linebuf[ 1024];
	size_t msgsize = strlen( msg);
	size_t argsize = arg?strlen( arg):0;
	if (sizeof(linebuf)-2 < argsize) goto _ERROR;
	if (sizeof(linebuf)-2 < msgsize) goto _ERROR;
	if (sizeof(linebuf)-2 < msgsize+argsize+1) goto _ERROR;
	memcpy( linebuf, msg, msgsize);
	if (arg)
	{
		linebuf[ msgsize] = ' ';
		memcpy( linebuf+msgsize+1, arg, argsize);
		msgsize += argsize + 1;
	}
	linebuf[ msgsize] = '\r';
	linebuf[ msgsize+1] = '\n';
	event = createEvent( WOLFCLI_SEND_DATA, 0, 0, linebuf, msgsize+2);
	if (!event) goto _ERROR;
	(*handler->eventNotifier)( handler->clientobject, event);
	destroyEvent( event);
	return 1;
_ERROR:
	if (!event) destroyEvent( event);
	return 0;
}

static int sendDocument( wolfcli_EventHandler* handler, int context, char* id, char* doc, size_t docsize)
{
	wolfcli_Event* event = NULL;
	wolfcli_Buffer docbuffer;
	initBuffer( &docbuffer, 0);

	if (!getContentEscaped( &docbuffer, doc, docsize)) goto _ERROR;
	event = createEvent( WOLFCLI_SEND_DATA, context, id, docbuffer.ptr, docbuffer.size);
	if (!event) goto _ERROR;
	(*handler->eventNotifier)( handler->clientobject, event);
	resetBuffer( &docbuffer, 0);
	destroyEvent( event);
	return 1;
_ERROR:
	resetBuffer( &docbuffer, 0);
	if (!event) destroyEvent( event);
	return 0;
}


static int notifyError( wolfcli_EventHandler* handler, const char* id)
{
	wolfcli_Event* event = createEvent( WOLFCLI_ERROR, 0, id, 0, 0);
	if (!event) return 0;
	(*handler->eventNotifier)( handler->clientobject, event);
	destroyEvent( event);
	return 1;
}

static int notifyErrorMessage( wolfcli_EventHandler* handler, const char* id, int context, const char* msg)
{
	wolfcli_Event* event = createEvent( WOLFCLI_ERROR, context, id, msg, msg?strlen(msg):0);
	if (!event) return 0;
	(*handler->eventNotifier)( handler->clientobject, event);
	destroyEvent( event);
	return 1;
}

static int notifyState( wolfcli_EventHandler* handler, const char* id)
{
	wolfcli_Event* event = createEvent( WOLFCLI_STATE, 0, id, 0, 0);
	if (!event) return 0;
	(*handler->eventNotifier)( handler->clientobject, event);
	destroyEvent( event);
	return 1;
}

static int notifyUIForm( wolfcli_EventHandler* handler)
{
	wolfcli_Event* event = createEvent( WOLFCLI_UIFORM, 0, handler->statearg.ptr, handler->docbuffer.ptr, handler->docbuffer.size);
	if (!event) return 0;
	(*handler->eventNotifier)( handler->clientobject, event);
	destroyEvent( event);
	return 1;
}

int wolfcli_EventHandler_pushRequest( wolfcli_EventHandler* handler, unsigned int context, const char* data, size_t datasize)
{
	int rt;
	wolfcli_Event* event = createEvent( WOLFCLI_REQUEST, context, 0, data, datasize);
	if (!event) return 0;
	rt = queuePut( &handler->requestqueue, event);
	if (!rt) destroyEvent( event);
	return rt;
}

static int isequal( const char* aa, const char* bb)
{
	size_t ii = 0;
	for (; aa[ii] && (aa[ii]|32) == (bb[ii]|32); ++ii);
	return (aa[ii] == 0 && bb[ii] == 0);
}

static const char* selectAuthMech( wolfcli_EventHandler* handler, const LineSplit* arg)
{
	size_t ii;
	if (handler == 0)
	{
		for (ii=1; ii<arg->size; ++ii)
		{
			if (isequal( arg->ptr[ii], "NONE")) return "NONE";
		}
	}
	return 0;
}

wolfcli_CallResult wolfcli_EventHandler_run( wolfcli_EventHandler* handler)
{
	Line line;
	LineSplit arg;
	wolfcli_Event* event;
	const char* authmech;
	const char* msg;
	DocState docState;
	int success;

	for (;;) switch (handler->state)
	{
		case WOLFCLI_STATE_INIT:
			if (!sendLine( handler, "AUTH", 0)) goto _ERROR;
			handler->state = WOLFCLI_STATE_AUTH;
			continue;

		case WOLFCLI_STATE_AUTH:
			if (!getLine( &line, handler->buffer.ptr, &handler->bufferpos, handler->buffer.size, 0)) goto _DATA;
			if (!getLineSplit_space( &arg, &line, 2)) goto _ERROR;
			if (!arg.size) continue;
			if (isequal( arg.ptr[0], "MECHS"))
			{
				if (arg.size == 1)
				{
					if (!notifyError( handler, "NO AUTH MECHS")) goto _ERROR;
					goto _CLOSED;
				}
				if (!getLineSplit_space( &arg, &line, 0)) goto _ERROR;
				authmech = selectAuthMech( handler, &arg);
				if (!authmech)
				{
					if (!notifyError( handler, "NO MATCHING AUTH MECH")) goto _ERROR;
					goto _CLOSED;
				}
				if (!sendLine( handler, "MECH", authmech)) goto _ERROR;
				handler->state = WOLFCLI_STATE_AUTH_WAIT;
				continue;
			}
			else if (isequal( arg.ptr[0], "ERR"))
			{
				msg = (arg.size == 1)?0:arg.ptr[1];
				if (!notifyErrorMessage( handler, "AUTH command failed", 0, msg)) goto _ERROR;
				handler->state = WOLFCLI_STATE_INIT;
				continue;
			}
			else
			{
				if (!notifyError( handler, "PROTOCOL")) goto _ERROR;
				goto _CLOSED;
			}

		case WOLFCLI_STATE_AUTH_WAIT:
			if (!getLine( &line, handler->buffer.ptr, &handler->bufferpos, handler->buffer.size, 0)) goto _DATA;
			if (!getLineSplit_space( &arg, &line, 2)) goto _ERROR;
			if (!arg.size) continue;
			if (isequal( arg.ptr[0], "OK"))
			{
				if (!notifyState( handler, "authorized")) goto _ERROR;
				if (!sendLine( handler, "INTERFACE", 0)) goto _ERROR;
				handler->state = WOLFCLI_STATE_LOAD_UIFORM;
				continue;
			}
			else if (isequal( arg.ptr[0], "ERR"))
			{
				msg = (arg.size == 1)?0:arg.ptr[1];
				if (!notifyErrorMessage( handler, "refused", 0, msg)) goto _ERROR;
				goto _CLOSED;
			}

		case WOLFCLI_STATE_LOAD_UIFORM:
			if (!getLine( &line, handler->buffer.ptr, &handler->bufferpos, handler->buffer.size, 0)) goto _DATA;
			if (!getLineSplit_space( &arg, &line, 2)) goto _ERROR;
			if (!arg.size) continue;
			if (isequal( arg.ptr[0], "UIFORM"))
			{
				const char* formid;
				if (arg.size == 1)
				{
					if (!notifyErrorMessage( handler, "protocol", 0, "UIFORM missing argument")) goto _ERROR;
					goto _CLOSED;
				}
				else
				{
					formid = arg.ptr[1];
					resetBuffer( &handler->statearg, strlen( formid)+1);
					bufferAppendString( &handler->statearg, formid);
					resetBuffer( &handler->docbuffer, 0);
					handler->state = WOLFCLI_STATE_LOAD_UIFORM_DATA;
					continue;
				}

			}
			else if (isequal( arg.ptr[0], "OK"))
			{
				if (!notifyState( handler, "Interfaces loaded")) goto _ERROR;
				handler->state = WOLFCLI_STATE_SESSION;
				continue;
			}
			else if (isequal( arg.ptr[0], "ERR"))
			{
				msg = (arg.size == 1)?0:arg.ptr[1];
				if (!notifyErrorMessage( handler, "UI formloader", 0, msg)) goto _ERROR;
				goto _CLOSED;
			}

		case WOLFCLI_STATE_LOAD_UIFORM_DATA:
			docState = getContentUnescaped( &handler->docbuffer, handler->buffer.ptr, &handler->bufferpos, handler->buffer.size);
			switch (docState)
			{
				case DOCSTATE_EOD:
					if (!notifyUIForm( handler)) goto _ERROR;
					resetBuffer( &handler->docbuffer, 0);
					resetBuffer( &handler->statearg, 0);
					handler->state = WOLFCLI_STATE_LOAD_UIFORM;
					continue;

				case DOCSTATE_READING:
					goto _DATA;

				case DOCSTATE_ERROR:
					goto _ERROR;
			}

		case WOLFCLI_STATE_SESSION:
			event = queueGet( &handler->requestqueue);
			if (!event) goto _IDLE;
			success = sendDocument( handler, event->context, event->id, event->content, event->contentsize);
			destroyEvent( event);
			if (!success) goto _ERROR;
			continue;

		case WOLFCLI_STATE_SESSION_ANSWER:
		//TODO: continue here with ANSWER, handle event of send document

		case WOLFCLI_STATE_CLOSED:
			goto _CLOSED;
	}
_DATA:
	bufferConsume( &handler->buffer, handler->bufferpos);
	return WOLFCLI_CALL_DATA;
_IDLE:
	return WOLFCLI_CALL_IDLE;
_ERROR:
	handler->state = WOLFCLI_STATE_CLOSED;
	return WOLFCLI_CALL_ERROR;
_CLOSED:
	handler->state = WOLFCLI_STATE_CLOSED;
	return WOLFCLI_CALL_CLOSED;
}


