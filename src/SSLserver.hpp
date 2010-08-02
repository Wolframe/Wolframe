//
// server.hpp
//

#ifndef _NETWORK_SSL_SERVER_HPP_INCLUDED
#define _NETWORK_SSL_SERVER_HPP_INCLUDED

#include <boost/noncopyable.hpp>
#include <boost/asio/ssl.hpp>

#include <string>

#include "appConfig.hpp"
#include "SSLconnection.hpp"
#include "server.hpp"

namespace _SMERP {

/// The top-level class of the SMERP server.
class SSLserver: public server/*,
		 private boost::noncopyable*/
{
/// public interface
public:
	/// Construct the server
	explicit SSLserver( const ApplicationConfiguration& config );

	/// Destruct the server
	~SSLserver();

	/// Run the server's io_service loop.
	void run();

	/// Stop the server. Outstanding asynchronous operations will be completed.
	void stop();

	/// Abort the server. Outstanding asynchronous operations will be aborted.
	void abort();

/// private functions of the server
private:
	/// Handle completion of an asynchronous accept operation.
	void handleAccept( const boost::system::error_code& e );

	/// Handle a request to stop the server.
	void handleStop();

	/// SSL server context
	boost::asio::ssl::context		SSLcontext_;

	/// Get a password from the console (i.e. SSL key password)
	std::string getPassword();
};

} // namespace _SMERP

#endif // _NETWORK_SSL_SERVER_HPP_INCLUDED
