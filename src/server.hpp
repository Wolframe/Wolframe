//
// server.hpp
//

#ifndef _NETWORK_SERVER_HPP_INCLUDED
#define _NETWORK_SERVER_HPP_INCLUDED

#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>

#include <string>
#include <vector>

#include "appConfig.hpp"
#include "serverEndpoint.hpp"
#include "acceptor.hpp"
#include "connectionHandler.hpp"

namespace _SMERP {

/// The top-level class of the SMERP network server.
class server: private boost::noncopyable
{
/// public interface
public:
	/// Construct the server
	explicit server( const ApplicationConfiguration& config, ServerHandler& serverHandler );

	/// Destruct the server
	~server();

	/// Run the server's io_service loop.
	void run();

	/// Stop the server. Outstanding asynchronous operations will be completed.
	void stop();

	/// Abort the server. Outstanding asynchronous operations will be aborted.
	void abort();

private:
	std::size_t				threadPoolSize_;// The number of threads that will call io_service::run().

	boost::asio::io_service			IOservice_;	// The io_service used to perform asynchronous operations.
	connectionTimeout			timeouts_;
	std::vector<acceptor*>			acceptor_;
#ifdef WITH_SSL
	std::vector<SSLacceptor*>		SSLacceptor_;
#endif // WITH_SSL
};

} // namespace _SMERP

#endif // _NETWORK_SERVER_HPP_INCLUDED
