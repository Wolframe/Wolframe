//
// server.cpp
//

#include "server.hpp"
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>

namespace _SMERP {

server::server(const std::string& address, const std::string& port, std::size_t thread_pool_size,
    long timeout_duration_ms)
  : thread_pool_size_(thread_pool_size),
    timeout_duration_ms_(timeout_duration_ms),
    strand_(io_service_),
    acceptor_(io_service_),
    new_connection_(new connection(io_service_, requestHandler_, timeout_duration_ms_)),
    requestHandler_()
{
  // Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
  boost::asio::ip::tcp::resolver resolver(io_service_);
  boost::asio::ip::tcp::resolver::query query(address, port);
  boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
  acceptor_.open(endpoint.protocol());
  acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
  acceptor_.bind(endpoint);
  acceptor_.listen();
  acceptor_.async_accept(new_connection_->socket(),
      strand_.wrap(
        boost::bind(&server::handle_accept, this,
          boost::asio::placeholders::error)));
}

void server::run()
{
  // Create a pool of threads to run all of the io_services.
  std::vector<boost::shared_ptr<boost::thread> > threads;
  for (std::size_t i = 0; i < thread_pool_size_; ++i)
  {
    boost::shared_ptr<boost::thread> thread(new boost::thread(
          boost::bind(&boost::asio::io_service::run, &io_service_)));
    threads.push_back(thread);
  }

  // Wait for all threads in the pool to exit.
  for (std::size_t i = 0; i < threads.size(); ++i)
    threads[i]->join();

  // Reset io_services.
  io_service_.reset();
}

void server::stop()
{
  // Post a call to the stop function so that server::stop() is safe to call
  // from any thread.
  io_service_.post(
      strand_.wrap(
        boost::bind(&server::handle_stop, this)));
}


// Stop io_services the hard way.
void server::abort()
{
	io_service_.stop();
}


void server::handle_accept(const boost::system::error_code& e)
{
  if (!e)
  {
    new_connection_->start();
    new_connection_.reset(new connection(io_service_, requestHandler_,
      timeout_duration_ms_));
    acceptor_.async_accept(new_connection_->socket(),
        strand_.wrap(
          boost::bind(&server::handle_accept, this,
            boost::asio::placeholders::error)));
  }
}


// The server is stopped by closing the acceptor.
// When all outstanding operations are completed
// all calls to io_service::run() will return.
void server::handle_stop()
{
	acceptor_.close();
}


} // namespace _SMERP
