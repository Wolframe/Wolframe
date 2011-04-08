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
				TCP_CONNECTION,
				SSL_CONNECTION
			};

			ConnectionEndpoint( const std::string& Host, unsigned short Port, ConnectionType tp )
			{
				host_ = Host;
				port_ = Port;
				type_ = tp;
			}

			const std::string& host() const		{ return host_; }
			unsigned short port() const		{ return port_; }
			ConnectionType type() const		{ return type_; }

			std::string toString() const
			{
				std::ostringstream o;
				o << host_ << ":" << port_;
				return o.str();
			}

		private:
			std::string	host_;
			unsigned short	port_;
			ConnectionType	type_;
		};


		/// Local connection endpoints
		/// base class for local endpoint
		class LocalEndpoint : public ConnectionEndpoint
		{
		public:
			LocalEndpoint( const std::string& Host, unsigned short Port,
				      ConnectionType tp )
				: ConnectionEndpoint( Host, Port, tp )	{}
		};

		/// local unencrypted endpoint
		class LocalTCPendpoint : public LocalEndpoint
		{
		public:
			LocalTCPendpoint( const std::string& Host, unsigned short Port )
				: LocalEndpoint( Host, Port, TCP_CONNECTION )	{}
		};

#ifdef WITH_SSL
		/// local SSL connection endpoint
		class LocalSSLendpoint : public LocalEndpoint
		{
		public:
			LocalSSLendpoint( const std::string& Host, unsigned short Port )
				: LocalEndpoint( Host, Port, SSL_CONNECTION )	{}
		};
#endif // WITH_SSL


		/// Remote connection endpoints
		/// base class for remote endpoint
		class RemoteEndpoint : public ConnectionEndpoint
		{
		public:
			RemoteEndpoint( const std::string& Host, unsigned short Port,
				       ConnectionType tp )
				: ConnectionEndpoint( Host, Port, tp )
			{
				connectionTime_ = time( NULL );
			}

			time_t connectionTime() const			{ return connectionTime_; }

		private:
			time_t	connectionTime_;
		};

		/// remote unencrypted endpoint
		class RemoteTCPendpoint : public RemoteEndpoint
		{
		public:
			RemoteTCPendpoint( const std::string& Host, unsigned short Port )
				: RemoteEndpoint( Host, Port, TCP_CONNECTION )
			{
				connectionTime_ = time( NULL );
			}

			time_t connectionTime() const			{ return connectionTime_; }

		private:
			time_t	connectionTime_;
		};

#ifdef WITH_SSL
		/// forward declaration for SSL certificate info
		class SSLcertificateInfo;

		/// remote SSL connection endpoint
		class RemoteSSLendpoint : public RemoteEndpoint
		{
		public:
			RemoteSSLendpoint( const std::string& Host, unsigned short Port )
				: RemoteEndpoint( Host, Port, SSL_CONNECTION )
			{
				connectionTime_ = time( NULL );
				sslInfo_ = NULL;
			}

			RemoteSSLendpoint( const std::string& Host, unsigned short Port,
					  const SSLcertificateInfo *sslInfo )
				: RemoteEndpoint( Host, Port, SSL_CONNECTION )
			{
				connectionTime_ = time( NULL );
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

	} // namespace net
} // namespace _Wolframe

#endif // _CONNECTION_ENDPOINT_HPP_INCLUDED
