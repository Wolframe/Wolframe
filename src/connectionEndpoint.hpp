#ifndef _CONNECTION_ENDPOINT_HPP_INCLUDED
#define _CONNECTION_ENDPOINT_HPP_INCLUDED

#include <string>
#include <sstream>

namespace _SMERP {

	/// Structures describing a
	class ConnectionEndpoint
	{
	private:
		std::string	address_;
		unsigned short	port_;
	public:
		ConnectionEndpoint( const std::string& Address, unsigned short Port )
						{ address_ = Address; port_ = Port; }

		std::string address()		{ return address_; }
		unsigned short port()		{ return port_; }
		std::string toString() const
		{
			std::ostringstream o;
			o << address_ << ":" << port_;
			return o.str();
		}
	};


	/// No encryption endpoints
	class LocalTCPendpoint : public ConnectionEndpoint
	{
	public:
		LocalTCPendpoint( const std::string& Address, unsigned short Port )
			: ConnectionEndpoint( Address, Port )	{}
	};

	class RemoteTCPendpoint : public ConnectionEndpoint
	{
	public:
		RemoteTCPendpoint( const std::string& Address, unsigned short Port )
			: ConnectionEndpoint( Address, Port )	{}
	};


	/// SSL connection endpoints
	class LocalSSLendpoint : public ConnectionEndpoint
	{
	public:
		LocalSSLendpoint( const std::string& Address, unsigned short Port )
			: ConnectionEndpoint( Address, Port )	{}
	};

	class RemoteSSLendpoint : public ConnectionEndpoint
	{
	public:
		RemoteSSLendpoint( const std::string& Address, unsigned short Port )
			: ConnectionEndpoint( Address, Port )	{}
	};

} // namespace _SMERP

#endif // _CONNECTION_ENDPOINT_HPP_INCLUDED
