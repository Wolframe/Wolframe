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
// connectionEndpoint.hpp
//

#ifndef _CONNECTION_ENDPOINT_HPP_INCLUDED
#define _CONNECTION_ENDPOINT_HPP_INCLUDED

#include <string>
#include <sstream>
#include <ctime>

namespace _Wolframe {
namespace net {

/// Base class for network endpoints
class ConnectionEndpoint
{
public:
	enum ConnectionType	{
		UDP,
		TCP,
		SSL
	};

	enum EndPoint	{
		LOCAL_ENDPOINT,
		REMOTE_ENDPOINT
	};

	ConnectionEndpoint( const std::string& Host, unsigned short Port )
		: m_host( Host ), m_port( Port )	{}

	virtual ~ConnectionEndpoint( ) {}

	const std::string& host() const			{ return m_host; }
	unsigned short port() const			{ return m_port; }
	virtual ConnectionType type() const = 0;
	virtual EndPoint endpoint() const = 0;

	std::string toString() const
	{
		std::ostringstream o;
		o << m_host << ":" << m_port;
		return o.str();
	}

private:
	const std::string	m_host;
	const unsigned short	m_port;
};


/// Local connection endpoints
/// base class for local endpoint
class LocalEndpoint : public ConnectionEndpoint
{
public:
	LocalEndpoint( const std::string& Host, unsigned short Port )
		: ConnectionEndpoint( Host, Port )
	{
		m_creationTime = time( NULL );
	}

	virtual ConnectionType type() const = 0;
	EndPoint endpoint() const			{ return LOCAL_ENDPOINT; }
	time_t creationTime() const			{ return m_creationTime; }

private:
	time_t	m_creationTime;
};

/// local unencrypted endpoint
class LocalTCPendpoint : public LocalEndpoint
{
public:
	LocalTCPendpoint( const std::string& Host, unsigned short Port )
		: LocalEndpoint( Host, Port )		{}

	ConnectionType type() const			{ return TCP; }
};

#ifdef WITH_SSL
/// local SSL connection endpoint
class LocalSSLendpoint : public LocalEndpoint
{
public:
	LocalSSLendpoint( const std::string& Host, unsigned short Port )
		: LocalEndpoint( Host, Port )		{}

	ConnectionType type() const			{ return SSL; }
};
#endif // WITH_SSL


/// Remote connection endpoints
/// base class for remote endpoint
class RemoteEndpoint : public ConnectionEndpoint
{
public:
	RemoteEndpoint( const std::string& Host, unsigned short Port )
		: ConnectionEndpoint( Host, Port )
	{
		m_connectionTime = time( NULL );
	}

	virtual ConnectionType type() const = 0;
	EndPoint endpoint() const			{ return REMOTE_ENDPOINT; }

	time_t connectionTime() const			{ return m_connectionTime; }

private:
	time_t	m_connectionTime;
};

/// remote unencrypted endpoint
class RemoteTCPendpoint : public RemoteEndpoint
{
public:
	RemoteTCPendpoint( const std::string& Host, unsigned short Port )
		: RemoteEndpoint( Host, Port )		{}

	ConnectionType type() const			{ return TCP; }
};

#ifdef WITH_SSL
/// forward declaration for SSL certificate info
class SSLcertificateInfo;

/// remote SSL connection endpoint
class RemoteSSLendpoint : public RemoteEndpoint
{
public:
	RemoteSSLendpoint( const std::string& Host, unsigned short Port )
		: RemoteEndpoint( Host, Port ), m_SSLinfo( NULL )	{}

	RemoteSSLendpoint( const std::string& Host, unsigned short Port,
			   const SSLcertificateInfo *SSLinfo )
		: RemoteEndpoint( Host, Port ), m_SSLinfo( SSLinfo )	{}

	ConnectionType type() const			{ return SSL; }

	/// SSL certificate information
	const SSLcertificateInfo* SSLcertInfo() const	{ return m_SSLinfo; }

private:
	const SSLcertificateInfo	*m_SSLinfo;
};
#endif // WITH_SSL

}} // namespace _Wolframe::net

#endif // _CONNECTION_ENDPOINT_HPP_INCLUDED
