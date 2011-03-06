//
// connectionEndpoint.hpp
//

#ifndef _CONNECTION_ENDPOINT_HPP_INCLUDED
#define _CONNECTION_ENDPOINT_HPP_INCLUDED

#include <string>
#include <sstream>
#include <ctime>

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
				: ConnectionEndpoint( Host, Port )	{ connectionTime_ = time( NULL ); }

			time_t connectionTime() const			{ return connectionTime_; }

		private:
			time_t	connectionTime_;
		};

#ifdef WITH_SSL
		/// SSL connection endpoints
		class LocalSSLendpoint : public ConnectionEndpoint
		{
		public:
			LocalSSLendpoint( const std::string& Host, unsigned short Port )
				: ConnectionEndpoint( Host, Port )	{}
		};


		/// forward declaration for SSL certificate info
		class SSLcertificateInfo;

		class RemoteSSLendpoint : public ConnectionEndpoint
		{
		public:
			RemoteSSLendpoint( const std::string& Host, unsigned short Port )
				: ConnectionEndpoint( Host, Port )	{ connectionTime_ = time( NULL );
									  sslInfo_ = NULL;
									}

			RemoteSSLendpoint( const std::string& Host, unsigned short Port,
					  const SSLcertificateInfo *sslInfo )
				: ConnectionEndpoint( Host, Port )	{ connectionTime_ = time( NULL );
									  sslInfo_ = sslInfo;
									}

			time_t connectionTime() const			{ return connectionTime_; }
			/// SSL certificate information
			const SSLcertificateInfo* SSLcertInfo() const	{ return sslInfo_; }

		private:
			time_t				connectionTime_;
			const SSLcertificateInfo	*sslInfo_;
		};
#endif // WITH_SSL

	} // namespace Network
} // namespace _Wolframe

#endif // _CONNECTION_ENDPOINT_HPP_INCLUDED
