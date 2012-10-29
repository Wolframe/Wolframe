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
///\file clientlib_protocol.cpp
///\brief Implementation of the client protocol handling
#include "clientlib_protocol.hpp"
#include <list>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>

using namespace _Wolframe;
using namespace _Wolframe::client;

namespace {
struct Line
{
	char* ptr;
	size_t size;
};

struct Buffer
{
	Buffer()
		:m_ptr(0),m_size(0),m_allocsize(0){}
	Buffer( const char* ptr_, std::size_t size_)
		:m_ptr((char*)std::malloc(size_)),m_size(size_),m_allocsize(size_)
	{
		if (!m_ptr) throw std::bad_alloc();
		std::memcpy( m_ptr, ptr_, size_);
	}
	explicit Buffer( std::size_t allocsize_)
		:m_ptr((char*)std::malloc(allocsize_)),m_size(0),m_allocsize(allocsize_)
	{
		if (!m_ptr) throw std::bad_alloc();
	}
	void reset()
	{
		if (m_ptr)
		{
			std::free( m_ptr);
			m_ptr = 0;
		}
		m_size = 0;
		m_allocsize = 0;
	}
	void reset( const char* ptr_, std::size_t size_)
	{
		reset();
		m_ptr = (char*)std::malloc(size_);
		if (!m_ptr) throw std::bad_alloc();
		m_allocsize = m_size = size_;
		std::memcpy( m_ptr, ptr_, size_);
	}

	~Buffer()
	{
		if (m_ptr) std::free( m_ptr);
	}

	enum {BUFFER_INITSIZE=(1<<14)};
	void append( const char* data, size_t datasize)
	{
		std::size_t newallocsize = m_allocsize?m_allocsize:(std::size_t)BUFFER_INITSIZE;
		while (newallocsize < m_size + datasize)
		{
			std::size_t mm = newallocsize * 2;
			if (mm < newallocsize) throw std::bad_alloc();
			newallocsize = mm;
		}
		char* newptr = (char*)std::realloc( m_ptr, newallocsize);
		if (!newptr) throw std::bad_alloc();
		m_ptr = newptr;
		m_allocsize = newallocsize;
		std::memcpy( m_ptr + m_size, data, datasize);
		m_size += datasize;
	}

	void append( const char* data)
	{
		append( data, std::strlen( data));
	}

	bool consume( size_t pos)
	{
		if (pos == 0 || m_size/4 > pos) return false;
		if (pos > m_size) throw std::logic_error( "illegal resize of buffer");
		std::memmove( m_ptr, m_ptr+pos, m_size - pos);
		m_size -= pos;
		std::size_t newallocsize = m_allocsize;
		if (m_size < BUFFER_INITSIZE/2)
		{
			newallocsize = BUFFER_INITSIZE;
		}
		if (m_allocsize > newallocsize)
		{
			char* newptr = (char*)std::realloc( m_ptr, newallocsize);
			if (newptr)
			{
				m_ptr = newptr;
				m_allocsize = newallocsize;
			}
		}
		return true;
	}

	char* ptr()			{return m_ptr;}
	std::size_t size()		{return m_size;}
	std::size_t allocsize()		{return m_allocsize;}

private:
	Buffer( const Buffer&){}

	char* m_ptr;
	std::size_t m_size;
	size_t m_allocsize;
};

static bool getLine( Line* line, char* baseptr, size_t* pos, size_t size, bool withCR=false)
{
	size_t ii,nn;
	if (*pos == size) return false;
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
		return true;
	}
	return false;
}

#define MAX_LINESPLIT_SIZE 64
struct LineSplit
{
	const char* ptr[ MAX_LINESPLIT_SIZE];
	size_t size;
};

static void getLineSplit_space( LineSplit* split, Line* line, size_t max_nof_elements)
{
	size_t ii;
	if (max_nof_elements > MAX_LINESPLIT_SIZE) throw std::logic_error( "get line split max nof elements parameter out of range");
	for (ii=0; ii < MAX_LINESPLIT_SIZE; ++ii) split->ptr[ ii] = 0;
	split->size = 0;
	for (ii=0; ii < line->size && line->ptr[ii] >= 0 && line->ptr[ii] <= 32; ++ii);
	if (ii == line->size) return;
	split->ptr[ line->size++] = line->ptr + ii;

	for (; ii < line->size; ++ii)
	{
		if (line->ptr[ii] >= 0 && line->ptr[ii] <= 32)
		{
			line->ptr[ii] = 0;
			if (split->size == MAX_LINESPLIT_SIZE) throw std::runtime_error( "too many elements in protocol line");
			if (split->size == max_nof_elements) return;
			for (; ii < line->size && line->ptr[ii] > 0 && line->ptr[ii] < 32; ++ii);
			if (ii == line->size) return;
			split->ptr[ ++split->size] = line->ptr + ii;
		}
	}
}

