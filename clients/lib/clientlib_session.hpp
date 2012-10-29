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
#include "clientlib_connect.hpp"
#include "clientlib_protocol.hpp"
#include <string>
#include <boost/thread.hpp>

namespace _Wolframe {
namespace client {

///\defgroup session
///\brief Interface to the client session handling (links protocol and client connection).
// Provides an abstraction of the wolframe client protocol statemachine
// to process complete data items.
///@{

///\brief Client request handler. Handles single request in a session
struct RequestHandler
{
	virtual ~RequestHandler(){}
	virtual void answer( const char* data, std::size_t datasize);
	virtual void error( const char* msg);
};

///\brief Client session handler
class Session
{
public:
	Session( const Connection::Configuration& cfg);
	virtual ~Session();

	bool doRequest( RequestHandler* handler, const char* data, std::size_t datasize);
	void doQuit();

public://methods to be implemented by the session
	virtual void receiveUIForm( const char* data, std::size_t datasize)=0;
	virtual void notifyState( const char* msg)=0;
	virtual void notifyError( const char* msg)=0;

private:
	static void requestCallback( void* this_, const Protocol::Event& event);

	void connectionCallback( const Connection::Event& event);
	static void connectionCallback_void( void* this_, const Connection::Event& event);

	void protocolCallback( const Protocol::Event& event);
	static void protocolCallback_void( void* this_, const Protocol::Event& event);

	static void runSession( Session* session);
	void signal_runSession();

private:
	Connection m_connection;		//< connection handler
	Protocol m_protocol;			//< protocol statemachine
	boost::condition_variable m_run_cond;	//< condition object for signalling the runSession thread
	boost::mutex m_run_mutex;		//< mutex for mutual exclusion on the runSession signal
	bool m_run_signal;			//< boolean variable as runSession signalling flag
	boost::thread m_run_thread;		//< thread that executes runSession
};


}} //namespace
#endif

