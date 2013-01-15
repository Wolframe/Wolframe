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

#ifndef _SERVER_ENDPOINT_HPP_INCLUDED
#define _SERVER_ENDPOINT_HPP_INCLUDED

#include <string>
#include "connectionEndpoint.hpp"

namespace _Wolframe	{
namespace net	{

	/// No encryption server endpoint
	class ServerTCPendpoint : public LocalTCPendpoint
	{
	public:
		ServerTCPendpoint( const std::string& Host, unsigned short Port,
				   const std::string& Identifier, unsigned short maxConn = 0 )
			: LocalTCPendpoint( Host, Port ),
			  m_identifier( Identifier ), m_maxConn( maxConn )	{}

		unsigned short maxConnections() const		{ return m_maxConn; }
		const std::string& identifier() const		{ return m_identifier; }

	private:
		const std::string	m_identifier;
		const unsigned short	m_maxConn;
	};


#ifdef WITH_SSL
	/// SSL connection server endpoint
	class ServerSSLendpoint : public LocalSSLendpoint
	{
		friend class server;
	public:
		ServerSSLendpoint( const std::string& Host, unsigned short Port,
				   const std::string& Identifier, unsigned short maxConn,
				   const std::string& Certificate, const std::string& Key,
				   bool verify, const std::string& CAdir, const std::string& CAchainFile )
			: LocalSSLendpoint( Host, Port ), m_identifier( Identifier ), m_maxConn( maxConn )
		{
			cert_ = Certificate;
			key_ = Key;
			verify_ = verify;
			CAdir_ = CAdir;
			CAchain_ = CAchainFile;
		}

		unsigned short maxConnections() const	{ return m_maxConn; }
		const std::string& identifier() const	{ return m_identifier; }
		const std::string& certificate() const	{ return cert_; }
		const std::string& key() const		{ return key_; }
		const std::string& CAdirectory() const	{ return CAdir_; }
		const std::string& CAchain() const	{ return CAchain_; }
		bool verifyClientCert() const		{ return verify_; }

		void setAbsolutePath( const std::string& referencePath );

	private:
		const std::string	m_identifier;
		const unsigned short	m_maxConn;
		std::string		cert_;
		std::string		key_;
		std::string		CAdir_;
		std::string		CAchain_;
		bool			verify_;
	};
#endif // WITH_SSL

}} // namespace _Wolframe::net

#endif // _SERVER_ENDPOINT_HPP_INCLUDED
