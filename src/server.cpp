//
// server.cpp
//

#include "server.hpp"
#include "logger.hpp"

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>


namespace _SMERP {

server::server( const ApplicationConfiguration& config )
	: threadPoolSize_( config.threads ),
	timeout_duration_((unsigned long)config.idleTimeout ),
	strand_( IOservice_ ),
	acceptor_( IOservice_ ),
	requestHandler_()
{
	boost::system::error_code	ec;

	// Open the acceptor(s) with the option to reuse the address (i.e. SO_REUSEADDR).
	boost::asio::ip::tcp::resolver resolver( IOservice_ );
	boost::asio::ip::tcp::resolver::query query( config.address[0].host, "");
	boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve( query );
	endpoint.port( config.address[0].port );

	newConnection_ = connection_ptr( new connection( IOservice_, requestHandler_, timeout_duration_ ));

	acceptor_.open( endpoint.protocol() );
	acceptor_.set_option( boost::asio::ip::tcp::acceptor::reuse_address( true ));
	acceptor_.bind( endpoint );
	acceptor_.listen();
	acceptor_.async_accept( newConnection_->socket(),
				strand_.wrap( boost::bind( &server::handleAccept,
							   this,
							   boost::asio::placeholders::error )));
	LOG_INFO << "Accepting connections on " << acceptor_.local_endpoint().address().to_string()
			<< " port " << acceptor_.local_endpoint().port();

	LOG_DEBUG << "Server is waiting for connections";
}


server::~server()
{
	LOG_TRACE << "Server destructor called";
}


void server::run()
{
	// Create a pool of threads to run all of the io_services.
	std::vector<boost::shared_ptr<boost::thread> >	threads;
	std::size_t					i;

	for ( i = 0; i < threadPoolSize_; ++i )	{
		boost::shared_ptr<boost::thread> thread( new boost::thread( boost::bind( &boost::asio::io_service::run, &IOservice_ )));
		threads.push_back( thread );
	}
	LOG_TRACE << i << " network server threads started";

	// Wait for all threads in the pool to exit.
	for ( i = 0; i < threads.size(); ++i )
		threads[i]->join();

	// Reset io_services.
	IOservice_.reset();
}


void server::stop()
{
	LOG_DEBUG << "Network server received a shutdown request";
	// Post a call to the stop function so that server::stop() is safe to call
	// from any thread.
	IOservice_.post( strand_.wrap( boost::bind( &server::handleStop, this )));
}


// Stop io_services the hard way.
void server::abort()
{
	LOG_DEBUG << "Network server received an abort request";
	IOservice_.stop();
}


void server::handleAccept( const boost::system::error_code& e )
{
	if ( !e )	{
		newConnection_->start();
		LOG_TRACE << "Received new connection";

		newConnection_.reset( new connection( IOservice_, requestHandler_, timeout_duration_ ));
		acceptor_.async_accept( newConnection_->socket(),
					 strand_.wrap( boost::bind( &server::handleAccept,
								    this,
								    boost::asio::placeholders::error )));
		LOG_TRACE << "Acceptor ready for new connection";
	}
}


// The server is stopped by closing the acceptor.
// When all outstanding operations are completed
// all calls to io_service::run() will return.
void server::handleStop()
{
	LOG_TRACE << "Closed acceptor for unencrypted connections";
}


} // namespace _SMERP
