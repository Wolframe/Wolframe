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
//\file connectionTypeSSL.hpp
//\brief Connection type SSL
#ifndef _Wolframe_CONNECTION_TYPE_SSL_HPP_INCLUDED
#define _Wolframe_CONNECTION_TYPE_SSL_HPP_INCLUDED
#ifdef WITH_SSL
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <list>
#include "connectionBase.hpp"
#include "system/connectionHandler.hpp"
#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace net {

typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket>	ssl_socket;

class ConnectionTypeSSL
	:public ConnectionBase< ssl_socket >
	,public ConnectionCount
{
	//\brief Construct a connection with the given io_service and SSL conetext.
	SSLconnection( boost::asio::io_service& IOservice,
				boost::asio::ssl::context& SSLcontext,
				types::SyncCounter* classCounter_,
				types::SyncCounter* globalCounter_,
				ConnectionHandler *handler );

	~SSLconnection();

	//\brief Get the socket associated with the SSL connection.
	ssl_socket& socket()			{ return m_SSLsocket; }

	//\brief Start the first asynchronous operation for the connection.
	void start();

	//\brief Get a description of this connection for log messages
	std::string logString() const;

private:
	//\brief Handle the SSL handshake
	void handleHandshake( const boost::system::error_code& error );

	//\brief Socket for the SSL connection.
	ssl_socket m_SSLsocket;
};

#endif // WITH_SSL
#endif


