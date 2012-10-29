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
///\file clientlib_connect.hpp
///\brief C client library interface

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

///\class Connection
///\brief Connection handler object
class Connection
{
public:
	///\struct Event
	///\brief Structure for a connection event
	struct Event
	{
		///\struct Type
		///\brief Enumeration of connection event types
		enum Type
		{
			DATA=	1,		//< connection has data to read
			STATE=	2,		//< state of connection
			ERROR=	3		//< connection error
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

	private:
		Type m_type;			//< type of the connection event
		const char* m_content;		//< data of the connection event
		std::size_t m_contentsize;	//< size of the event data in bytes
	};

	typedef void (*Callback)( void* clientobject, const Connection::Event& event);
	struct Configuration
	{
		Configuration( std::string address_, std::string name_, unsigned short connect_timeout_=30, unsigned short read_timeout_=30)
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
	///\param[in] notifier_ callback for event notification
	///\param[in] clientobject_ first parameter of notifier_
	Connection( const Configuration& cfg, Callback notifier_, void* clientobject_);
	virtual ~Connection();

	void read();
	void write( const char* data, std::size_t datasize);

	///\enum State
	///\brief Connection state
	enum State
	{
		INIT,			//< connection is in initialization
		OPEN,			//< connection context is created
		READY,			//< connection is ready
		CLOSED			//< connection has been closed
	};
	State state() const;

private:
	Connection( const Connection&){}	//non copyable

	///\class Impl
	///\brief PIMPL for internal connection handler data structure
	struct Impl;
	Impl* m_impl;
	Callback m_notifier;
	void* m_clientobject;
};

///@} end group connection
}}//namespace
#endif

