//
// server.hpp
//

#ifndef _NETWORK_SERVER_HPP_INCLUDED
#define _NETWORK_SERVER_HPP_INCLUDED

#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>

#include <list>

#include "serverEndpoint.hpp"
#include "acceptor.hpp"
#include "connectionHandler.hpp"
#include "standardConfigs.hpp"

namespace _Wolframe {
	namespace Network	{

		/// The top-level class of the Wolframe network server.
		class server: private boost::noncopyable
		{
			/// public interface
		public:
			/// Construct the server
			explicit server( const ServerConfiguration& config,
					 _Wolframe::ServerHandler& serverHandler );

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
			std::size_t		threadPoolSize_;

			/// The io_service used to perform asynchronous operations.
			boost::asio::io_service	IOservice_;
			/// The list(s) of connection acceptors.
			std::list<acceptor*>	acceptor_;
#ifdef WITH_SSL
			std::list<SSLacceptor*>	SSLacceptor_;
#endif // WITH_SSL
			GlobalConnectionList	globalList_;
		};

	} // namespace Network
} // namespace _Wolframe

#endif // _NETWORK_SERVER_HPP_INCLUDED
