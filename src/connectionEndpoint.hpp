#ifndef _CONNECTION_ENDPOINT_HPP_INCLUDED
#define _CONNECTION_ENDPOINT_HPP_INCLUDED

#include <string>

namespace _SMERP {

	/// Structures describing a
	class ConnectionEndpoint
	{
		std::string	address;
		unsigned short	port;

		connectionPeer( std::string address, unsigned short port )
						{ address = peerAddress; port = peerPort; }
		std::string toString();
	};

} // namespace _SMERP

#endif // _CONNECTION_ENDPOINT_HPP_INCLUDED
