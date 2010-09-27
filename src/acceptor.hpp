//
// acceptor.hpp
//

#ifndef _NETWORK_ACCEPTOR_HPP_INCLUDED
#define _NETWORK_ACCEPTOR_HPP_INCLUDED

#include <boost/asio.hpp>
#ifdef WITH_SSL
#include <boost/asio/ssl.hpp>
#endif // WITH_SSL
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

#include <string>

#include "connectionTimeout.hpp"
#include "connection.hpp"
#include "connectionHandler.hpp"

namespace _SMERP {

/// acceptor class of the SMERP network server.
class acceptor: private boost::noncopyable
{
/// public interface
public:
	/// Construct the acceptor
	explicit acceptor( boost::asio::io_service& IOservice,
			   const std::string& host, const unsigned short port,
			   const connectionTimeout& timeouts,
			   const ServerHandler& srvHandler );

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
	const connectionTimeout&		timeouts_;
	std::string				identifier_;

	const ServerHandler&			srvHandler_;	// The handler for all incoming requests.
};


#ifdef WITH_SSL

/// SSL acceptor class of the SMERP network server.
class SSLacceptor: private boost::noncopyable
{
/// public interface
public:
	/// Construct the acceptor
	explicit SSLacceptor( boost::asio::io_service& IOservice,
			      const std::string& certFile, const std::string& keyFile,
				  bool verify, const std::string& CAchainFile, const std::string& CAdirectory,
				  const std::string& host, const unsigned short port,
				  const connectionTimeout& timeouts,
				  const ServerHandler& srvHandler );

	/// Destruct the serverrequestHandler&				requestHandler
	~SSLacceptor();

	/// Stop the acceptor. Outstanding asynchronous operations will be completed.
	void stop();

	/// Abort the server. Outstanding asynchronous operations will be aborted.
	void abort();

	/// Get a password from the console (i.e. SSL key password)
	std::string getPassword();

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
	boost::asio::ssl::context		SSLcontext_;	/// SSL acceptor server context
	SSLconnection_ptr			newConnection_;	// The next connection to be accepted.
	const connectionTimeout&		timeouts_;
	std::string				identifier_;

	const ServerHandler&			srvHandler_;	// The handler for all incoming requests.
};

#endif // WITH_SSL

} // namespace _SMERP

#endif // _NETWORK_ACCEPTOR_HPP_INCLUDED
