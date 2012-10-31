/************************************************************************
///\file client

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
		ctx.set_verify_mode( boost::asio::ssl::context::verify_peer
			| boost::asio::ssl::context::verify_fail_if_no_peer_cert );

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


struct Connection::Impl
{
	Impl() :m_state(INIT){}
	virtual ~Impl(){}
	virtual void write( const char* data, std::size_t size)=0;
	virtual void read()=0;
	virtual void stop()=0;
	virtual bool connect()=0;

	Connection::State state() const		{return m_state;}

protected:
	Connection::State m_state;
};


template <class TransportLayer>
class ConnectionImpl :public Connection::Impl
{
public:
	ConnectionImpl( const Connection::Configuration& config,
			Connection::Callback  notifier_,
			void* clientobject_)
		:m_config(config)
		,m_notifier(notifier_)
		,m_clientobject(clientobject_)
	{}

	virtual bool connect()
	{
		boost::asio::ip::tcp::resolver::iterator endpoint_iter;
		boost::asio::ip::tcp::resolver resolver( m_io_service);
		boost::asio::ip::tcp::resolver::query query( m_config.m_address, m_config.m_name);
		endpoint_iter = resolver.resolve( query);

		if (endpoint_iter == boost::asio::ip::tcp::resolver::iterator())
		{
			notify( Connection::Event::ERROR, "unable to resove host");
			return false;
		}
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
		return true;
	}

	virtual ~ConnectionImpl(){}

	virtual void stop()
	{
		m_io_service.post( boost::bind( &ConnectionImpl::conn_stop, this));
		m_io_service_thread.join();
	}

	class WriteBuffer
	{
	public:
		std::size_t datasize() const	{return m_datasize;}
		const char* data() const	{return m_data;}

		static boost::shared_ptr<WriteBuffer> create( const char* data_, std::size_t datasize_)
		{
			WriteBuffer* rt = (WriteBuffer*) std::malloc( sizeof( WriteBuffer) + datasize_);
			if (!rt) throw std::bad_alloc();
			rt->m_datasize = datasize_;
			std::memcpy( &rt->m_data, data_, datasize_);
			return boost::shared_ptr<WriteBuffer>( rt, std::free);
		}

	private:
		std::size_t m_datasize;
		char m_data[1];
	};

	virtual void write( const char* data, std::size_t datasize)
	{
		m_io_service.post( boost::bind( &ConnectionImpl::conn_write, this, WriteBuffer::create( data, datasize)));
	}

	virtual void read()
	{
		m_io_service.post( boost::bind( &ConnectionImpl::conn_read, this));
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
		notify( Connection::Event::ERROR, ec.message().c_str());
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
			notify( Connection::Event::STATE, "connect");
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
			notify( Connection::Event::STATE, "connected");
			m_state = Connection::READY;
			notify( Connection::Event::READY);
		}
	}

	void check_deadline()
	{
		if (m_deadline_timer->expires_at() <= boost::asio::deadline_timer::traits_type::now())
		{
			m_deadline_timer->expires_at( boost::posix_time::pos_infin);
			notify( Connection::Event::ERROR, "timeout");
			conn_stop();
		}
		else
		{
			m_deadline_timer->async_wait( boost::bind( &ConnectionImpl::check_deadline, this));
		}
	}

	void conn_write( const boost::shared_ptr<WriteBuffer>& wb)
	{
		boost::asio::write( *m_socket, boost::asio::buffer( wb->data(), wb->datasize()));
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
			notify( Connection::Event::DATA, m_buffer, bytes_transferred);
			conn_read();
		}
	}

private:
	boost::asio::io_service m_io_service;
	boost::thread m_io_service_thread;
	typedef typename TransportLayer::socket_type Socket;
	boost::shared_ptr<Socket> m_socket;
	boost::shared_ptr<boost::asio::deadline_timer> m_deadline_timer;
	Connection::Configuration m_config;
	Connection::Callback m_notifier;
	void* m_clientobject;
	enum { BufferSize = 1<<12 };
	char m_buffer[ BufferSize];
};


Connection::Connection( const Configuration& cfg, Callback notifier_, void* clientobject_)
	:m_impl(0)
	,m_notifier(notifier_)
	,m_clientobject(clientobject_)
{
	switch (cfg.m_transportLayerType)
	{
		case Configuration::SSL:
#if !WITH_SSL
			throw std::runtime_error( "no SSL support built in (WITH_SSL=1)");
#endif
			m_impl = new ConnectionImpl<TransportLayerSSL>( cfg, notifier_, clientobject_);
			break;
		case Configuration::Plain:
			m_impl = new ConnectionImpl<TransportLayerPlain>( cfg, notifier_, clientobject_);
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

bool Connection::connect()
{
	return m_impl->connect();
}

void Connection::read()
{
	m_impl->read();
}

void Connection::write( const char* data, std::size_t datasize)
{
	m_impl->write( data, datasize);
}

void Connection::stop()
{
	m_impl->stop();
}

static const char* eventTypeName( Connection::Event::Type type)
{
	switch (type)
	{
		case Connection::Event::DATA: return "DATA";
		case Connection::Event::READY: return "READY";
		case Connection::Event::STATE: return "STATE";
		case Connection::Event::ERROR: return "ERROR";
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




