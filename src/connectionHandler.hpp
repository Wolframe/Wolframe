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
	};

	struct SSLconnectionPeer
	{
		std::string	dn;
		std::string	address;
		unsigned short	port;
	};


	/// A message to be sent to a client.
	struct outputMessage
	{
	public:
		const void	*data;
		std::size_t	size;
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
	protected:
		virtual connectionHandler()	{}
		virtual ~connectionHandler()	{}

	private:
		connectionHandler( const connectionHandler& );
		connectionHandler& operator = ( const connectionHandler& );

	public:
		/// Parse incoming data. The return value indicates how much of the
		/// input has been consumed.
		virtual char *parseInput( char *begin, std::size_t bytesTransferred ) = 0;

		/// Handle a request and produce a reply.
		virtual networkOperation nextOperation() = 0;

	private:
	};


	/// The server
	class serverHandler
	{
	protected:
		virtual serverHandler()	{}
		virtual ~connectionHandler()	{}

	private:
		serverHandler( const serverHandler& );
		serverHandler& operator = ( const serverHandler& );

	public:
		/// Create a new connection handler and return a pointer to it
		virtual connectionHandler* newConnection( const connectionPeer& peer );
		virtual connectionHandler* newConnection( const SSLconnectionPeer& peer );
	};
} // namespace _SMERP

#endif // _CONNECTION_HANDLER_HPP_INCLUDED
