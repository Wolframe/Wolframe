//
// server.cpp
//

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include "server.hpp"
#include "acceptor.hpp"
#include "logger.hpp"
#include "connectionHandler.hpp"


namespace _SMERP {

server::server( const ApplicationConfiguration& config, const ServerHandler& serverHandler )
	: threadPoolSize_( config.threads ),
	IOservice_(),
	timeouts_( (unsigned long)config.idleTimeout,
		   (unsigned long)config.requestTimeout,
		   (unsigned long)config.processTimeout,
		   (unsigned long)config.answerTimeout )
{
	size_t	i;
	for ( i = 0; i < config.address.size(); i++ )	{
		acceptor* acptr = new acceptor( IOservice_,
						config.address[i].host, config.address[i].port,
						timeouts_, serverHandler );
		acceptor_.push_back( acptr );
	}
	LOG_DEBUG << i << " network acceptor(s) created.";
#ifdef WITH_SSL
	for ( i = 0; i < config.SSLaddress.size(); i++ )	{
		SSLacceptor* acptr = new SSLacceptor( IOservice_,
						      config.SSLaddress[i].certFile, config.SSLaddress[i].keyFile,
						      config.SSLaddress[i].verify,
						      config.SSLaddress[i].CAchainFile, config.SSLaddress[i].CAdirectory,
						      config.SSLaddress[i].host, config.SSLaddress[i].port,
						      timeouts_, serverHandler );
		SSLacceptor_.push_back( acptr );
	}
	LOG_DEBUG << i << " network SSL acceptor(s) created.";
#endif // WITH_SSL
}


server::~server()
{
	LOG_TRACE << "Server destructor called";

	std::size_t	i;
	for ( i = 0; i < acceptor_.size(); i++ )
		delete acceptor_[i];
	LOG_TRACE << i << " acceptor(s) deleted";
#ifdef WITH_SSL
	for ( i = 0; i < SSLacceptor_.size(); i++ )
		delete SSLacceptor_[i];
	LOG_TRACE << i << " SSL acceptor(s) deleted";
#endif // WITH_SSL
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
	LOG_DEBUG << i << " acceptor(s) signaled to stop";
#ifdef WITH_SSL
	for ( i = 0; i < SSLacceptor_.size(); i++ )
		SSLacceptor_[i]->stop();
	LOG_DEBUG << i << " SSL acceptor(s) signaled to stop";
#endif // WITH_SSL
}


// Stop io_services the hard way.
void server::abort()
{
	LOG_DEBUG << "Network server received an abort request";
	IOservice_.stop();
}

} // namespace _SMERP
