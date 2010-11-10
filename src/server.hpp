//
// server.hpp
//

#ifndef _NETWORK_SERVER_HPP_INCLUDED
#define _NETWORK_SERVER_HPP_INCLUDED

#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>

#include <vector>

#include "serverEndpoint.hpp"
#include "acceptor.hpp"
#include "connectionHandler.hpp"

namespace _SMERP {
	namespace Network	{

		/// The top-level class of the SMERP network server.
		class server: private boost::noncopyable
		{
			/// public interface
		public:
			/// Construct the server
			explicit server( const std::vector<ServerTCPendpoint>& TCPserver,
					 const std::vector<ServerSSLendpoint>& SSLserver,
					 ServerHandler& serverHandler,
					 unsigned threads, unsigned maxConnections );

			/// Destruct the server
			~server();

			/// Run the server's io_service loop.
			void run();

			/// Stop the server. Outstanding asynchronous operations will be completed.
			void stop();

			/// Abort the server. Outstanding asynchronous operations will be aborted.
			void abort();

		private:
			/// The number of threads that will call io_service::run().
			std::size_t				threadPoolSize_;
			/// The maximum number of simultaneous connections accepted by the server.
			unsigned				maxConnections_;
			/// The io_service used to perform asynchronous operations.
			boost::asio::io_service			IOservice_;
			/// The vector(s) of connection acceptors.
			std::vector<acceptor*>			acceptor_;
#ifdef WITH_SSL
			std::vector<SSLacceptor*>		SSLacceptor_;
#endif // WITH_SSL
		};

	} // namespace Network
} // namespace _SMERP

#endif // _NETWORK_SERVER_HPP_INCLUDED
