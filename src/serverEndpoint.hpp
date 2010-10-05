#ifndef _SERVER_ENDPOINT_HPP_INCLUDED
#define _SERVER_ENDPOINT_HPP_INCLUDED

#include <string>
#include "connectionEndpoint.hpp"

namespace _SMERP	{

	/// No encryption server endpoint
	class ServerTCPendpoint : public ConnectionEndpoint
	{
	public:
		ServerTCPendpoint( const std::string& Address, unsigned short Port )
			: ConnectionEndpoint( Address, Port )	{}
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
		ServerSSLendpoint( const std::string& Address, unsigned short Port,
				   const std::string& Certificate, const std::string& Key,
				   bool verify, const std::string& CAdirectory, const std::string& CAchainFile )
			: ConnectionEndpoint( Address, Port )
		{
			cert_ = Certificate;
			key_ = Key;
			verify_ = verify;
			CAdir_ = CAdirectory;
			CAchain_ = CAchainFile;
		}
	};

}

#endif // _SERVER_ENDPOINT_HPP_INCLUDED
