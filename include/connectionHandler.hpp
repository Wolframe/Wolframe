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
	protected:
		enum Operation	{
			READ,
			WRITE,
			TERMINATE,
			END_OF_LIFE
		};

		NetworkOperation( Operation op, unsigned to = 0 )
					{ operation_ = op, timeout_ = to; }
	public:
		Operation operation()	{ return operation_; }
		virtual ~NetworkOperation() {}
	private:
		Operation	operation_;
	protected:
		unsigned	timeout_;
	};


	class ReadOperation : public NetworkOperation
	{
	public:
		ReadOperation( unsigned to = 0 )
			: NetworkOperation( READ, to )	{}

		unsigned timeout()			{ return timeout_; }
	};

	class WriteOperation : public NetworkOperation
	{
	public:
		WriteOperation( const void* d, std::size_t s, unsigned to = 0 )
			: NetworkOperation( WRITE, to )	{ data_ = d; size_ = s; }

		unsigned timeout()			{ return timeout_; }
		const void* data()			{ return data_; }
		std::size_t size()			{ return size_; }
	private:
		const void*	data_;
		std::size_t	size_;
	};


	class TerminateOperation : public NetworkOperation
	{
	public:
		TerminateOperation()
			: NetworkOperation( TERMINATE, 0 )	{}

		TerminateOperation( const void* d, std::size_t s, unsigned to = 0 )
			: NetworkOperation( TERMINATE, to )	{ data_ = d; size_ = s; }

		unsigned timeout()			{ return timeout_; }
		const void* data()			{ return data_; }
		std::size_t size()			{ return size_; }
	private:
		const void*	data_;
		std::size_t	size_;
	};

	class EOL_Operation : public NetworkOperation
	{
	public:
		EOL_Operation()
			: NetworkOperation( TERMINATE, 0 )	{}
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
		virtual NetworkOperation nextOperation() = 0;

		/// Timeout timer was fired.
		virtual void timeoutOccured()	{}

		/// A signal was received from outside.
		virtual void signalOccured()	{}

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
		ServerHandlerImpl	*impl_;
	};

} // namespace _SMERP

#endif // _CONNECTION_HANDLER_HPP_INCLUDED
