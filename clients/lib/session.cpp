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
///\file session.cpp
///\brief Implementation C++ client session
#include "session.hpp"
#include <stdexcept>
#include <iostream>
#include <boost/interprocess/sync/scoped_lock.hpp>

using namespace _Wolframe;
using namespace _Wolframe::client;

struct Session::Impl
{
	boost::mutex m_notify_mutex;		//< mutex for session handler notifiers
	boost::condition_variable m_run_cond;	//< condition object for signalling the runSession thread
	boost::mutex m_run_mutex;		//< mutex for mutual exclusion on the runSession signal
	bool m_run_signal;			//< boolean variable as runSession signalling flag
	boost::thread m_run_thread;		//< thread that executes runSession

	Impl()
		:m_run_signal(false){}
	~Impl(){}
};

Session::Session( const Connection::Configuration& cfg)
	:m_connection(cfg,&connectionCallback_void,this)
	,m_protocol(&protocolCallback_void,this)
	,m_impl(new Impl()){}

Session::~Session()
{
	delete m_impl;
}

void Session::signal_runSession()
{
	{
		boost::lock_guard<boost::mutex> lock(m_impl->m_run_mutex);
		m_impl->m_run_signal = true;
	}
	m_impl->m_run_cond.notify_one();
}

void Session::runSession( Session* session)
{
	try
	{
		if (!session->m_connection.connect())
		{
			throw std::runtime_error( "failed to connect to server");
		}
		while (session->m_connection.state() != Connection::CLOSED)
		{
			{
				boost::unique_lock<boost::mutex> lock( session->m_impl->m_run_mutex);
				while (!session->m_impl->m_run_signal)
				{
					session->m_impl->m_run_cond.wait(lock);
				}
				session->m_impl->m_run_signal = false;
			}
			if (session->m_connection.state() != Connection::CLOSED)
			{
				switch (session->m_protocol.run())
				{
					case Protocol::CALL_DATA:
						session->m_connection.read();
						break;
					case Protocol::CALL_IDLE:
						break;
					case Protocol::CALL_CLOSED:
						return;
				}
			}
		}
	}
	catch (const std::runtime_error& err)
	{
		std::ostringstream msg;
		msg << "runtime error in session thread: " << err.what();
		boost::interprocess::scoped_lock<boost::mutex> lock( session->m_impl->m_notify_mutex);
		session->notifyError( msg.str().c_str());
	}
	catch (const std::bad_alloc& err)
	{
		boost::interprocess::scoped_lock<boost::mutex> lock( session->m_impl->m_notify_mutex);
		session->notifyError( err.what());
	}
	catch (const std::exception& err)
	{
		std::ostringstream msg;
		msg << "exception thrown in session thread: " << err.what();
		boost::interprocess::scoped_lock<boost::mutex> lock( session->m_impl->m_notify_mutex);
		session->notifyError( err.what());
	}
}

void Session::start()
{
	m_impl->m_run_thread = boost::thread( runSession, this);
}

void Session::stop()
{
	m_protocol.doQuit();
	signal_runSession();
	m_impl->m_run_thread.join();
	m_connection.stop();
}

void Session::requestCallback( void* this_, const Protocol::Event& event)
{
	switch (event.type())
	{
		case Protocol::Event::SEND: throw std::logic_error( "request handler cannot handle answer event SEND");
		case Protocol::Event::UIFORM: throw std::logic_error( "request handler cannot handle answer event UIFORM");
		case Protocol::Event::ANSWER: ((RequestHandler*)this_)->answer( event.content(), event.contentsize());
		case Protocol::Event::STATE: throw std::logic_error( "request handler cannot handle answer event STATE");
		case Protocol::Event::ATTRIBUTE: throw std::logic_error( "request handler cannot handle answer event ATTRIBUTE");
		case Protocol::Event::ERROR: ((RequestHandler*)this_)->error( event.id());
	}
}

bool Session::doRequest( RequestHandler* handler, const char* data, std::size_t datasize)
{
	bool rt = m_protocol.pushRequest( requestCallback, handler, data, datasize);
	if (rt)
	{
		signal_runSession();
	}
	return rt;
}

void Session::protocolCallback( const Protocol::Event& event)
{
	switch (event.type())
	{
		case Protocol::Event::SEND:
			m_connection.write( event.content(), event.contentsize());
			break;

		case Protocol::Event::UIFORM:
		{
			boost::interprocess::scoped_lock<boost::mutex> lock( m_impl->m_notify_mutex);
			receiveUIForm( event.id(), event.content(), event.contentsize());
			break;
		}
		case Protocol::Event::ANSWER:
			std::logic_error("answer not passed to request (SessionHandler::protocolCallback)");

		case Protocol::Event::STATE:
		{
			boost::interprocess::scoped_lock<boost::mutex> lock( m_impl->m_notify_mutex);
			notifyState( event.id());
			break;
		}
		case Protocol::Event::ATTRIBUTE:
		{
			boost::interprocess::scoped_lock<boost::mutex> lock( m_impl->m_notify_mutex);
			notifyAttribute( event.id(), event.content());
			break;
		}
		case Protocol::Event::ERROR:
		{
			boost::interprocess::scoped_lock<boost::mutex> lock( m_impl->m_notify_mutex);
			notifyError( event.id());
			break;
		}
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
		case Connection::Event::DATA:
			m_protocol.pushData( event.content(), event.contentsize());
			signal_runSession();
			break;
		case Connection::Event::READY:
			signal_runSession();
			break;
		case Connection::Event::STATE:
		{
			boost::interprocess::scoped_lock<boost::mutex> lock( m_impl->m_notify_mutex);
			notifyState( event.content());
			break;
		}
		case Connection::Event::ERROR:
		{
			boost::interprocess::scoped_lock<boost::mutex> lock( m_impl->m_notify_mutex);
			notifyError( event.content());
			break;
		}
		case Connection::Event::TERMINATED:
			signal_runSession();
			break;
	}
}

void Session::connectionCallback_void( void* this_, const Connection::Event& event)
{
	return ((Session*)this_)->connectionCallback( event);
}


