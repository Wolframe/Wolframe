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
/// \brief Classes for network endpoints
/// \file system/connectionEndpoint.hpp

#ifndef _CONNECTION_ENDPOINT_HPP_INCLUDED
#define _CONNECTION_ENDPOINT_HPP_INCLUDED

#include <string>
#include <vector>
#include <sstream>
#include <ctime>
#include <boost/algorithm/string.hpp>
#include <boost/shared_ptr.hpp>

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
	static const char* connectionTypeName( ConnectionType t)
	{
		static const char* ar[] = {"UDP","TCP","SSL"};
		return ar[t];
	}

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
	void operator=( const ConnectionEndpoint& )	{}
private:
	std::string	m_host;
	unsigned short	m_port;
};

/// Local connection endpoint configuration for authorization, connection based timeout, etc.
struct LocalEndpointConfig
{
	enum ProtocolCapability
	{
		PasswordChange=0x1,	///< the user is allowed to change his password
		Request=0x2		///< the user is allowed make requests
	};
	static const char* protocolCapabilityName( ProtocolCapability c)
	{
		static const char* ar[] = {0,"PasswordChange","Request"};
		return ar[c];
	}

	std::vector<std::string> capabilities;
	std::string socketIdentifier;
	std::string protocol;

	/// \brief Default constructor
	LocalEndpointConfig()
		:capabilities(0xFFFF){}
	/// \brief Copy constructor
	LocalEndpointConfig( const LocalEndpointConfig& o)
		:capabilities(o.capabilities),socketIdentifier(o.socketIdentifier){}
	/// \brief Constructor
	explicit LocalEndpointConfig( const std::string& socketIdentifier_)
		:capabilities(0xFFFF),socketIdentifier(socketIdentifier_){}

	/// \brief Reset capabilities
	void resetCapabilities()
	{
		capabilities.clear();
	}
	/// \brief Set a capability for this local endpoint configuration
	void setCapability( const std::string& c)
	{
		capabilities.push_back( c);
	}
	/// \brief Ask for a capability for this local endpoint configuration
	bool hasCapability( const std::string& c) const
	{
		std::vector<std::string>::const_iterator ci = capabilities.begin(), ce = capabilities.end();
		for (; ci != ce; ++ci)
		{
			if (boost::algorithm::iequals( *ci, c)) return true;
		}
		return false;
	}
};

/// Local connection endpoints
class LocalEndpoint : public ConnectionEndpoint
{
public:
	LocalEndpoint( const std::string& Host, unsigned short Port, const LocalEndpointConfig& Config=LocalEndpointConfig())
		: ConnectionEndpoint( Host, Port ), m_config( Config )
	{
		m_creationTime = time( NULL );
	}

	virtual ConnectionType type() const = 0;
	EndPoint endpoint() const			{ return LOCAL_ENDPOINT; }
	time_t creationTime() const			{ return m_creationTime; }
	const LocalEndpointConfig& config() const	{ return m_config; }

private:
	LocalEndpointConfig m_config;			///< configuration for authorization checks
	time_t	m_creationTime;				///< time when object has been constructed
};

typedef boost::shared_ptr<LocalEndpoint> LocalEndpointR;


/// Local unencrypted endpoint
class LocalTCPendpoint : public LocalEndpoint
{
public:
	LocalTCPendpoint( const std::string& Host, unsigned short Port, const LocalEndpointConfig& Config=LocalEndpointConfig())
		: LocalEndpoint( Host, Port, Config )		{}

	ConnectionType type() const			{ return TCP; }
};

#ifdef WITH_SSL
/// Local SSL connection endpoint
class LocalSSLendpoint : public LocalEndpoint
{
public:
	LocalSSLendpoint( const std::string& Host, unsigned short Port, const LocalEndpointConfig& Config=LocalEndpointConfig())
		: LocalEndpoint( Host, Port, Config )		{}

	ConnectionType type() const			{ return SSL; }
};
#endif // WITH_SSL


/// \brief Remote connection endpoint
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

typedef boost::shared_ptr<RemoteEndpoint> RemoteEndpointR;


/// Remote unencrypted endpoint
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
