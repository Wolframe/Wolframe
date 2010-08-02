//
// acceptor.hpp
//

#ifndef _NETWORK_ACCEPTOR_HPP_INCLUDED
#define _NETWORK_ACCEPTOR_HPP_INCLUDED

#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

#include <string>

#include "connectionTimeout.hpp"
#include "connection.hpp"
#include "requestHandler.hpp"

namespace _SMERP {

/// The top-level class of the SMERP network server.
class acceptor: private boost::noncopyable
{
/// public interface
public:
	/// Construct the acceptor
	explicit acceptor( boost::asio::io_service& IOservice,
			   const std::string& host, const unsigned short port,
			   connectionTimeout& timeouts,
			   requestHandler& reqHandler );

	/// Destruct the serverrequestHandler&				requestHandler
	~acceptor();

	/// Stop the acceptor. Outstanding asynchronous operations will be completed.
	void stop();

	/// Abort the server. Outstanding asynchronous operations will be aborted.
	void abort();

/// private functions of the server
private:
	/// Handle completion of an asynchronous accept operation.
	void handleAccept( const boost::system::error_code& e );

	/// Handle a request to stop the server.
	void handleStop();

/// object variables
	boost::asio::io_service&		IOservice_;	// The io_service used to perform asynchronous operations.
	boost::asio::io_service::strand		strand_;	// Strand to ensure the acceptor's handlers are not called concurrently.
	boost::asio::ip::tcp::acceptor		acceptor_;	// Acceptor(s) used to listen for incoming connections.
	connection_ptr				newConnection_;	// The next connection to be accepted.
	connectionTimeout&			timeouts_;
	std::string				identifier_;

	requestHandler&				requestHandler_;// The handler for all incoming requests.
};

} // namespace _SMERP

#endif // _NETWORK_ACCEPTOR_HPP_INCLUDED
