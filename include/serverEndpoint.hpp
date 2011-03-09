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
		ServerTCPendpoint( const std::string& Host, unsigned short Port,
				  unsigned short maxConn = 0 )
			: ConnectionEndpoint( Host, Port, TCP_CONNECTION )
		{
			maxConnections_ = maxConn;
		}

		unsigned short maxConnections() const	{ return maxConnections_; }

	private:
		unsigned short	maxConnections_;
	};


	/// SSL connection server endpoint
	class ServerSSLendpoint : public ConnectionEndpoint
	{
		friend class server;
	public:
		ServerSSLendpoint( const std::string& Host, unsigned short Port, unsigned short maxConn,
				   const std::string& Certificate, const std::string& Key,
				   bool verify, const std::string& CAdir, const std::string& CAchainFile )
			: ConnectionEndpoint( Host, Port, SSL_CONNECTION )
		{
			maxConnections_ = maxConn;
			cert_ = Certificate;
			key_ = Key;
			verify_ = verify;
			CAdir_ = CAdir;
			CAchain_ = CAchainFile;
		}

		unsigned short maxConnections() const	{ return maxConnections_; }
		const std::string& certificate() const	{ return cert_; }
		const std::string& key() const		{ return key_; }
		const std::string& CAdirectory() const	{ return CAdir_; }
		const std::string& CAchain() const	{ return CAchain_; }
		bool verifyClientCert() const		{ return verify_; }

		void setAbsolutePath( const std::string& referencePath );

	private:
		unsigned short	maxConnections_;
		std::string	cert_;
		std::string	key_;
		std::string	CAdir_;
		std::string	CAchain_;
		bool		verify_;
	};

	} // namespace Network
} // namespace _Wolframe

#endif // _SERVER_ENDPOINT_HPP_INCLUDED
