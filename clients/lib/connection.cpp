/************************************************************************
///\file client

 Copyright (C) 2011 - 2014 Project Wolframe.
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
///\file connection.cpp
///\brief Implementation of the client connection handling
#include "connection.hpp"
#include <iostream>
#include <sstream>
#include <cstring>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#ifdef WITH_SSL
#include <boost/asio/ssl.hpp>
#endif
#include <boost/shared_ptr.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/enable_shared_from_this.hpp> 

using namespace _Wolframe;
using namespace _Wolframe::client;

#ifdef WITH_SSL
struct TransportLayerSSL
{
	typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket> socket_type;
	typedef void (*handshake_handler_type)( const boost::system::error_code &ec);

	static socket_type* socket( boost::asio::io_service& io_service, const Connection::Configuration& config)
	{
		boost::system::error_code ec;
		boost::asio::ssl::context ctx( io_service, boost::asio::ssl::context::sslv23);

		ctx.set_options( boost::asio::ssl::context::default_workarounds
			| boost::asio::ssl::context::no_sslv2 );

		ctx.load_verify_file( config.m_CA_cert_file);
		ctx.use_certificate_file( config.m_client_cert_file, boost::asio::ssl::context::pem, ec);
		if (ec)
		{
			std::ostringstream msg;
			msg << "client certificate illegal or in wrong format (expecting PEM): " << ec.message( ) << " (" << ec.value( ) << ")";
			throw std::runtime_error( msg.str());
		}
		ctx.use_private_key_file( config.m_client_cert_key, boost::asio::ssl::context::pem, ec);
		if (ec)
		{
			std::ostringstream msg;
			msg << "client key illegal or in wrong format (expecting PEM): " << ec.message( ) << " (" << ec.value( ) << ")";
			throw std::runtime_error( msg.str());
		}

		ctx.set_verify_mode( boost::asio::ssl::context::verify_peer
			| boost::asio::ssl::context::verify_peer );
		return new socket_type( io_service, ctx);
	}
};
#endif

struct TransportLayerPlain
{
	typedef boost::asio::ip::tcp::socket socket_type;
	typedef void (*handshake_handler_type)( const boost::system::error_code &ec);

	static socket_type* socket( boost::asio::io_service& io_service, const Connection::Configuration&)
	{
		return new socket_type( io_service);
	}
};


struct Connection::Impl :public boost::enable_shared_from_this<Connection::Impl>
{
	Impl() :m_state(INIT){}
	virtual ~Impl(){}

	virtual void connect()=0;
	virtual void stop()=0;
	virtual void post_operation()=0;

	Connection::State state() const		{return m_state;}

protected:
	Connection::State m_state;
};


template <class TransportLayer>
class ConnectionImpl :public Connection::Impl
{
public:
	ConnectionImpl( const Connection::Configuration& config,
			ConnectionHandler* connectionHandler_,
			Connection::Callback  notifier_,
			void* clientobject_)
		:m_config(config)
		,m_connectionHandler(connectionHandler_)
		,m_notifier(notifier_)
		,m_clientobject(clientobject_)
	{}

	virtual ~ConnectionImpl(){}

	virtual void connect()
	{
		boost::asio::ip::tcp::resolver::iterator endpoint_iter;
		boost::asio::ip::tcp::resolver resolver( m_io_service);
		boost::asio::ip::tcp::resolver::query query( m_config.m_address, m_config.m_name);
		endpoint_iter = resolver.resolve( query);

		if (endpoint_iter == boost::asio::ip::tcp::resolver::iterator())
		{
			notify( Connection::Event::FAILED, "unable to resove host");
			notify( Connection::Event::STATE, "terminated");
			notify( Connection::Event::TERMINATED);
		}
		else
		{
			notify( Connection::Event::STATE, "resolved");
			m_socket.reset( TransportLayer::socket( m_io_service, m_config));
			m_deadline_timer.reset( new boost::asio::deadline_timer( m_io_service));
			m_deadline_timer->expires_from_now( boost::posix_time::seconds( m_config.m_connect_timeout));
			m_socket->lowest_layer().async_connect( endpoint_iter->endpoint(),
				boost::bind( &ConnectionImpl::handle_connect, this, _1, endpoint_iter));
			m_deadline_timer->async_wait( boost::bind( &ConnectionImpl::check_deadline, this));
			notify( Connection::Event::STATE, "connect");
			m_state = Connection::OPEN;
			m_io_service_thread = boost::thread( boost::bind( &boost::asio::io_service::run, &m_io_service));
		}
	}

	virtual void stop()
	{
		m_io_service.post( boost::bind( &ConnectionImpl::conn_stop, this));
		m_io_service_thread.join();
	}

	virtual void post_operation()
	{
		m_io_service.post( boost::bind( &ConnectionImpl::handle_operation, this));
	}

private:
	void notify( Connection::Event::Type type, const char* content, std::size_t contentsize)
	{
		m_notifier( m_clientobject, Connection::Event( type, content, contentsize));
	}

	void notify( Connection::Event::Type type, const char* content=0)
	{
		m_notifier( m_clientobject, Connection::Event( type, content));
	}

	void conn_stop()
	{
		m_state = Connection::CLOSED;
		m_io_service.stop();
		m_socket->lowest_layer().close();
		notify( Connection::Event::STATE, "terminated");
		notify( Connection::Event::TERMINATED);
	}

	void conn_error( const char* errmsg, const boost::system::error_code &ec)
	{
		std::ostringstream msg;
		msg << errmsg << ": " << ec.message() << " (" << ec.value() << ")";
		notify( Connection::Event::FAILED, msg.str().c_str());
	}

	void handle_connect( const boost::system::error_code &ec, boost::asio::ip::tcp::resolver::iterator)
	{
		if (ec)
		{
			conn_error( "connection error", ec);
			conn_stop();
		}
		else
		{
			notify( Connection::Event::STATE, "connected");
			handshake();
		}
	}

	template <class TL>
	typename boost::enable_if_c<boost::is_same<TL,TransportLayerPlain>::value,void>::type
	handshake_()
	{
		// for TransportLayerPlain we skip the handshake
		boost::system::error_code ec;
		handle_handshake( ec);
	}
#ifdef WITH_SSL
	template <class TL>
	typename boost::enable_if_c<boost::is_same<TL,TransportLayerSSL>::value,void>::type
	handshake_()
	{
		// for TransportLayerSSL we do the handshake
		m_deadline_timer->expires_from_now( boost::posix_time::seconds( m_config.m_connect_timeout));
		m_socket->async_handshake( boost::asio::ssl::stream_base::client, boost::bind(&ConnectionImpl::handle_handshake, this, _1));
		notify( Connection::Event::STATE, "handshake");
	}
#endif
	void handshake()
	{
		handshake_<TransportLayer>();
	}

	void handle_handshake( const boost::system::error_code &ec)
	{
		if (ec)
		{
			conn_error( "error in handshake", ec);
			conn_stop();
		}
		else
		{
			m_deadline_timer->expires_from_now( boost::posix_time::seconds( m_config.m_read_timeout));
			notify( Connection::Event::STATE, "ready");
			m_state = Connection::READY;
			notify( Connection::Event::READY);
			conn_read();
		}
	}

	void check_deadline()
	{
		if (m_deadline_timer->expires_at() <= boost::asio::deadline_timer::traits_type::now())
		{
			m_deadline_timer->expires_at( boost::posix_time::pos_infin);
			notify( Connection::Event::FAILED, "timeout");
			conn_stop();
		}
		else
		{
			m_deadline_timer->async_wait( boost::bind( &ConnectionImpl::check_deadline, this));
		}
	}

	void conn_write( const char* data, std::size_t datasize)
	{
		boost::asio::write( *m_socket, boost::asio::buffer( data, datasize));
	}

	void conn_read()
	{
		m_deadline_timer->expires_from_now( boost::posix_time::seconds( m_config.m_read_timeout));
		m_socket->async_read_some(
			boost::asio::buffer( m_buffer, sizeof(m_buffer)),
			boost::bind( &ConnectionImpl::handle_read, this,
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
	}

	void handle_read( const boost::system::error_code &ec, std::size_t bytes_transferred)
	{
		if (ec)
		{
			if (ec != boost::asio::error::eof)
			{
				if (ec.category() == boost::system::system_category() &&
					ec.value() == boost::system::errc::operation_canceled) {
					// The server sends a timeout, we get a ECANCELED (errno.h)
					notify( Connection::Event::STATE, "canceled");
				}
				else
				{
					conn_error( "read error", ec);
				}
			}
			conn_stop();
		}
		else
		{
			m_connectionHandler->pushData( m_buffer, bytes_transferred);
			post_operation();
		}
	}

	void handle_operation()
	{
		ConnectionHandler::Operation op = m_connectionHandler->nextop();
		switch (op.id)
		{
			case ConnectionHandler::Operation::READ:
				conn_read();
				break;
			case ConnectionHandler::Operation::WRITE:
				conn_write( op.data, op.datasize);
				post_operation();
				break;
			case ConnectionHandler::Operation::IDLE:
				break;
			case ConnectionHandler::Operation::CLOSE:
				conn_stop();
				break;
		}
	}

private:
	boost::asio::io_service m_io_service;
	boost::thread m_io_service_thread;
	typedef typename TransportLayer::socket_type Socket;
	boost::shared_ptr<Socket> m_socket;
	boost::shared_ptr<boost::asio::deadline_timer> m_deadline_timer;
	Connection::Configuration m_config;
	ConnectionHandler* m_connectionHandler;
	Connection::Callback m_notifier;
	void* m_clientobject;
	enum { BufferSize = 1<<12 };
	char m_buffer[ BufferSize];
};


Connection::Connection( const Connection::Configuration& cfg, ConnectionHandler* connhnd, Callback notifier_, void* clientobject_)
	:m_impl(0)
	,m_notifier(notifier_)
	,m_clientobject(clientobject_)
{
	switch (cfg.m_transportLayerType)
	{
		case Configuration::SSL:
#if WITH_SSL
			m_impl = new ConnectionImpl<TransportLayerSSL>( cfg, connhnd, notifier_, clientobject_);
#else
			throw std::runtime_error( "no SSL support built in (WITH_SSL=1)");
#endif
			break;
		case Configuration::Plain:
			m_impl = new ConnectionImpl<TransportLayerPlain>( cfg, connhnd, notifier_, clientobject_);
			break;
		default:
			throw std::runtime_error( "undefined transport layer type (WITH_SSL=1)");
	}
}

Connection::~Connection()
{
	delete m_impl;
}

Connection::State Connection::state() const
{
	return m_impl->state();
}

void Connection::connect()
{
	m_impl->connect();
}

void Connection::stop()
{
	m_impl->stop();
}

void Connection::post_request()
{
	m_impl->post_operation();
}

static const char* eventTypeName( Connection::Event::Type type)
{
	switch (type)
	{
		case Connection::Event::READY: return "READY";
		case Connection::Event::STATE: return "STATE";
		case Connection::Event::FAILED: return "FAILED";
		case Connection::Event::TERMINATED: return "TERMINATED";
	}
	return "(null)";
}

std::string Connection::Event::tostring() const
{
	std::ostringstream msg;
	msg << eventTypeName( m_type) << " '" << std::string( m_content, m_contentsize) << "'";
	return msg.str();
}




