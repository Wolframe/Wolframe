//
// connectionHandler.hpp
//

#ifndef _CONNECTION_HANDLER_HPP_INCLUDED
#define _CONNECTION_HANDLER_HPP_INCLUDED

#include <cstddef>

#include "connectionEndpoint.hpp"

namespace _SMERP {

	struct	NetworkOperation
	{
	public:
		enum Operation	{
			SET_TIMEOUT,
			READ,
			WRITE,
			TERMINATE
		};

	private:
		Operation	operation_;
		const void	*data_;
		std::size_t	size_;
		unsigned long	timeout_;
		int		timeoutID_;

	public:
		explicit NetworkOperation( Operation op )
					{ operation_ = op, data_ = NULL; size_ = 0;
					  timeout_ = 0; timeoutID_ = -1; }
		NetworkOperation( Operation op, const void *d, std::size_t s )
					{ operation_ = op, data_ = d; size_ = s;
					  timeout_ = 0; timeoutID_ = -1; }
		NetworkOperation( Operation op, unsigned long to, int ID )
					{ operation_ = op, data_ = NULL; size_ = 0;
					  timeout_ = to; timeoutID_ = ID; }
		NetworkOperation( Operation op, const void *d, std::size_t s, unsigned long to, int ID )
					{ operation_ = op, data_ = d; size_ = s;
					  timeout_ = to; timeoutID_ = ID; }

		Operation operation()	{ return operation_; }
		const void* data()	{ return data_; }
		std::size_t size()	{ return size_; }
		unsigned long timeout()	{ return timeout_; }
		int timeoutID()		{ return timeoutID_; }
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
		virtual void timeoutOccured( unsigned /* ID */ )	{ }

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
} // namespace _SMERP

#endif // _CONNECTION_HANDLER_HPP_INCLUDED