static bool getContentUnescaped( Buffer* docbuffer, char* baseptr, size_t* pos, size_t size)
{
	Line line;
	while (getLine( &line, baseptr, pos, size))
	{
		if (line.ptr[0] == '.')
		{
			if (line.size == 1) return true;
			docbuffer->append( line.ptr+1, line.size-1);
		}
		else
		{
			docbuffer->append( line.ptr, line.size);
		}
	}
	return false;
}

static void getContentEscaped( Buffer* docbuffer, char* ptr, size_t size)
{
	Line line;
	size_t pos = 0;
	while (getLine( &line, ptr, &pos, size))
	{
		if (line.ptr[0] == '.')
		{
			docbuffer->append( line.ptr, 1);
			docbuffer->append( line.ptr, line.size);
		}
		else
		{
			docbuffer->append( line.ptr, line.size);
		}
	}
	docbuffer->append( "\r\n.\r\n", 5);
}

static int isequal( const char* aa, const char* bb)
{
	size_t ii = 0;
	for (; aa[ii] && (aa[ii]|32) == (bb[ii]|32); ++ii);
	return (aa[ii] == 0 && bb[ii] == 0);
}



typedef Buffer Request;
typedef boost::shared_ptr<Request> RequestR;

struct ProtocolState
{
	enum Id
	{
		INIT,
		AUTH,
		AUTH_WAIT,
		LOAD_UIFORM,
		LOAD_UIFORM_DATA,
		SESSION,
		SESSION_QUIT,
		SESSION_ANSWER,
		SESSION_ANSWER_DATA,
		CLOSED
	};
};

}//anonymous namespace


struct Protocol::Impl
{
public:
	Impl( Protocol::Callback notifier_, void* clientobject_)
		:m_notifier(notifier_)
		,m_clientobject(clientobject_)
		,m_requestqueue_open(true)
		,m_bufferpos(0)
		,m_state(ProtocolState::INIT){}

	bool putRequest( const char* data_, std::size_t datasize_)
	{
		RequestR request( new Request( data_, datasize_));
		boost::lock_guard<boost::mutex> lock(m_requestqueue_mutex);
		if (!m_requestqueue_open) return false;
		m_requestqueue.push_back( request);
		return true;
	}

	void closeRequestQueue()
	{
		boost::lock_guard<boost::mutex> lock(m_requestqueue_mutex);
		m_requestqueue_open = false;
	}

	RequestR fetchRequest()
	{
		RequestR rt;
		boost::lock_guard<boost::mutex> lock(m_requestqueue_mutex);
		if (m_requestqueue.empty()) return rt;
		rt = *m_requestqueue.begin();
		m_requestqueue.erase( m_requestqueue.begin());
		return rt;
	}

	void pushData( const char* data_, std::size_t datasize_)
	{
		boost::lock_guard<boost::mutex> lock(m_buffer_mutex);
		m_buffer.append( data_, datasize_);
	}

	void consumeData( std::size_t datasize_)
	{
		boost::lock_guard<boost::mutex> lock(m_buffer_mutex);
		if (datasize_ + m_bufferpos > m_buffer.size()) throw std::logic_error( "illegal buffer resize (consumeData)");
		if (m_buffer.consume( datasize_ + m_bufferpos))
		{
			m_bufferpos = 0;
		}
		else
		{
			m_bufferpos += datasize_;
		}
	}

	char* dataptr()			{return m_buffer.ptr() + m_bufferpos;}
	std::size_t datasize()		{return m_buffer.size() - m_bufferpos;}

	void sendLine( const char* msg, const char* arg=0)
	{
		char linebuf[ 1024];
		size_t msgsize = strlen( msg);
		size_t argsize = arg?strlen( arg):0;
		if (sizeof(linebuf)-2 < argsize) throw std::bad_alloc();
		if (sizeof(linebuf)-2 < msgsize) throw std::bad_alloc();
		if (sizeof(linebuf)-2 < msgsize+argsize+1) throw std::bad_alloc();
		memcpy( linebuf, msg, msgsize);
		if (arg)
		{
			linebuf[ msgsize] = ' ';
			memcpy( linebuf+msgsize+1, arg, argsize);
			msgsize += argsize + 1;
		}
		linebuf[ msgsize] = '\r';
		linebuf[ msgsize+1] = '\n';
		Protocol::Event event( Protocol::Event::SEND, 0, linebuf, msgsize+2);
		m_notifier( m_clientobject, event);
	}

