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
//\file connectionTypeSocket.cpp
//\brief Implementation of the connection type Socket
#include "connectionTypeSocket.hpp"
#include "logger-v1.hpp"

using namespace _Wolframe;
using namespace _Wolframe::net;

static const char* REFUSE_MSG = "Server is busy. Please try again later.\n";

ConnectionTypeSocket::ConnectionTypeSocket(
			boost::asio::io_service& IOservice,
			ConnectionTypeList* connList,
			ConnectionHandler *handler)
	:ConnectionBase< boost::asio::ip::tcp::socket >( IOservice, handler )
	,ConnectionType(connList)
	,m_socket( IOservice )
{
	LOG_TRACE << "New connection created";
}

ConnectionTypeSocket::~ConnectionTypeSocket()
{
	if ( m_ID.empty() )
		LOG_TRACE << "Connection (unconnected) destroyed";
	else
		LOG_TRACE << "Connection to " << m_ID <<" destroyed";
}

void ConnectionTypeSocket::start()
{
	identifier( std::string( socket().remote_endpoint().address().to_string())
		    + ":" + boost::lexical_cast<std::string>( socket().remote_endpoint().port() ));
	LOG_TRACE << "Starting connection to " << identifier();

	// if the maximum number of connections has been reached refuse the connection
	if ( !registerConnection() )	{
		LOG_DEBUG << "Refusing connection from " << identifier() << ". Too many connections.";
		boost::system::error_code ignored_ec;
		socket().write_some( boost::asio::buffer( REFUSE_MSG, strlen( REFUSE_MSG ) ));
		socket().lowest_layer().shutdown( boost::asio::ip::tcp::socket::shutdown_both, ignored_ec );
	}
	else	{
		m_connHandler->setPeer( RemoteTCPendpoint( socket().remote_endpoint().address().to_string(),
							   socket().remote_endpoint().port()));
		nextOperation();
	}
}


std::string ConnectionTypeSocket::logString() const
{
	std::string str;

	boost::system::error_code ec;
	boost::asio::ip::tcp::endpoint ep = m_socket.remote_endpoint( ec );
	if ( !ec )
	{
		str = ep.address().to_string()  + ":" + boost::lexical_cast< std::string >( ep.port() );
		str += "->";
	}
	ep = m_socket.local_endpoint( ec );
	if ( !ec )
		str += ep.address().to_string() + ":" + boost::lexical_cast< std::string >( ep.port() );
	return str;
}



