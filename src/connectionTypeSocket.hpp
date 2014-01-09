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
//\file connectionTypeSocket.hpp
//\brief Connection type Socket
#ifndef _Wolframe_CONNECTION_TYPE_SOCKET_HPP_INCLUDED
#define _Wolframe_CONNECTION_TYPE_SOCKET_HPP_INCLUDED
#include <boost/asio.hpp>
#include <list>
#include "connectionBase.hpp"
#include "connectionType.hpp"
#include "types/syncCounter.hpp"
#include "system/connectionHandler.hpp"
#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>

namespace _Wolframe {
namespace net {

//\brief Represents a single connection from a client.
class ConnectionTypeSocket
	:public ConnectionBase< boost::asio::ip::tcp::socket >
	,public ConnectionType
{
public:
	//\brief Construct a connection with the given io_service.
	ConnectionTypeSocket( boost::asio::io_service& IOservice,
				ConnectionTypeList* connList,
				ConnectionHandler *handler);
	virtual ~ConnectionTypeSocket();

	//\brief Get the socket associated with the connection.
	boost::asio::ip::tcp::socket& socket()	{ return m_socket; }

	//\brief Start the first asynchronous operation for the connection.
	virtual void start();

	//\brief Get a description of this connection for log messages
	virtual std::string logString() const;

	virtual void signalTerminate()
	{
		signal();
	}

private:
	boost::asio::ip::tcp::socket m_socket;		//< socket for the connection
};

typedef boost::shared_ptr<ConnectionTypeSocket> ConnectionTypeSocketR;

}}//namespace
#endif

