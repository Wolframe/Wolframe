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
///\file protocol.hpp
///\brief C client library protocol interface

#ifndef _WOLFRAME_CLIENTLIB_PROTOCOL_HPP_INCLUDED
#define _WOLFRAME_CLIENTLIB_PROTOCOL_HPP_INCLUDED
#include "connection.hpp"
#include <string>
#include <cstdlib>

namespace _Wolframe {
namespace client {

///\defgroup protocol
///\brief Interface to the client protocol handling.
// Provides an abstraction of the wolframe client protocol statemachine
// to process complete data items.
///@{

///\brief Client protocol handler
class Protocol :public ConnectionHandler
{
public:
	///\struct Event
	///\brief Protocl event structure
	struct Event
	{
		///\struct Type
		///\brief Enumeration of protocol event types
		enum Type
		{
			UIFORM,		//< UI form sent from server to client in the initialization phase
			ANSWER,		//< answer of a requset from the server to the client in a session
			STATE,		//< selected state info for the client
			ATTRIBUTE,	//< attribute value pair ("server GogoServer", "software Wolframe", "version 0.0.1"),etc.)
			ERROR		//< error reported by the server
		};

		Event( Type t, const char* i, const char* c, std::size_t s)
			:m_type(t),m_id(i),m_content(c),m_contentsize(s){}
		Event( const Event& o)
			:m_type(o.m_type),m_id(o.m_id),m_content(o.m_content),m_contentsize(o.m_contentsize){}

		Type type() const			{return m_type;}
		const char* id() const			{return m_id;}
		const char* content() const		{return m_content;}
		std::size_t contentsize() const		{return m_contentsize;}
		std::string tostring() const;

	private:
		Type m_type;			//< type of the protocol event
		const char* m_id;		//< identifier of the protocol event
		const char* m_content;		//< data of the protocol event
		std::size_t m_contentsize;	//< size of the protocol event data in bytes
	};

	typedef void (*Callback)( void* clientobject, const Protocol::Event& event);
	typedef void (*AnswerCallback)( void* requestobject, const Protocol::Event& event);

	///\class Configuration
	///\brief Describes the parameters for the protocol to establish a session
	struct Configuration
	{
		Configuration()
			:uiform_minindex(0){}
		Configuration( const Configuration& o)
			:uiform_minindex(o.uiform_minindex){}

		unsigned int uiform_minindex;
	};

	///\brief Constructor
	///\param[in] notifier_ callback for event notification
	///\param[in] clientobject_ first parameter of notifier_
	Protocol( const Configuration& config_, Callback notifier_, void* clientobject_);
	virtual ~Protocol();

	///\brief Push a request that will be served when the session is established
	///\param[in] notifier_ to return answer to
	///\param[in] requestobject_ handler for request answer
	///\param[in] request_ request type
	///\param[in] data_ request data (decrypted plain data)
	///\param[in] datasize_ size of data in bytes
	bool pushRequest( AnswerCallback notifier_, void* requestobject_, const std::string& request_, const char* data_, std::size_t datasize_);

	///\brief Push a quit request that closes the queue for further requests and closes the session when all pending requests are processed
	void doQuit();

	///\brief Implements ConnectionHandler::pushData( const char*,std::size_t)
	///\param[in] data pointer to data to push (decrypted plain data)
	///\param[in] datasize size of data in bytes
	virtual void pushData( const char* data, std::size_t datasize);

	///\brief Implements ConnectionHandler::nextop()
	///\brief Call the protocol handler state machine
	///\return The next operation to post on the connection
	virtual Operation nextop();

private:
	Protocol( const Protocol&) : ConnectionHandler() {}	//non copyable

	///\class Impl
	///\brief PIMPL for internal protocol handler data structure
	struct Impl;
	Impl* m_impl;
};

///@} end group protocol
}}//namespace
#endif


