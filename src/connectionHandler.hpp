//
// connectionHandler.hpp
//

#ifndef _CONNECTION_HANDLER_HPP_INCLUDED
#define _CONNECTION_HANDLER_HPP_INCLUDED

#include <string>

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


	/// A message to be sent to a client.
	struct outputMessage
	{
	public:
		const void	*data;
		std::size_t	size;

		outputMessage()			{ data = NULL; size = 0;}
		outputMessage( const void *d, std::size_t s )
						{ data = d; size = s;}
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
		outputMessage	*msg;
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
		virtual char* parseInput( char *begin, std::size_t bytesTransferred ) const;

		/// Handle a request and produce a reply.
		virtual networkOperation nextOperation() const;

		virtual void setPeer( const connectionPeer& local, const connectionPeer& remote );
		virtual void setPeer( const SSLconnectionPeer& local, const SSLconnectionPeer& remote );
	private:
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
		virtual connectionHandler* newConnection() const;
		virtual connectionHandler* newSSLconnection() const;
	};
} // namespace _SMERP

#endif // _CONNECTION_HANDLER_HPP_INCLUDED
