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
///\file connection.hpp
///\brief Client library connection interface

#ifndef _WOLFRAME_CLIENTLIB_CONNECTION_HPP_INCLUDED
#define _WOLFRAME_CLIENTLIB_CONNECTION_HPP_INCLUDED
#include <string>
#include <cstring>
#include <cstdlib>

namespace _Wolframe {
namespace client {

///\defgroup connection
///\brief Interface to the client connection handling
///@{

///\defgroup Connection handler interface
///\brief The object determining the operations on the client connection
///@{
class ConnectionHandler
{
public:
	virtual ~ConnectionHandler(){}

	///\class Operation
	///\brief Connection operation description
	struct Operation
	{
		enum Id
		{
			READ,	//< the protocol event handler is requesting to read more data. all input (complete lines have been consumed)
			WRITE,	//< the protocol event handler is requesting to write data. all input (complete lines have been consumed)
			IDLE,	//< the protocol event handler is in an established session state and has processed all requests until now.
			CLOSE	//< the client has to terminate becaues it cannot work anymore or the server will close the connection
		};
		static const char* name( Id id)
		{
			const char* ar[] = {"READ","WRITE","IDLE","CLOSE"};
			return ar[ (int)id];
		}

		Operation( Id id_, const char* data_, std::size_t datasize_)
			:id(id_),data(data_),datasize(datasize_){}
		Operation( Id id_)
			:id(id_),data(0),datasize(0){}
		Operation( const Operation& o)
			:id(o.id),data(o.data),datasize(o.datasize){}

		Id id;
		const char* data;
		std::size_t datasize;
	};

	///\brief Push data as answer to a READ operation
	///\param[in] data pointer to data to push
	///\param[in] datasize size of data in bytes
	virtual void pushData( const char* data, std::size_t datasize)=0;

	///\brief Call the protocol handler state machine
	///\return The next operation to post on the connection
	virtual Operation nextop()=0;
};


///\class Connection
///\brief Client connection handler
class Connection
{
public:
	///\class Event
	///\brief Structure for a connection event
	struct Event
	{
		///\class Type
		///\brief Enumeration of connection event types
		enum Type
		{
			STATE,			//< state of connection
			READY,			//< connection ready (signal sent once)
			FAILED,			//< connection error
			TERMINATED		//< connection terminated
		};
		Event( Type t, const char* c, std::size_t s)
			:m_type(t),m_content(c),m_contentsize(s){}
		Event( Type t, const char* c)
			:m_type(t),m_content(c),m_contentsize(c?std::strlen(c):0){}
		Event( const Event& o)
			:m_type(o.m_type),m_content(o.m_content),m_contentsize(o.m_contentsize){}

		Type type() const			{return m_type;}
		const char* content() const		{return m_content;}
		std::size_t contentsize() const		{return m_contentsize;}
		std::string tostring() const;

	private:
		Type m_type;			//< type of the connection event
		const char* m_content;		//< data of the connection event
		std::size_t m_contentsize;	//< size of the event data in bytes
	};

	typedef void (*Callback)( void* clientobject, const Connection::Event& event);
	struct Configuration
	{
		explicit Configuration( std::string address_="127.0.0.1", std::string name_="7661", unsigned short connect_timeout_=30, unsigned short read_timeout_=30)
			:m_address(address_)
			,m_name(name_)
			,m_connect_timeout(connect_timeout_)
			,m_read_timeout(read_timeout_)
			,m_transportLayerType(Plain){}

		Configuration( const Configuration& o)
			:m_address(o.m_address)
			,m_name(o.m_name)
			,m_connect_timeout(o.m_connect_timeout)
			,m_read_timeout(o.m_read_timeout)
			,m_CA_cert_file(o.m_CA_cert_file)
			,m_client_cert_file(o.m_client_cert_file)
			,m_client_cert_key(o.m_client_cert_key)
			,m_transportLayerType(o.m_transportLayerType){}

		void ssl_certificate( const std::string& CA_cert_file_, const std::string& client_cert_file_, const std::string& client_cert_key_)
		{
			m_CA_cert_file = CA_cert_file_;
			m_client_cert_file = client_cert_file_;
			m_client_cert_key = client_cert_key_;
			m_transportLayerType = SSL;
		}

		enum TransportLayerType
		{
			Plain,				//< plain TCP/IP
			SSL				//< SSL
		};

	public:
		std::string m_address;
		std::string m_name;
		unsigned short m_connect_timeout;
		unsigned short m_read_timeout;
		std::string m_CA_cert_file;
		std::string m_client_cert_file;
		std::string m_client_cert_key;
		TransportLayerType m_transportLayerType;
	};

	///\brief Constructor
	///\param[in] cfg the connection configuration
	///\param[in] handler_ object determining the network operations
	///\param[in] notifier_ callback for event notification
	///\param[in] clientobject_ first parameter of notifier_
	Connection( const Configuration& cfg, ConnectionHandler* connhnd, Callback notifier_, void* clientobject_);
	virtual ~Connection();

	///\brief Connect and build up a session
	void connect();
	///\brief Tell the connection handler that a request or a quit has been issued and that the handler should wake up in the case of beeing in idle sleep (forever)
	void post_request();
	///\brief Stop the connection
	void stop();

	///\enum State
	///\brief Connection state
	enum State
	{
		INIT,			//< connection is in initialization
		OPEN,			//< connection context is created
		READY,			//< connection is ready
		CLOSED			//< connection has been closed
	};
	static const char* stateName( State i)
	{
		static const char* ar[] = {"INIT","OPEN","READY","CLOSED"};
		return ar[(int)i];
	}
	State state() const;

public:
	///\class Impl
	///\brief PIMPL for internal connection handler data structure
	struct Impl;

private:
	Connection( const Connection&){}	//non copyable

	Impl* m_impl;
	Callback m_notifier;
	void* m_clientobject;
};

///@} end group connection
}}//namespace
#endif