	void sendContent( char* doc, size_t docsize)
	{
		Buffer docbuffer;
		getContentEscaped( &m_docbuffer, doc, docsize);
		Protocol::Event event( Protocol::Event::SEND, 0, m_docbuffer.ptr(), m_docbuffer.size());
		m_notifier( m_clientobject, event);
	}

	void notifyError( const char* id)
	{
		Protocol::Event event( Protocol::Event::ERROR, 0, id, strlen(id));
		m_notifier( m_clientobject, event);
	}

	void notifyRequestError( const char* id)
	{
		Protocol::Event event( Protocol::Event::REQERR, 0, id, strlen(id));
		m_notifier( m_clientobject, event);
	}

	void notifyState( const char* id)
	{
		Protocol::Event event( Protocol::Event::STATE, id, 0, 0);
		m_notifier( m_clientobject, event);
	}

	void notifyUIForm()
	{
		Protocol::Event event( Protocol::Event::UIFORM, m_statearg.ptr(), m_docbuffer.ptr(), m_docbuffer.size());
		m_notifier( m_clientobject, event);
		m_docbuffer.reset();
		m_statearg.reset();
	}

	void notifyAnswer()
	{
		Protocol::Event event( Protocol::Event::ANSWER, 0, m_docbuffer.ptr(), m_docbuffer.size());
		m_notifier( m_clientobject, event);
		m_docbuffer.reset();
	}

	const char* selectAuthMech( const LineSplit& arg)
	{
		size_t ii;
		for (ii=1; ii<arg.size; ++ii)
		{
			if (isequal( arg.ptr[ii], "NONE")) return "NONE";
		}
		return 0;
	}

	bool isOpen() const
	{
		return m_requestqueue_open;
	}

	ProtocolState::Id state() const		{return m_state;}
	void state( ProtocolState::Id id)	{m_state = id;}


