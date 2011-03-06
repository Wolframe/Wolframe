#ifndef _SERVER_ENDPOINT_HPP_INCLUDED
#define _SERVER_ENDPOINT_HPP_INCLUDED

#include <string>
#include "connectionEndpoint.hpp"

namespace _Wolframe	{
	namespace Network	{

	/// No encryption server endpoint
	class ServerTCPendpoint : public ConnectionEndpoint
	{
	public:
		ServerTCPendpoint( const std::string& Host, unsigned short Port, unsigned maxConn = 0 )
			: ConnectionEndpoint( Host, Port )
		{
			maxConnections_ = maxConn;
		}

		unsigned maxConnections() const	{ return maxConnections_; }

	private:
		unsigned	maxConnections_;
	};


#ifdef WITH_SSL
	/// SSL connection server endpoint
	class ServerSSLendpoint : public ServerTCPendpoint
	{
		friend class server;
	public:
		ServerSSLendpoint( const std::string& Host, unsigned short Port, unsigned maxConn,
				   const std::string& Certificate, const std::string& Key,
				   bool verify, const std::string& CAdir, const std::string& CAchainFile )
			: ServerTCPendpoint( Host, Port, maxConn )
		{
			cert_ = Certificate;
			key_ = Key;
			verify_ = verify;
			CAdir_ = CAdir;
			CAchain_ = CAchainFile;
		}

		const std::string& certificate() const	{ return cert_; }
		const std::string& key() const		{ return key_; }
		const std::string& CAdirectory() const	{ return CAdir_; }
		const std::string& CAchain() const	{ return CAchain_; }
		bool verifyClientCert() const		{ return verify_; }

		void setAbsolutePath( const std::string& referencePath );

	private:
		std::string	cert_;
		std::string	key_;
		std::string	CAdir_;
		std::string	CAchain_;
		bool		verify_;
	};
#endif // WITH_SSL

	} // namespace Network
} // namespace _Wolframe

#endif // _SERVER_ENDPOINT_HPP_INCLUDED
