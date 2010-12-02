//
// connectionHandler.hpp
//

#ifndef _CONNECTION_HANDLER_HPP_INCLUDED
#define _CONNECTION_HANDLER_HPP_INCLUDED

#include <cstddef>

#include "connectionEndpoint.hpp"

namespace _SMERP {
	namespace Network {

	struct	NetworkOperation
	{
	public:
		enum Operation	{
			READ,
			WRITE,
			TERMINATE
		};

	private:
		Operation	operation_;
		const void*	data_;
		std::size_t	size_;
		unsigned	timeout_;

	public:
		NetworkOperation( Operation op, unsigned to = 0 )
					{ operation_ = op, data_ = NULL; size_ = 0; timeout_ = to; }
		NetworkOperation( Operation op, const void *d, std::size_t s, unsigned to = 0 )
					{ operation_ = op, data_ = d; size_ = s; timeout_ = to; }


		Operation operation()	{ return operation_; }
		const void* data()	{ return data_; }
		std::size_t size()	{ return size_; }
		unsigned timeout()	{ return timeout_; }
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
		virtual void* parseInput( const void *begin, std::size_t bytesTransferred ) = 0;

		/// What should the network do next.
		virtual NetworkOperation nextOperation() = 0;

		/// What should the network do next.
		virtual void timeoutOccured()	{}

		/// A signal was received from outside.
		virtual void signalOccured()	{}

		/// Set the remote peer. The connection is up now.
		virtual void setPeer( const RemoteTCPendpoint& remote ) = 0;
		virtual void setPeer( const RemoteSSLendpoint& remote ) = 0;
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
		virtual connectionHandler* newConnection( const LocalTCPendpoint& local ) = 0;
		virtual connectionHandler* newSSLconnection( const LocalSSLendpoint& local ) = 0;
	};

} // namespace Network
} // namespace _SMERP

#endif // _CONNECTION_HANDLER_HPP_INCLUDED
