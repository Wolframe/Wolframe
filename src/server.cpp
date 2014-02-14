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
// server.cpp
//

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include "server.hpp"
#include "acceptor.hpp"
#include "logger-v1.hpp"
#include "system/serverEndpoint.hpp"
#include "system/connectionHandler.hpp"

namespace _Wolframe {
namespace net	{

server::server( const Configuration* conf, _Wolframe::ServerHandler& serverHandler )
	: m_threadPoolSize( conf->threads ),
	  m_globalList( conf->maxConnections )
{
	int i = 0;
	for ( std::list<ServerTCPendpoint>::const_iterator it = conf->address.begin();
	      it != conf->address.end(); it++ )	{
		acceptor* acptr = new acceptor( m_IOservice,
						it->host(), it->port(), it->maxConnections(),
						m_globalList,
						serverHandler );
		m_acceptors.push_back( acptr );
		i++;
	}
	LOG_DEBUG << i << " network acceptor(s) created.";
#ifdef WITH_SSL
	i = 0;
	for ( std::list<ServerSSLendpoint>::const_iterator it = conf->SSLaddress.begin();
	      it != conf->SSLaddress.end(); it++ )	{
		SSLacceptor* acptr = new SSLacceptor( m_IOservice,
						      it->certificate(), it->key(),
						      it->verifyClientCert(),
						      it->CAchain(), it->CAdirectory(),
						      it->host(), it->port(), it->maxConnections(),
						      m_globalList,
						      serverHandler );
		m_SSLacceptors.push_back( acptr );
		i++;
	}
	LOG_DEBUG << i << " network SSL acceptor(s) created.";
#endif // WITH_SSL
}

/// Network server destructor
/// simply delete the acceptors and remove them from the
server::~server()
{
	LOG_TRACE << "Server destructor called";

	std::size_t	i = 0;
	for ( std::list< acceptor* >::iterator it = m_acceptors.begin();
						it != m_acceptors.end(); it++, i++ )
		delete *it;
	LOG_TRACE << i << " acceptor(s) deleted";
#ifdef WITH_SSL
	i = 0;
	for ( std::list< SSLacceptor* >::iterator it = m_SSLacceptors.begin();
						it != m_SSLacceptors.end(); it++, i++ )
		delete *it;
	LOG_TRACE << i << " SSL acceptor(s) deleted";
#endif // WITH_SSL
}


void server::run()
{
	// Create a pool of threads to run all of the io_services.
	std::vector< boost::shared_ptr<boost::thread> >	threads;
	std::size_t					i;

	for ( i = 0; i < m_threadPoolSize; i++ )	{
		boost::shared_ptr<boost::thread> thread( new boost::thread( boost::bind( &boost::asio::io_service::run, &m_IOservice ) ));
		threads.push_back( thread );
	}
	LOG_TRACE << i << " network server thread(s) started";

	// Wait for all threads in the pool to exit.
	for ( i = 0; i < threads.size(); i++ )
		threads[i]->join();

	// Reset io_services.
	m_IOservice.reset();
}

/// Stop the server i.e. notify acceptors to stop
void server::stop()
{
	LOG_DEBUG << "Network server received a shutdown request";

	std::size_t	i = 0;
	for ( std::list< acceptor* >::iterator it = m_acceptors.begin();
	      it != m_acceptors.end(); it++, i++ )
		(*it)->stop();
	LOG_DEBUG << i << " acceptor(s) signaled to stop";
#ifdef WITH_SSL
	i = 0;
	for ( std::list< SSLacceptor* >::iterator it = m_SSLacceptors.begin();
	      it != m_SSLacceptors.end(); it++, i++ )
		(*it)->stop();
	LOG_DEBUG << i << " SSL acceptor(s) signaled to stop";
#endif // WITH_SSL
}


/// Stop io_services the hard way.
void server::abort()
{
	LOG_DEBUG << "Network server received an abort request";
	// Reset io_services.
	m_IOservice.reset();
}

}} // namespace _Wolframe::net
