#ifndef _CONNECTION_ENDPOINT_HPP_INCLUDED
#define _CONNECTION_ENDPOINT_HPP_INCLUDED

#include <string>
#include <sstream>

namespace _Wolframe {
	namespace Network {

		/// Base class for network endpoints
		class ConnectionEndpoint
		{
		private:
			std::string	host_;
			unsigned short	port_;
		public:
			ConnectionEndpoint( const std::string& Host, unsigned short Port )
			{ host_ = Host; port_ = Port; }

			const std::string& host() const	{ return host_; }
			unsigned short port() const	{ return port_; }
			std::string toString() const
			{
				std::ostringstream o;
				o << host_ << ":" << port_;
				return o.str();
			}
		};


		/// No encryption endpoints
		class LocalTCPendpoint : public ConnectionEndpoint
		{
		public:
			LocalTCPendpoint( const std::string& Host, unsigned short Port )
				: ConnectionEndpoint( Host, Port )	{}
		};

		class RemoteTCPendpoint : public ConnectionEndpoint
		{
		public:
			RemoteTCPendpoint( const std::string& Host, unsigned short Port )
				: ConnectionEndpoint( Host, Port )	{}
		};


		/// SSL connection endpoints
		class LocalSSLendpoint : public ConnectionEndpoint
		{
		public:
			LocalSSLendpoint( const std::string& Host, unsigned short Port )
				: ConnectionEndpoint( Host, Port )	{}
		};

		class RemoteSSLendpoint : public ConnectionEndpoint
		{
		public:
			RemoteSSLendpoint( const std::string& Host, unsigned short Port )
				: ConnectionEndpoint( Host, Port )	{}
			RemoteSSLendpoint( const std::string& Host, unsigned short Port,
					   const std::string& CN )
						   : ConnectionEndpoint( Host, Port )	{ CN_ = CN; }

			const std::string& commonName() const		{ return CN_; }
		private:
			std::string	CN_;
		};

	} // namespace Network
} // namespace _Wolframe

#endif // _CONNECTION_ENDPOINT_HPP_INCLUDED
