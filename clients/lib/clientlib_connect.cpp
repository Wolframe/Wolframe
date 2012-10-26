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
#include "clientlib.h"
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

namespace _Wolframe {
namespace client {

struct ContextDescription
{
	ContextDescription( const char* address_, const char* name_, const char* clientKeyFile_, const char* clientCertFile_, const char* CACertFile_)
		:m_address(address_)
		,m_name(name_)
		,m_clientKeyFile(clientKeyFile_)
		,m_clientCertFile(clientCertFile_)
		,m_CACertFile(CACertFile_){}

	std::string m_address;
	std::string m_name;
	std::string m_clientKeyFile;
	std::string m_clientCertFile;
	std::string m_CACertFile;
};

class ConnectionImpl
{
public:
	ConnectionImpl(	ContextDescription ctx_,
			unsigned short connect_timeout,
			unsigned short read_timeout,
			wolfcli_ConnectionEventCallback notifier_,
			void* clientobject_)
		:m_connect_timeout(connect_timeout)
		,m_read_timeout(read_timeout)
		,m_notifier(notifier_)
		,m_clientobject(clientobject_)
	{
		boost::asio::ip::tcp::resolver::iterator endpoint_iter;
		try {
			boost::asio::ip::tcp::resolver resolver( m_io_service);
			boost::asio::ip::tcp::resolver::query query( ctx_.m_address, ctx_.m_name);
			endpoint_iter = resolver.resolve( query);
		}
		catch (std::runtime_error &e)
		{
			notify( WOLFCLI_COMM_ERROR, e.what());
		}
		if (endpoint_iter != boost::asio::ip::tcp::resolver::iterator())
		{
			notify( WOLFCLI_COMM_ERROR, "unable to resove host");
		}
		notify( WOLFCLI_COMM_STATE, "resolved");

		boost::system::error_code ec;
		boost::asio::ssl::context ctx( m_io_service, boost::asio::ssl::context::sslv23);

		ctx.set_options( boost::asio::ssl::context::default_workarounds
			| boost::asio::ssl::context::no_sslv2 );
		ctx.set_verify_mode( boost::asio::ssl::context::verify_peer
			| boost::asio::ssl::context::verify_fail_if_no_peer_cert );

		ctx.load_verify_file( ctx_.m_CACertFile );
		ctx.use_certificate_file( ctx_.m_clientCertFile, boost::asio::ssl::context::pem, ec);
		if (ec)
		{
			std::ostringstream msg;
			msg << "client certificate illegal or in wrong format (expecting PEM): " << ec.message( ) << " (" << ec.value( ) << ")";
			notify( WOLFCLI_COMM_ERROR, msg.str().c_str());
			return;
		}
		ctx.use_private_key_file( ctx_.m_clientKeyFile, boost::asio::ssl::context::pem, ec);
		if (ec)
		{
			std::ostringstream msg;
			msg << "client key illegal or in wrong format (expecting PEM): " << ec.message( ) << " (" << ec.value( ) << ")";
			notify( WOLFCLI_COMM_ERROR, msg.str().c_str());
			return;
		}
		notify( WOLFCLI_COMM_STATE, "certificate");
		m_socket.reset( new Socket( m_io_service, ctx));
		m_deadline_timer.reset( new boost::asio::deadline_timer( m_io_service));
		m_deadline_timer->expires_from_now( boost::posix_time::seconds( m_connect_timeout));
		m_socket->lowest_layer().async_connect( endpoint_iter->endpoint(),
			boost::bind( &ConnectionImpl::handle_connect, this, _1, endpoint_iter));
		m_deadline_timer->async_wait( boost::bind( &ConnectionImpl::check_deadline, this));
		notify( WOLFCLI_COMM_STATE, "connect");
	}

private:
	void notify( wolfcli_ConnectionEventType type, const char* content, size_t contentsize)
	{
		wolfcli_ConnectionEvent event;
		event.type = type;
		event.content = content;
		event.contentsize = contentsize;
		m_notifier( m_clientobject, &event);
	}

	void notify( wolfcli_ConnectionEventType type, const char* content=0)
	{
		notify( type, content, content?std::strlen(content):0);
	}

	void conn_stop()
	{
		m_socket->lowest_layer().close();
		m_io_service.stop();
		notify( WOLFCLI_COMM_CLOSED, "terminated");
	}

	void conn_error( const char* errmsg, const boost::system::error_code &ec)
	{
		std::ostringstream msg;
		msg << errmsg << ": " << ec.message() << " (" << ec.value() << ")";
		notify( WOLFCLI_COMM_ERROR, ec.message().c_str());
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
			notify( WOLFCLI_COMM_STATE, "connect");
			m_deadline_timer->expires_from_now( boost::posix_time::seconds( m_connect_timeout));
			m_socket->async_handshake( boost::asio::ssl::stream_base::client,
				boost::bind(&ConnectionImpl::handle_handshake, this, boost::asio::placeholders::error));
		}
	}

	void handle_handshake( const boost::system::error_code &ec)
	{
		if (ec)
		{
			conn_error( "error in SSL handshake", ec);
			conn_stop();
		}
		else
		{
			m_deadline_timer->expires_from_now( boost::posix_time::seconds( m_read_timeout));
			notify( WOLFCLI_COMM_STATE, "ready");
			notify( WOLFCLI_COMM_CONNECTED);
		}
	}

	void check_deadline()
	{
		if( m_deadline_timer->expires_at() <= boost::asio::deadline_timer::traits_type::now())
		{
			m_deadline_timer->expires_at( boost::posix_time::pos_infin);
			notify( WOLFCLI_COMM_ERROR, "timeout");
			conn_stop();
		}
		else
		{
			m_deadline_timer->async_wait( boost::bind( &ConnectionImpl::check_deadline, this));
		}
	}

	void write( const char* data, std::size_t size)
	{
		try
		{
			boost::asio::write( *m_socket, boost::asio::buffer( data, size));
		}
		catch (const std::exception& e)
		{
			notify( WOLFCLI_COMM_ERROR, e.what());
		}
	}

	void read()
	{
		m_deadline_timer->expires_from_now( boost::posix_time::seconds( m_read_timeout));
		m_socket->async_read_some(
			boost::asio::buffer( m_buffer, sizeof(m_buffer)),
			boost::bind( &ConnectionImpl::handle_read, this,
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
	}

	void handle_read( const boost::system::error_code &ec, size_t bytes_transferred)
	{
		if (ec)
		{
			if( ec.category() == boost::system::system_category() &&
				ec.value() == boost::system::errc::operation_canceled) {
				// The server sends a timeout, we get a ECANCELED (errno.h)
				notify( WOLFCLI_COMM_CLOSED, "canceled");
			}
			conn_error( "read error", ec);
			conn_stop();
		}
		else
		{
			notify( WOLFCLI_COMM_DATA, m_buffer, bytes_transferred);
		}
	}

private:
	boost::asio::io_service m_io_service;
	typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket> Socket;
	boost::shared_ptr<Socket> m_socket;
	boost::shared_ptr<boost::asio::deadline_timer> m_deadline_timer;
	unsigned short m_connect_timeout;
	unsigned short m_read_timeout;
	wolfcli_ConnectionEventCallback m_notifier;
	void* m_clientobject;
	enum { BufferSize = 1<<12 };
	char m_buffer[ BufferSize];

};

}} //namespace

