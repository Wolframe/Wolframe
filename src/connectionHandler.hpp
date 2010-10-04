//
// connectionHandler.hpp
//

#ifndef _CONNECTION_HANDLER_HPP_INCLUDED
#define _CONNECTION_HANDLER_HPP_INCLUDED

#include <cstddef>
#include <cstring>
#include <string>

#include "connectionEndpoint.hpp"

namespace _SMERP {

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
		const void	*data_;
		std::size_t	size_;

	public:
		NetworkOperation( Operation op )
					{ operation_ = op, data_ = NULL; size_ = 0; }
		NetworkOperation( Operation op, const void *d, std::size_t s )
					{ operation_ = op, data_ = d; size_ = s; }
		NetworkOperation( Operation op, const char *str )
					{ operation_ = op, data_ = str; size_ = std::strlen( str); }
		NetworkOperation( Operation op, std::string& str )
					{ operation_ = op, data_ = str.c_str(); size_ = str.length(); }

		Operation operation()	{ return operation_; }
		const void* data()	{ return data_; }
		std::size_t size()	{ return size_; }
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

		/// Handle a request and produce a reply.
		virtual NetworkOperation nextOperation() = 0;

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
