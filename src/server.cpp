//
// server.cpp
//

#include "server.hpp"
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>

#include <iostream>

namespace _SMERP {

server::server( const ApplicationConfiguration& config )
	: threadPoolSize_( config.threads ),
    timeout_duration_ms_((unsigned long)config.idleTimeout * 1000 ),
	strand_( IOservice_ ),
	acceptor_( IOservice_ ),
    new_connection_(new connection(IOservice_, requestHandler_, timeout_duration_ms_)),
    requestHandler_()
{
	// Open the acceptor(s) with the option to reuse the address (i.e. SO_REUSEADDR).
	boost::asio::ip::tcp::resolver resolver( IOservice_ );
	boost::asio::ip::tcp::resolver::query query( config.address[0].first, "");
	boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve( query );
	endpoint.port( config.address[0].second );
	acceptor_.open( endpoint.protocol() );
	acceptor_.set_option( boost::asio::ip::tcp::acceptor::reuse_address( true ));
	acceptor_.bind( endpoint );
	acceptor_.listen();
	acceptor_.async_accept( new_connection_->socket(), strand_.wrap( boost::bind( &server::handleAccept,
										      this,
										      boost::asio::placeholders::error )));
}


void server::run()
{
	// Create a pool of threads to run all of the io_services.
	std::vector<boost::shared_ptr<boost::thread> > threads;
	for ( std::size_t i = 0; i < threadPoolSize_; ++i )	{
		boost::shared_ptr<boost::thread> thread( new boost::thread( boost::bind( &boost::asio::io_service::run, &IOservice_ )));
		threads.push_back( thread );
	}

	// Wait for all threads in the pool to exit.
	for ( std::size_t i = 0; i < threads.size(); ++i )
		threads[i]->join();

	// Reset io_services.
	IOservice_.reset();
}


void server::stop()
{
	// Post a call to the stop function so that server::stop() is safe to call
	// from any thread.
	IOservice_.post( strand_.wrap( boost::bind( &server::handleStop, this )));
}


// Stop io_services the hard way.
void server::abort()
{
	IOservice_.stop();
}


void server::handleAccept(const boost::system::error_code& e)
{
  if (!e)
  {
    new_connection_->start();
    new_connection_.reset(new connection(IOservice_, requestHandler_,
      timeout_duration_ms_));
    acceptor_.async_accept(new_connection_->socket(),
        strand_.wrap(
	  boost::bind(&server::handleAccept, this,
            boost::asio::placeholders::error)));
  }
}


// The server is stopped by closing the acceptor.
// When all outstanding operations are completed
// all calls to io_service::run() will return.
void server::handleStop()
{
	acceptor_.close();
}


} // namespace _SMERP
