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
//
// server.hpp
//

#ifndef _NETWORK_SERVER_HPP_INCLUDED
#define _NETWORK_SERVER_HPP_INCLUDED

#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>

#include <list>

#include "serverEndpoint.hpp"
#include "acceptor.hpp"
#include "connectionHandler.hpp"	// for server handler
#include "config/configurationBase.hpp"
#include "standardConfigs.hpp"		// for server configuration

namespace _Wolframe {
namespace net	{

class io_service_pool
	:private boost::noncopyable
{
public:
	explicit io_service_pool( std::size_t pool_size)
		:m_io_service_cnt(0)
	{
		if (pool_size == 0) throw std::logic_error( "illegal size of io service pool");
		for (std::size_t ii=0; ii<pool_size; ++ii)
		{
			m_io_services.push_back( io_service_ptr( new boost::asio::io_service()));
		}
	}

	boost::asio::io_service& get()
	{
		return *m_io_services[ m_io_service_cnt++ % m_io_services.size()];
	}
private:
	typedef boost::shared_ptr<boost::asio::io_service> io_service_ptr;
	//[PF:NOTE] No mutex here because it is only important that every thread picks one io service
	//		and not if two thread share an io service.
	std::vector<io_service_ptr> m_io_services;	//< array of io services
	std::size_t m_io_service_cnt;			//< next element to pick with get
};


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

	///[PF:NOTE] Changed IO Service to a pool of objects with the intention that every thread
	//		gets only one IO service to fetch events from.
	/// The io_service(s) used to perform asynchronous operations.
	io_service_pool		m_IOservicePool;

	/// The list(s) of connection acceptors.
	std::list<acceptor*>	m_acceptors;
#ifdef WITH_SSL
	std::list<SSLacceptor*>	m_SSLacceptors;
#endif // WITH_SSL
	GlobalConnectionList	m_globalList;
};

}} // namespace _Wolframe::net

#endif // _NETWORK_SERVER_HPP_INCLUDED
