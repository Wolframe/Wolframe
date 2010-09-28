//
// connectionHandler.hpp
//

#ifndef _CONNECTION_HANDLER_HPP_INCLUDED
#define _CONNECTION_HANDLER_HPP_INCLUDED

#include <string>

#include "networkMessage.hpp"

namespace _SMERP {

	/// Structures describing the peer
	struct connectionPeer
	{
		std::string	address;
		unsigned short	port;

		connectionPeer( std::string peerAddress, unsigned short peerPort )
						{ address = peerAddress; port = peerPort; }
	};

	struct SSLconnectionPeer
	{
		std::string	dn;
		std::string	address;
		unsigned short	port;

		SSLconnectionPeer( std::string peerAddress, unsigned short peerPort )
						{ address = peerAddress; port = peerPort; }
	};


	struct	networkOperation
	{
	public:
		enum Operation	{
			READ,
			WRITE,
			TERMINATE
		};
		Operation	operation;
		NetworkMessage	msg;
	};


	/// The common handler for the connection status.
	class connectionHandler
	{
		template< typename socketType > friend class connectionBase;
	protected:
		connectionHandler()		{}
		virtual ~connectionHandler()	{}

	private:
		connectionHandler( const connectionHandler& );
		connectionHandler& operator = ( const connectionHandler& );

	public:
		/// Parse incoming data. The return value indicates how much of the
		/// input has been consumed.
		virtual char* parseInput( char *begin, std::size_t bytesTransferred ) = 0;

		/// Handle a request and produce a reply.
		virtual networkOperation nextOperation() = 0;

		virtual void setPeer( const connectionPeer& remote ) = 0;
		virtual void setPeer( const SSLconnectionPeer& remote ) = 0;
	};


	/// The server
	class ServerHandler
	{
	protected:
		ServerHandler()			{}
		virtual ~ServerHandler()	{}

	private:
		ServerHandler( const ServerHandler& );
		ServerHandler& operator = ( const ServerHandler& );

	public:
		/// Create a new connection handler and return a pointer to it
		virtual connectionHandler* newConnection( const connectionPeer& local ) = 0;
		virtual connectionHandler* newSSLconnection( const SSLconnectionPeer& local ) = 0;
	};
} // namespace _SMERP

#endif // _CONNECTION_HANDLER_HPP_INCLUDED
