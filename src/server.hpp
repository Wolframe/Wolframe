/************************************************************************

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
//
// server.hpp
//

#ifndef _NETWORK_SERVER_HPP_INCLUDED
#define _NETWORK_SERVER_HPP_INCLUDED

#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>

#include <list>

#include "system/serverEndpoint.hpp"
#include "acceptorSocket.hpp"
#include "acceptorSSL.hpp"
#include "types/syncCounter.hpp"
#include "system/connectionHandler.hpp"	// for server handler
#include "config/configurationBase.hpp"
#include "standardConfigs.hpp"		// for server configuration

namespace _Wolframe {
namespace net	{

class server: private boost::noncopyable
{
	/// public interface
public:
	/// Construct the server
	explicit server( const Configuration* conf, _Wolframe::ServerHandler& serverHandler );

	/// Destruct the server
	~server();

	/// Run the server's io_service loop.
	void run();

	/// Stop the server. Outstanding asynchronous operations will be completed.
	void stop();

	/// Abort the server. Outstanding asynchronous operations will be aborted.
	void abort();

private:
	/// The number of threads that will call io_service::run().
	std::size_t		m_threadPoolSize;

	/// The io_service(s) used to perform asynchronous operations.
	boost::asio::io_service	m_IOservice;

	/// The list(s) of connection acceptors.
	std::list< AcceptorSocket* > m_acceptors;
#ifdef WITH_SSL
	std::list< AcceptorSSL* > m_SSLacceptors;
#endif // WITH_SSL
	types::SyncCounter m_globalConnectionCounter;
};

}} // namespace _Wolframe::net

#endif // _NETWORK_SERVER_HPP_INCLUDED
