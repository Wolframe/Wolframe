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
// acceptor.hpp
//

#ifndef _NETWORK_ACCEPTOR_HPP_INCLUDED
#define _NETWORK_ACCEPTOR_HPP_INCLUDED

#include <boost/asio.hpp>
#ifdef WITH_SSL
#include <boost/asio/ssl.hpp>
#endif // WITH_SSL
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

#include <string>

#include "connection.hpp"
#include "system/connectionHandler.hpp"

namespace _Wolframe {
namespace net {

/// acceptor class of the Wolframe network server.
class acceptor: private boost::noncopyable
{
	/// public interface
public:
	/// Constructor
	explicit acceptor( boost::asio::io_service& IOservice,
			   const std::string& host, unsigned short port, unsigned maxConnections,
			   GlobalConnectionList& globalList,
			   _Wolframe::ServerHandler& srvHandler );

	/// Destructor
	~acceptor();

	/// Stop the acceptor. Outstanding asynchronous operations will be completed.
	void stop();

	/// private functions of the server
private:
	/// Handle completion of an asynchronous accept operation.
	void handleAccept( const boost::system::error_code& e );

	/// Handle a request to stop the server.
	void handleStop();

	/// object variables
	boost::asio::io_service&		m_IOservice;	// The io_service used to perform asynchronous operations.
	boost::asio::io_service::strand		m_strand;	// Strand to ensure the acceptor's handlers are not called concurrently.
	boost::asio::ip::tcp::acceptor		m_acceptor;	// Acceptor(s) used to listen for incoming connections.
	connection_ptr				m_newConnection;// The next connection to be accepted.
	SocketConnectionList< connection_ptr >	m_connList;	// List of active connections

	std::string				m_identifier;

	_Wolframe::ServerHandler&		m_srvHandler;	// The handler for all incoming requests.
};


#ifdef WITH_SSL

/// SSL acceptor class of the Wolframe network server.
class SSLacceptor: private boost::noncopyable
{
	/// public interface
public:
	/// Construct the acceptor
	explicit SSLacceptor( boost::asio::io_service& IOservice,
			      const std::string& certFile, const std::string& keyFile,
			      bool verify, const std::string& CAchainFile, const std::string& CAdirectory,
			      const std::string& host, unsigned short port, unsigned maxConnections,
			      GlobalConnectionList& globalList,
			      _Wolframe::ServerHandler& srvHandler );

	/// Destruct the serverrequestHandler&				requestHandler
	~SSLacceptor();

	/// Stop the acceptor. Outstanding asynchronous operations will be completed.
	void stop();

	/// Get a password from the console (i.e. SSL key password)
	std::string getPassword();

	/// private functions of the server
private:
	/// Handle completion of an asynchronous accept operation.
	void handleAccept( const boost::system::error_code& e );

	/// Handle a request to stop the server.
	void handleStop();

	/// object variables
	boost::asio::io_service&		m_IOservice;	// The io_service used to perform asynchronous operations.
	boost::asio::io_service::strand		m_strand;	// Strand to ensure the acceptor's handlers are not called concurrently.
	boost::asio::ip::tcp::acceptor		m_acceptor;	// Acceptor(s) used to listen for incoming connections.
	boost::asio::ssl::context		m_SSLcontext;	// SSL acceptor server context
	SSLconnection_ptr			m_newConnection;// The next connection to be accepted.
	SocketConnectionList< SSLconnection_ptr > m_connList;	// List of active connections

	std::string				m_identifier;

	_Wolframe::ServerHandler&		m_srvHandler;	// The handler for all incoming requests.
};

#endif // WITH_SSL

}} // namespace _Wolframe::net

#endif // _NETWORK_ACCEPTOR_HPP_INCLUDED
