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
//\file acceptorSocket.cpp
//\brief Implementation of the acceptor for ordinary socket connections
#include "acceptorSocket.hpp"
#include "system/errorCode.hpp"
#include "logger-v1.hpp"

using namespace _Wolframe;
using namespace _Wolframe::net;

AcceptorSocket::AcceptorSocket( boost::asio::io_service* IOservice,
				const std::string& host, unsigned short port, unsigned maxConnections,
				types::SyncCounter* globalCounter_,
				ServerHandler* srvHandler ) :
	m_IOservice( IOservice ),
	m_strand( *IOservice ),
	m_acceptor( *IOservice ),
	m_connectionList( maxConnections, globalCounter_),
	m_srvHandler( srvHandler )
{
	// Open the acceptor(s) with the option to reuse the address (i.e. SO_REUSEADDR).
	boost::asio::ip::tcp::endpoint endpoint;
	try	{
		boost::asio::ip::tcp::resolver resolver( *m_IOservice );
		boost::asio::ip::tcp::resolver::query query( host, "7661" );
		endpoint = *resolver.resolve( query );
	}
	catch ( std::exception& e )	{
		LOG_FATAL << "Unable to resolve host '" << host << "' (" << e.what() << ")";
		exit( ErrorCode::FAILURE );
	}
	endpoint.port( port );

	ConnectionHandler *handler = m_srvHandler->newConnection( LocalTCPendpoint( host, port ));
	m_newConnection = ConnectionTypeSocketR( new ConnectionTypeSocket( *m_IOservice, &m_connectionList, handler ));

	try	{
		m_acceptor.open( endpoint.protocol() );
		m_acceptor.set_option( boost::asio::ip::tcp::acceptor::reuse_address( true ));
		m_acceptor.bind( endpoint );
		m_acceptor.listen();
	}
	catch ( std::exception& e )	{
		LOG_FATAL << "Unable to listen on " << endpoint << " (" << e.what() << ")";
		exit( ErrorCode::FAILURE );
	}

	m_identifier = m_acceptor.local_endpoint().address().to_string()
			+ ":" + boost::lexical_cast<std::string>( m_acceptor.local_endpoint().port() );

	m_acceptor.async_accept( m_newConnection->socket(),
				 m_strand.wrap( boost::bind( &AcceptorSocket::handleAccept,
							     this,
							     boost::asio::placeholders::error )));
	LOG_INFO << "Accepting connections on " << m_identifier;
}

AcceptorSocket::~AcceptorSocket()
{
	LOG_TRACE << "Acceptor destructor called for " << m_identifier;
}

void AcceptorSocket::handleAccept( const boost::system::error_code& e )
{
	if ( !e )	{
		LOG_TRACE << "Received new connection on " << m_identifier;
		m_newConnection->start();

		ConnectionHandler *handler = m_srvHandler->newConnection( LocalTCPendpoint( m_acceptor.local_endpoint().address().to_string(),
											   m_acceptor.local_endpoint().port() ));
		m_newConnection.reset( new ConnectionTypeSocket( *m_IOservice, &m_connectionList, handler ));
		m_acceptor.async_accept( m_newConnection->socket(),
					 m_strand.wrap( boost::bind( &AcceptorSocket::handleAccept,
								     this,
								     boost::asio::placeholders::error )));
		LOG_DATA << "Acceptor on " << m_identifier << " ready for new connection";
	}
}

// Post a call to the stop function so that acceptor::stop() is safe to call
// from any thread.
void AcceptorSocket::stop()
{
	LOG_TRACE << "Acceptor for " << m_identifier << " received a shutdown request";
	m_IOservice->post( m_strand.wrap( boost::bind( &AcceptorSocket::handleStop, this )));
}

// The server is stopped by closing the acceptor.
// When all outstanding operations are completed
// all calls to io_service::run() will return.
void AcceptorSocket::handleStop()
{
	m_acceptor.close();
	LOG_DEBUG << "Closed acceptor for " << m_identifier;
	//TODO SIGNAL CONNECTIONS TO TERMINATE
}

