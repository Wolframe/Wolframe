#ifndef _CONNECTION_ENDPOINT_HPP_INCLUDED
#define _CONNECTION_ENDPOINT_HPP_INCLUDED

#include <string>
#include <ostream>

namespace _SMERP {

	/// Structures describing a
	class ConnectionEndpoint
	{
	private:
		std::string	address_;
		unsigned short	port_;
	public:
		ConnectionEndpoint( std::string address, unsigned short port )
						{ address_ = address; port_ = port; }
		std::string address()		{ return address_; }
		unsigned short port()		{ return port_; }
		std::string toString();
	};

} // namespace _SMERP

#endif // _CONNECTION_ENDPOINT_HPP_INCLUDED
