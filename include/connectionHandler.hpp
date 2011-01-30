//
// connectionHandler.hpp
//

#ifndef _CONNECTION_HANDLER_HPP_INCLUDED
#define _CONNECTION_HANDLER_HPP_INCLUDED

#include <cstddef>

#include "connectionEndpoint.hpp"

namespace _SMERP {
	namespace Network {

	class	NetworkOperation
	{
		template< typename T > friend class connectionBase;
	protected:
		enum Operation	{
			READ,
			WRITE,
			CLOSE,
			TERMINATE
		};

		NetworkOperation( Operation op, const void* d = NULL, std::size_t s = 0, unsigned to = 0 )
					{ operation_ = op,  data_ = d; size_ = s; timeout_ = to; }
		unsigned timeout()			{ return timeout_; }
		const void* data()			{ return data_; }
		std::size_t size()			{ return size_; }
	public:
		Operation operation()	{ return operation_; }

	private:
		Operation	operation_;
//	protected:
		unsigned	timeout_;
		const void*	data_;
		std::size_t	size_;
	};


	class ReadOperation : public NetworkOperation
	{
	public:
		ReadOperation( unsigned to = 0 )
			: NetworkOperation( READ, NULL, 0, to )	{}
		ReadOperation( const void* d, std::size_t s, unsigned to = 0 )
			: NetworkOperation( WRITE, d, s, to )	{}
	};

	class WriteOperation : public NetworkOperation
	{
	public:
		WriteOperation( const void* d, std::size_t s, unsigned to = 0 )
			: NetworkOperation( WRITE, d, s, to )	{}
	};


	class CloseOperation : public NetworkOperation
	{
	public:
		CloseOperation() : NetworkOperation( CLOSE )	{}
	};

	class TerminateOperation : public NetworkOperation
	{
	public:
		TerminateOperation() : NetworkOperation( TERMINATE )	{}
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
		enum NetworkSignal	{
			END_OF_FILE,
			OPERATION_CANCELLED,
			BROKEN_PIPE,
			UNKNOWN_ERROR
		};


		/// Get the incoming data. The return value indicates how much of the
		/// input has been consumed.
		virtual void* networkInput( const void *begin, std::size_t bytesTransferred ) = 0;

		/// What should the network do next.
		virtual const NetworkOperation nextOperation() = 0;

		/// Timeout timer was fired.
		virtual void timeoutOccured()			{}

		/// A signal was received from outside.
		virtual void signalOccured()			{}

		/// An error network occured
		virtual void errorOccured( NetworkSignal )	{}

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

	/// The server
	class ServerHandler
	{
	public:
		ServerHandler();
		~ServerHandler();

		/// Create a new connection handler and return a pointer to it
		Network::connectionHandler* newConnection( const Network::LocalTCPendpoint& local );
		Network::connectionHandler* newSSLconnection( const Network::LocalSSLendpoint& local );
	private:
		class ServerHandlerImpl;
		ServerHandlerImpl *impl_;
	};

} // namespace _SMERP

#endif // _CONNECTION_HANDLER_HPP_INCLUDED
