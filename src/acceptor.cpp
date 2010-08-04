//
// acceptor.cpp
//

#include "acceptor.hpp"
#include "connection.hpp"
#include "connectionTimeout.hpp"
#include "logger.hpp"

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>


namespace _SMERP {

acceptor::acceptor( boost::asio::io_service& IOservice,
		    const std::string& host, const unsigned short port,
		    connectionTimeout& timeouts, requestHandler& reqHandler) :
	IOservice_( IOservice ),
	strand_( IOservice_ ),
	acceptor_( IOservice_ ),
	timeouts_( timeouts ),
	requestHandler_( reqHandler )
{
	// Open the acceptor(s) with the option to reuse the address (i.e. SO_REUSEADDR).
	boost::asio::ip::tcp::resolver resolver( IOservice_ );
	boost::asio::ip::tcp::resolver::query query( host, "");
	boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve( query );
	endpoint.port( port );

	newConnection_ = connection_ptr( new connection( IOservice_, timeouts_, requestHandler_ ));

	acceptor_.open( endpoint.protocol() );
	acceptor_.set_option( boost::asio::ip::tcp::acceptor::reuse_address( true ));
	acceptor_.bind( endpoint );
	acceptor_.listen();
	acceptor_.async_accept( newConnection_->socket(),
				strand_.wrap( boost::bind( &acceptor::handleAccept,
							   this,
							   boost::asio::placeholders::error )));
	identifier_ = acceptor_.local_endpoint().address().to_string()
		      + ":" + boost::lexical_cast<std::string>( acceptor_.local_endpoint().port() );
	LOG_INFO << "Accepting connections on " << identifier_;
}


acceptor::~acceptor()
{
	LOG_TRACE << "Acceptor destructor called for " << identifier_;
}


void acceptor::handleAccept( const boost::system::error_code& e )
{
	if ( !e )	{
		newConnection_->start();
		LOG_DEBUG << "Received new connection on " << identifier_;

		newConnection_.reset( new connection( IOservice_, timeouts_, requestHandler_ ));
		acceptor_.async_accept( newConnection_->socket(),
					strand_.wrap( boost::bind( &acceptor::handleAccept,
								   this,
								   boost::asio::placeholders::error )));
		LOG_DATA << "Acceptor " << identifier_ << " ready for new connection";
	}
}


void acceptor::stop()
{
	LOG_TRACE << "Acceptor for " << identifier_ << " received a shutdown request";
	// Post a call to the stop function so that acceptor::stop() is safe to call
	// from any thread.
	IOservice_.post( strand_.wrap( boost::bind( &acceptor::handleStop, this )));
}


// The server is stopped by closing the acceptor.
// When all outstanding operations are completed
// all calls to io_service::run() will return.
void acceptor::handleStop()
{
	acceptor_.close();
	LOG_DEBUG << "Closed acceptor for " << identifier_;
}


} // namespace _SMERP
