//
// connectionHandler.hpp
//

#ifndef _CONNECTION_HANDLER_HPP_INCLUDED
#define _CONNECTION_HANDLER_HPP_INCLUDED

#include <string>
#include <cstddef>
#include <cstring>

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
			CLOSE
		};

		explicit NetworkOperation( Operation op, void* d = NULL, std::size_t s = 0, unsigned to = 0 )
					{ operation_ = op,  data_ = d; size_ = s; timeout_ = to; }
		unsigned timeout()	{ return timeout_; }
		const void* data()	{ return (const void*)data_; }
		void* buffer()		{ return data_; }
		std::size_t size()	{ return size_; }
		Operation operation()	{ return operation_; }

	private:
		Operation	operation_;
		unsigned	timeout_;
		void*		data_;
		std::size_t	size_;
	};


	class ReadData : public NetworkOperation
	{
	public:
		explicit ReadData( void* d, std::size_t s, unsigned to = 0 )
			: NetworkOperation( READ, d, s, to )	{}
	};

	class SendData : public NetworkOperation
	{
	public:
		explicit SendData( const void* d, std::size_t s, unsigned to = 0 )
			: NetworkOperation( WRITE, const_cast<void*>( d ), s, to )	{}
	};

	/// This is just some syntactic sugar
	class SendString : public NetworkOperation
	{
	public:
		explicit SendString( const std::string& s, unsigned to = 0 )
			: NetworkOperation( WRITE, const_cast<char*>( s.c_str() ), s.length(), to )	{}
		explicit SendString( const char *s, unsigned to = 0 )
			: NetworkOperation( WRITE, const_cast<char*>( s ), strlen( s ), to )	{}
	};

	class CloseConnection : public NetworkOperation
	{
	public:
		CloseConnection() : NetworkOperation( CLOSE )	{}
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


		/// Signal the incoming data. buffer is the buffer given to the read operation
		virtual void networkInput( const void* buffer, std::size_t bytesTransferred ) = 0;

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
		ServerHandler( const ServerHandler& );
		ServerHandler& operator = ( const ServerHandler& );

		class ServerHandlerImpl;
		ServerHandlerImpl *impl_;
	};

} // namespace _SMERP

#endif // _CONNECTION_HANDLER_HPP_INCLUDED
