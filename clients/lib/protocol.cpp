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
///\file protocol.cpp
///\brief Implementation of the client protocol handling
#include "protocol.hpp"
#include "connection.hpp"
#include <list>
#include <stdexcept>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

using namespace _Wolframe;
using namespace _Wolframe::client;

namespace {
struct Line
{
	const char* ptr;
	size_t size;
	size_t size_with_eoln;
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

	void append( const std::string& data)
	{
		append( data.c_str(), data.size());
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

static bool getLine( Line* line, const char* baseptr, size_t* pos, size_t size, bool withCR=false)
{
	size_t ii,nn;
	if (*pos == size) return false;
	line->ptr = baseptr + *pos;
	nn = size - *pos;
	for (ii = 0; ii < nn && line->ptr[ii] != '\n'; ++ii);
	if (ii < nn)
	{
		line->size_with_eoln = ii+1;
		if (!withCR && ii > 0 && line->ptr[ii-1] == '\r')
		{
			line->size = ii-1;
		}
		else
		{
			line->size = ii;
		}
		*pos += ii+1;
		return true;
	}
	return false;
}

#define MAX_LINESPLIT_SIZE 64
#define MAX_LINEBUF_SIZE 1024
struct LineSplit
{
	char* ptr[ MAX_LINESPLIT_SIZE];
	std::size_t size;
	char line[ MAX_LINEBUF_SIZE];
	std::size_t linesize;
};

static void getLineSplit_space( LineSplit& split, const Line& line, size_t max_nof_elements)
{
	if (line.size >= MAX_LINEBUF_SIZE) throw std::runtime_error( "protocol: line too big");
	std::memcpy( split.line, line.ptr, line.size);
	split.line[ split.linesize = line.size] = 0;
	std::size_t ii;
	if (max_nof_elements > MAX_LINESPLIT_SIZE) throw std::logic_error( "get line split max nof elements parameter out of range");
	for (ii=0; ii < MAX_LINESPLIT_SIZE; ++ii) split.ptr[ ii] = 0;
	split.size = 0;
	for (ii=0; ii < split.linesize && (unsigned char)split.line[ii] <= 32; ++ii);
	if (ii == split.linesize) return;
	split.ptr[ split.size++] = split.line + ii;
	if (max_nof_elements == 1) return;
	for (; ii < split.linesize; ++ii)
	{
		if ((unsigned char)split.line[ii] <= 32)
		{
			if (split.size == max_nof_elements) return;
			if (split.size == MAX_LINESPLIT_SIZE) throw std::runtime_error( "too many elements in protocol line");
			split.line[ii] = 0;
			for (; ii < split.linesize && (unsigned char)split.line[ii] <= 32; ++ii);
			if (ii == split.linesize) return;
			split.ptr[ split.size++] = split.line + ii;
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
			docbuffer->append( line.ptr+1, line.size_with_eoln-1);
		}
		else
		{
			docbuffer->append( line.ptr, line.size_with_eoln);
		}
	}
	return false;
}

static void getContentEscaped( Buffer* docbuffer, const char* ptr, size_t size)
{
	Line line;
	size_t pos = 0;
	while (getLine( &line, ptr, &pos, size))
	{
		if (line.ptr[0] == '.')
		{
			docbuffer->append( line.ptr, 1);
			docbuffer->append( line.ptr, line.size_with_eoln);
		}
		else
		{
			docbuffer->append( line.ptr, line.size_with_eoln);
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


class Request
{
public:
	Request( Protocol::AnswerCallback notifier_, void* requestobject_, const std::string& type_, const char* data_, std::size_t datasize_)
		:m_notifier(notifier_)
		,m_requestobject(requestobject_)
		,m_type(type_)
		,m_data((char*)std::malloc(datasize_))
		,m_datasize(datasize_)
	{
		if (!m_data) throw std::bad_alloc();
		std::memcpy( m_data, data_, datasize_);
	}

	Request( const Request& o)
		:m_notifier(o.m_notifier)
		,m_requestobject(o.m_requestobject)
		,m_type(o.m_type)
		,m_data((char*)std::malloc(o.m_datasize))
		,m_datasize(o.m_datasize)
	{
		if (!m_data) throw std::bad_alloc();
		std::memcpy( m_data, o.m_data, m_datasize);
	}

	~Request()
	{
		if (m_data) std::free( m_data);
	}

	const char* type() const	{return m_type.empty()?0:m_type.c_str();}
	const char* data() const	{return m_data;}
	std::size_t datasize() const	{return m_datasize;}

	void answer( const Protocol::Event& event)
	{
		m_notifier( m_requestobject, event);
	}

private:
	Protocol::AnswerCallback m_notifier;
	void* m_requestobject;
	std::string m_type;
	char* m_data;
	std::size_t m_datasize;
};

typedef boost::shared_ptr<Request> RequestR;

struct ProtocolState
{
	enum Id
	{
		INIT,
		BANNER,
		START,
		AUTH,
		AUTH_WAIT,
		SESSION,
		SESSION_QUIT,
		SESSION_REQUEST,
		SESSION_ANSWER,
		SESSION_ANSWER_DATA,
		SESSION_ANSWER_RESULT,
		CLOSED
	};
	static const char* name( Id id)
	{
		static const char* ar[] =
		{
			"INIT",
			"BANNER",
			"START",
			"AUTH",
			"AUTH_WAIT",
			"SESSION",
			"SESSION_QUIT",
			"SESSION_REQUEST",
			"SESSION_ANSWER",
			"SESSION_ANSWER_DATA",
			"SESSION_ANSWER_RESULT",
			"CLOSED"
		};
		return ar[(int)id];
	}
};

}//anonymous namespace


struct Protocol::Impl
{
public:
	Impl( Protocol::Configuration config_, Protocol::Callback notifier_, void* clientobject_)
		:m_config(config_)
		,m_notifier(notifier_)
		,m_clientobject(clientobject_)
		,m_requestqueue_open(true)
		,m_bufferpos(0)
		,m_state(ProtocolState::INIT){}

	bool putRequest( Protocol::AnswerCallback notifier_, void* requestobject_, const std::string& request_, const char* data_, std::size_t datasize_)
	{
		RequestR request( new Request( notifier_, requestobject_, request_, data_, datasize_));
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
		m_buffer.append( data_, datasize_);
	}

	void consumeData( std::size_t datasize_)
	{
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

	void notifyError( const char* id)
	{
		Protocol::Event event( Protocol::Event::ERROR, id, 0, 0);
		m_notifier( m_clientobject, event);
	}

	void notifyState( const char* id)
	{
		Protocol::Event event( Protocol::Event::STATE, id, 0, 0);
		m_notifier( m_clientobject, event);
	}

	void notifyAttribute( const char* name, const char* value)
	{
		Protocol::Event event( Protocol::Event::ATTRIBUTE, name, value, std::strlen(value));
		m_notifier( m_clientobject, event);
	}

	void notifyUIForm()
	{
		Protocol::Event event( Protocol::Event::UIFORM, m_statearg.ptr(), m_docbuffer.ptr(), m_docbuffer.size());
		m_notifier( m_clientobject, event);
		m_docbuffer.reset();
		m_statearg.reset();
	}

	void notifyAnswerOk()
	{
		Protocol::Event event( Protocol::Event::ANSWER, m_request->type(), m_docbuffer.ptr(), m_docbuffer.size());
		m_request->answer( event);
		m_request.reset();
		m_docbuffer.reset();
	}

	void notifyAnswerError( const char* id)
	{
		Protocol::Event event( Protocol::Event::ERROR, id, 0, 0);
		m_request->answer( event);
		m_request.reset();
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

	ConnectionHandler::Operation nextop()
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

		struct OP_READ :public ConnectionHandler::Operation
		{OP_READ() :ConnectionHandler::Operation( ConnectionHandler::Operation::READ){}};
		struct OP_WRITE :public ConnectionHandler::Operation
		{OP_WRITE( const char* data_, std::size_t datasize_) :ConnectionHandler::Operation( ConnectionHandler::Operation::WRITE, data_, datasize_){}
		 OP_WRITE( const char* data_) :ConnectionHandler::Operation( ConnectionHandler::Operation::WRITE, data_, std::strlen(data_)){}};
		struct OP_IDLE :public ConnectionHandler::Operation
		{OP_IDLE() :ConnectionHandler::Operation( ConnectionHandler::Operation::IDLE){}};
		struct OP_CLOSE :public ConnectionHandler::Operation
		{OP_CLOSE() :ConnectionHandler::Operation( ConnectionHandler::Operation::CLOSE){}};

		for (;;)
		{
		switch (state())
		{
			case ProtocolState::INIT:
				state( ProtocolState::BANNER);
				/* no break here !*/

			case ProtocolState::BANNER:
				if (!getLine( &line, dataptr(), &di.pos, datasize()))
				{
					return OP_READ();
				}
				getLineSplit_space( arg, line, 1);
				if (!arg.size) continue;
				notifyAttribute( "banner", arg.ptr[0]);
				state( ProtocolState::START);
				/* no break here !*/

			case ProtocolState::START:
				if (!getLine( &line, dataptr(), &di.pos, datasize()))
				{
					return OP_READ();
				}
				getLineSplit_space( arg, line, 2);
				if (!arg.size) continue;
				if (isequal( arg.ptr[0], "OK"))
				{
					state( ProtocolState::AUTH);
					return OP_WRITE( "AUTH\r\n");
				}
				else if (isequal( arg.ptr[0], "ERR"))
				{
					msg = (arg.size == 1)?"rejected connection":arg.ptr[1];
					notifyError( msg);
					state( ProtocolState::CLOSED);
					return OP_CLOSE();
				}
				else
				{
					notifyError( "protocol error in server connection reply");
					state( ProtocolState::CLOSED);
					return OP_CLOSE();
				}

			case ProtocolState::AUTH:
				if (!getLine( &line, dataptr(), &di.pos, datasize()))
				{
					return OP_READ();
				}
				getLineSplit_space( arg, line, 2);
				if (!arg.size) continue;
				if (isequal( arg.ptr[0], "MECHS"))
				{
					if (arg.size == 1)
					{
						notifyError( "NO AUTH MECHS");
						state( ProtocolState::CLOSED);
						return OP_CLOSE();
					}
					getLineSplit_space( arg, line, 0);
					authmech = selectAuthMech( arg);
					if (!authmech)
					{
						notifyError( "NO MATCHING AUTH MECH");
						state( ProtocolState::CLOSED);
						return OP_CLOSE();
					}
					state( ProtocolState::AUTH_WAIT);
					m_statearg.reset();
					m_statearg.append( "MECH ");
					m_statearg.append( authmech);
					m_statearg.append( "\r\n");
					return OP_WRITE( m_statearg.ptr(), m_statearg.size());
				}
				else if (isequal( arg.ptr[0], "ERR"))
				{
					msg = (arg.size == 1)?"AUTH returned error":arg.ptr[1];
					notifyError( msg);
					state( ProtocolState::SESSION_QUIT);
					return OP_WRITE( "QUIT\r\n");
				}
				else
				{
					notifyError( "protocol error in AUTH reply");
					state( ProtocolState::CLOSED);
					return OP_CLOSE();
				}

			case ProtocolState::AUTH_WAIT:
				if (!getLine( &line, dataptr(), &di.pos, datasize()))
				{
					return OP_READ();
				}
				getLineSplit_space( arg, line, 2);
				if (!arg.size) continue;
				if (isequal( arg.ptr[0], "OK"))
				{
					notifyState( "authorized");
					state( ProtocolState::SESSION);
					continue;
				}
				else if (isequal( arg.ptr[0], "ERR"))
				{
					msg = (arg.size == 1)?"authorization (MECH command) failed":arg.ptr[1];
					notifyError( msg);
					state( ProtocolState::CLOSED);
					return OP_CLOSE();
				}
				else
				{
					notifyError( "protocol error in authorization (MECH command reply)");
					state( ProtocolState::CLOSED);
					return OP_CLOSE();
				}

			case ProtocolState::SESSION:
			{
				m_request = fetchRequest();
				if (!m_request.get())
				{
					if (isOpen()) return OP_IDLE();
					state( ProtocolState::SESSION_QUIT);
					return OP_WRITE( "QUIT\r\n");
				}
				else
				{
					m_docbuffer.reset();
					state( ProtocolState::SESSION_REQUEST);
					if (m_request->type())
					{
						m_statearg.reset();
						m_statearg.append( "REQUEST ");
						m_statearg.append( m_request->type());
						m_statearg.append( "\r\n");
						return OP_WRITE( m_statearg.ptr(), m_statearg.size());
					}
					else
					{
						return OP_WRITE( "REQUEST\r\n");
					}
				}
			}

			case ProtocolState::SESSION_QUIT:
				if (!getLine( &line, dataptr(), &di.pos, datasize()))
				{
					return OP_READ();
				}
				getLineSplit_space( arg, line, 2);
				if (!arg.size) continue;
				if (isequal( arg.ptr[0], "BYE"))
				{
					notifyState( "closed");
					state( ProtocolState::CLOSED);
					return OP_CLOSE();
				}
				else
				{
					notifyError( "protocol error in QUIT reply (BYE expected)");
					state( ProtocolState::CLOSED);
					return OP_CLOSE();
				}

			case ProtocolState::SESSION_REQUEST:
				m_statearg.reset();
				getContentEscaped( &m_docbuffer, m_request->data(), m_request->datasize());
				state( ProtocolState::SESSION_ANSWER);
				return OP_WRITE( m_docbuffer.ptr(), m_docbuffer.size());

			case ProtocolState::SESSION_ANSWER:
				if (!getLine( &line, dataptr(), &di.pos, datasize()))
				{
					return OP_READ();
				}
				getLineSplit_space( arg, line, 2);
				if (!arg.size) continue;
				if (isequal( arg.ptr[0], "ANSWER"))
				{
					m_docbuffer.reset();
					state( ProtocolState::SESSION_ANSWER_DATA);
					continue;
				}
				else if (isequal( arg.ptr[0], "OK"))
				{
					m_docbuffer.reset();
					m_request.reset();
					state( ProtocolState::SESSION);
					continue;
				}
				else if (isequal( arg.ptr[0], "ERR"))
				{
					msg = (arg.size == 1)?"unspecified error in request":arg.ptr[1];
					notifyAnswerError( msg);
					state( ProtocolState::SESSION);
					continue;
				}
				else
				{
					notifyError( "protocol error in REQUEST (ANSWER, OK or ERR expected)");
					notifyAnswerError( "protocol error");
					state( ProtocolState::CLOSED);
					return OP_CLOSE();
				}

			case ProtocolState::SESSION_ANSWER_DATA:
				if (!getContentUnescaped( &m_docbuffer, dataptr(), &di.pos, datasize()))
				{
					return OP_READ();
				}
				state( ProtocolState::SESSION_ANSWER_RESULT);
				continue;

			case ProtocolState::SESSION_ANSWER_RESULT:
				if (!getLine( &line, dataptr(), &di.pos, datasize()))
				{
					return OP_READ();
				}
				getLineSplit_space( arg, line, 2);
				if (!arg.size) continue;
				if (isequal( arg.ptr[0], "OK"))
				{
					notifyAnswerOk();
					state( ProtocolState::SESSION);
					continue;
				}
				else if (isequal( arg.ptr[0], "ERR"))
				{
					msg = (arg.size == 1)?"unspecified error in request":arg.ptr[1];
					notifyAnswerError( msg);
					state( ProtocolState::SESSION);
					continue;
				}
				else
				{
					notifyError( "protocol error in answer after data (OK or ERR expected)");
					notifyAnswerError( "protocol error");
					state( ProtocolState::CLOSED);
					return OP_CLOSE();
				}

			case ProtocolState::CLOSED:
				return OP_CLOSE();
		}
		}
	}


private:
	Protocol::Configuration m_config;
	Protocol::Callback m_notifier;
	void* m_clientobject;
	std::list<RequestR> m_requestqueue;
	boost::mutex m_requestqueue_mutex;
	bool m_requestqueue_open;
	RequestR m_request;
 	Buffer m_buffer;
	std::size_t m_bufferpos;
	ProtocolState::Id m_state;
	Buffer m_statearg;
	Buffer m_docbuffer;
};

Protocol::Protocol( const Configuration& config_, Callback notifier_, void* clientobject_)
	:m_impl( new Protocol::Impl( config_, notifier_, clientobject_))
{}

Protocol::~Protocol()
{
	delete m_impl;
}

bool Protocol::pushRequest( AnswerCallback notifier_, void* requestobject_, const std::string& request_, const char* data_, std::size_t datasize_)
{
	return m_impl->putRequest( notifier_, requestobject_, request_, data_, datasize_);
}

void Protocol::doQuit()
{
	m_impl->closeRequestQueue();
}

void Protocol::pushData( const char* data_, std::size_t datasize_)
{
	m_impl->pushData( data_, datasize_);
}

ConnectionHandler::Operation Protocol::nextop()
{
	return m_impl->nextop();
}

static const char* eventTypeName( Protocol::Event::Type type)
{
	switch (type)
	{
		case Protocol::Event::UIFORM: return "UIFORM";
		case Protocol::Event::ANSWER: return "ANSWER";
		case Protocol::Event::STATE: return "STATE";
		case Protocol::Event::ATTRIBUTE: return "ATTRIBUTE";
		case Protocol::Event::ERROR: return "ERROR";
	}
	return "(null)";
}

std::string Protocol::Event::tostring() const
{
	std::ostringstream msg;
	msg << eventTypeName( m_type) << " " << (m_id?m_id:"") << " '" << std::string( m_content, m_contentsize) << "'";
	return msg.str();
}