	Protocol::CallResult run()
	{
		Line line;
		LineSplit arg;
		const char* authmech;
		const char* msg;

		struct DataIterator
		{
			Impl* impl;
			std::size_t pos;

			DataIterator( Impl* impl_) :impl(impl_),pos(0){}
			~DataIterator()
			{
				impl->consumeData( pos);
			}
		};
		DataIterator di( this);

		for (;;)
		{
			switch (state())
			{
				case ProtocolState::INIT:
					sendLine( "AUTH");
					state( ProtocolState::AUTH);
					continue;

				case ProtocolState::AUTH:
					if (!getLine( &line, dataptr(), &di.pos, datasize()))
					{
						return Protocol::CALL_DATA;
					}
					getLineSplit_space( &arg, &line, 2);
					if (!arg.size) continue;
					if (isequal( arg.ptr[0], "MECHS"))
					{
						if (arg.size == 1)
						{
							notifyError( "NO AUTH MECHS");
							state( ProtocolState::CLOSED);
							return Protocol::CALL_ERROR;
						}
						getLineSplit_space( &arg, &line, 0);
						authmech = selectAuthMech( arg);
						if (!authmech)
						{
							notifyError( "NO MATCHING AUTH MECH");
							state( ProtocolState::CLOSED);
							return Protocol::CALL_ERROR;
						}
						sendLine( "MECH", authmech);
						state( ProtocolState::AUTH_WAIT);
						continue;
					}
					else if (isequal( arg.ptr[0], "ERR"))
					{
						msg = (arg.size == 1)?"AUTH returned error":arg.ptr[1];
						notifyError( msg);
						state( ProtocolState::INIT);
						continue;
					}
					else
					{
						notifyError( "protocol error in AUTH reply");
						state( ProtocolState::CLOSED);
						return Protocol::CALL_ERROR;
					}

				case ProtocolState::AUTH_WAIT:
					if (!getLine( &line, dataptr(), &di.pos, datasize()))
					{
						return Protocol::CALL_DATA;
					}
					getLineSplit_space( &arg, &line, 2);
					if (!arg.size) continue;
					if (isequal( arg.ptr[0], "OK"))
					{
						notifyState( "authorized");
						sendLine( "INTERFACE");
						state( ProtocolState::LOAD_UIFORM);
						continue;
					}
					else if (isequal( arg.ptr[0], "ERR"))
					{
						msg = (arg.size == 1)?"interface retrieval failed":arg.ptr[1];
						notifyError( msg);
						state( ProtocolState::CLOSED);
						return Protocol::CALL_ERROR;
					}
					else
					{
						notifyError( "protocol error in INTERFACE reply");
						state( ProtocolState::CLOSED);
						return Protocol::CALL_ERROR;
					}

				case ProtocolState::LOAD_UIFORM:
					if (!getLine( &line, dataptr(), &di.pos, datasize()))
					{
						return Protocol::CALL_DATA;
					}
					getLineSplit_space( &arg, &line, 2);
					if (!arg.size) continue;
					if (isequal( arg.ptr[0], "UIFORM"))
					{
						if (arg.size == 1)
						{
							notifyError( "UIFORM missing argument");
							state( ProtocolState::CLOSED);
							return Protocol::CALL_ERROR;
						}
						else
						{
							const char* formid = arg.ptr[1];
							m_statearg.reset( formid, strlen( formid)+1);
							m_docbuffer.reset();
							state( ProtocolState::LOAD_UIFORM_DATA);
							continue;
						}
					}
					else if (isequal( arg.ptr[0], "OK"))
					{
						notifyState( "session");
						state( ProtocolState::SESSION);
						continue;
					}
					else if (isequal( arg.ptr[0], "ERR"))
					{
						msg = (arg.size == 1)?"unspecified error instead of UIFORM":arg.ptr[1];
						notifyError( msg);
						state( ProtocolState::CLOSED);
						return Protocol::CALL_ERROR;
					}
					else
					{
						notifyError( "protocol error in INTERFACE reply (UIFORM,OK expected)");
						state( ProtocolState::CLOSED);
						return Protocol::CALL_ERROR;
					}

				case ProtocolState::LOAD_UIFORM_DATA:
					if (!getContentUnescaped( &m_docbuffer, dataptr(), &di.pos, datasize()))
					{
						return Protocol::CALL_DATA;
					}
					notifyUIForm();
					state( ProtocolState::LOAD_UIFORM);
					continue;

				case ProtocolState::SESSION:
				{
					RequestR request = fetchRequest();
					if (!request.get())
					{
						if (isOpen())
						{
							return Protocol::CALL_DATA;
						}
						sendLine( "QUIT");
						continue;
					}
					else
					{
						sendLine( "REQUEST");
						sendContent( request->ptr(), request->size());
						continue;
					}
				}

				case ProtocolState::SESSION_QUIT:
					if (!getLine( &line, dataptr(), &di.pos, datasize()))
					{
						return Protocol::CALL_DATA;
					}
					getLineSplit_space( &arg, &line, 2);
					if (!arg.size) continue;
					if (isequal( arg.ptr[0], "BYE"))
					{
						notifyState( "closed");
						state( ProtocolState::CLOSED);
						return Protocol::CALL_CLOSED;
					}
					else
					{
						notifyError( "protocol error in QUIT reply (BYE expected)");
						state( ProtocolState::CLOSED);
						return Protocol::CALL_ERROR;
					}

				case ProtocolState::SESSION_ANSWER:
					if (!getLine( &line, dataptr(), &di.pos, datasize()))
					{
						return Protocol::CALL_DATA;
					}
					getLineSplit_space( &arg, &line, 2);
					if (!arg.size) continue;
					if (isequal( arg.ptr[0], "OK"))
					{
						m_docbuffer.reset();
						state( ProtocolState::SESSION_ANSWER_DATA);
						continue;
					}
					else if (isequal( arg.ptr[0], "ERR"))
					{
						msg = (arg.size == 1)?"unspecified error in request":arg.ptr[1];
						notifyRequestError( msg);
						state( ProtocolState::SESSION);
						continue;
					}
					else
					{
						notifyError( "protocol error in ANSWER (OK or ERR expected)");
						state( ProtocolState::CLOSED);
						return Protocol::CALL_ERROR;
					}

				case ProtocolState::SESSION_ANSWER_DATA:
					if (!getContentUnescaped( &m_docbuffer, dataptr(), &di.pos, datasize()))
					{
						return Protocol::CALL_DATA;
					}
					notifyAnswer();
					state( ProtocolState::SESSION);
					continue;

				case ProtocolState::CLOSED:
					return Protocol::CALL_CLOSED;
			}
		}
	}


private:
	Protocol::Callback m_notifier;
	void* m_clientobject;
	std::list<RequestR> m_requestqueue;
	boost::mutex m_requestqueue_mutex;
	bool m_requestqueue_open;
 	Buffer m_buffer;
	boost::mutex m_buffer_mutex;
	std::size_t m_bufferpos;
	ProtocolState::Id m_state;
	Buffer m_statearg;
	Buffer m_docbuffer;
};


bool Protocol::isOpen() const
{
	return m_impl->isOpen();
}

void Protocol::doQuit()
{
	m_impl->closeRequestQueue();
}


Protocol::CallResult Protocol::run()
{
	return m_impl->run();
}

