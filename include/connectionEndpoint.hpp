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

			enum EndPoint	{
				LOCAL_ENDPOINT,
				REMOTE_ENDPOINT
			};

			ConnectionEndpoint( const std::string& Host, unsigned short Port )
				: m_host( Host ), m_port( Port )	{}

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
				m_connectionTime = time( NULL );
			}

			virtual ConnectionType type() const = 0;
			EndPoint endpoint() const			{ return LOCAL_ENDPOINT; }
			time_t connectionTime() const			{ return m_connectionTime; }

		private:
			time_t	m_connectionTime;
		};

		/// local unencrypted endpoint
		class LocalTCPendpoint : public LocalEndpoint
		{
		public:
			LocalTCPendpoint( const std::string& Host, unsigned short Port )
				: LocalEndpoint( Host, Port )		{}

			ConnectionType type() const			{ return TCP_CONNECTION; }
		};

#ifdef WITH_SSL
		/// local SSL connection endpoint
		class LocalSSLendpoint : public LocalEndpoint
		{
		public:
			LocalSSLendpoint( const std::string& Host, unsigned short Port )
				: LocalEndpoint( Host, Port )		{}

			ConnectionType type() const			{ return SSL_CONNECTION; }
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

			ConnectionType type() const			{ return TCP_CONNECTION; }
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

			ConnectionType type() const			{ return SSL_CONNECTION; }

			/// SSL certificate information
			const SSLcertificateInfo* SSLcertInfo() const	{ return m_SSLinfo; }

		private:
			const SSLcertificateInfo	*m_SSLinfo;
		};
#endif // WITH_SSL

	} // namespace net
} // namespace _Wolframe

#endif // _CONNECTION_ENDPOINT_HPP_INCLUDED
