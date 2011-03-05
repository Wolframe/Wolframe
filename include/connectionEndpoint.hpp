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
				: ConnectionEndpoint( Host, Port )	{ connectionTime_ = time( NULL );
									  certSerialNumber_ = 0;
									  certNotBefore_ = 0;
									  certNotAfter_ = 0;
									}

			RemoteSSLendpoint( const std::string& Host, unsigned short Port,
					  unsigned long serialNumber, time_t notBefore, time_t notAfter,
					  const std::string& issuer,
					  const std::string& subject, const std::string& commonName )
				: ConnectionEndpoint( Host, Port )	{ connectionTime_ = time( NULL );
									  certSerialNumber_ = serialNumber;
									  certIssuer_ = issuer;
									  certNotBefore_ = notBefore;
									  certNotAfter_ = notAfter;
									  certSubject_ = subject;
									  certCommonName_ = commonName;
									}

			time_t connectionTime() const				{ return connectionTime_; }
			/// SSL certificate information
			unsigned long certSerialNumber() const			{ return certSerialNumber_; }
			const std::string& certIssuer() const			{ return certIssuer_; }
			time_t certNotBefore() const				{ return certNotBefore_; }
			time_t certNotAfter() const				{ return certNotAfter_; }
			const std::string& certSubject() const			{ return certSubject_; }
			const std::string& certCommonName() const		{ return certCommonName_; }

		private:
			time_t		connectionTime_;
			unsigned long	certSerialNumber_;
			std::string	certIssuer_;
			time_t		certNotBefore_;
			time_t		certNotAfter_;
			std::string	certSubject_;
			std::string	certCommonName_;
		};

	} // namespace Network
} // namespace _Wolframe

#endif // _CONNECTION_ENDPOINT_HPP_INCLUDED
