//
// server.cpp
//

#include "server.hpp"
#include "logger.hpp"

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>


namespace _SMERP {

server::server( const ApplicationConfiguration& config )
	: threadPoolSize_( config.threads ),
	IOservice_(),
	timeouts_( (unsigned long)config.idleTimeout,
		   (unsigned long)config.requestTimeout,
		   (unsigned long)config.processTimeout,
		   (unsigned long)config.answerTimeout ),
	requestHandler_()
{
	size_t	i;
	for ( i = 0; i < config.address.size(); i++ )	{
		acceptor* acptr = new acceptor( IOservice_,
						config.address[i].host, config.address[i].port,
						timeouts_, requestHandler_ );
		acceptor_.push_back( acptr );
	}
	LOG_DEBUG << i << " unencrypted network acceptor(s) created.";
}


server::~server()
{
	LOG_TRACE << "Server destructor called";

	std::size_t	i;
	for ( i = 0; i < acceptor_.size(); i++ )
		delete acceptor_[i];
	LOG_TRACE << i << " acceptor(s) deleted";
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
	LOG_TRACE << i << " network server thread(s) started";

	// Wait for all threads in the pool to exit.
	for ( i = 0; i < threads.size(); ++i )
		threads[i]->join();

	// Reset io_services.
	IOservice_.reset();
}


void server::stop()
{
	LOG_DEBUG << "Network server received a shutdown request";

	std::size_t	i;
	for ( i = 0; i < acceptor_.size(); i++ )
		acceptor_[i]->stop();
	LOG_DEBUG << i << " unencrypted acceptor(s) signaled to stop";
}


// Stop io_services the hard way.
void server::abort()
{
	LOG_DEBUG << "Network server received an abort request";
	IOservice_.stop();
}

} // namespace _SMERP
