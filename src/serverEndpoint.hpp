#ifndef _SERVER_ENDPOINT_HPP_INCLUDED
#define _SERVER_ENDPOINT_HPP_INCLUDED

#include <string>
#include "connectionEndpoint.hpp"

namespace _SMERP	{

	/// No encryption server endpoint
	class ServerTCPendpoint : public ConnectionEndpoint
	{
	public:
		ServerTCPendpoint( const std::string& Host, unsigned short Port )
			: ConnectionEndpoint( Host, Port )	{}
	};


	/// SSL connection server endpoint
	class ServerSSLendpoint : public ConnectionEndpoint
	{
		friend class server;
	private:
		std::string	cert_;
		std::string	key_;
		std::string	CAdir_;
		std::string	CAchain_;
		bool		verify_;
	public:
		ServerSSLendpoint( const std::string& Host, unsigned short Port,
				   const std::string& Certificate, const std::string& Key,
				   bool verify, const std::string& CAdir, const std::string& CAchainFile )
			: ConnectionEndpoint( Host, Port )
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
	};

}

#endif // _SERVER_ENDPOINT_HPP_INCLUDED
