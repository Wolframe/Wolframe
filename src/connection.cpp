//
// connection.cpp
//

#include "connectionBase.hpp"
#include "connection.hpp"
#include "logger.hpp"
#include "requestHandler.hpp"
#include "connContext.hpp"

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

namespace _SMERP {

connection::connection( boost::asio::io_service& IOservice,
			connectionTimeout& timeouts,
			requestHandler& handler ) :
	connectionBase< boost::asio::ip::tcp::socket >( IOservice, timeouts, handler ),
	socket_( IOservice )
{
	LOG_TRACE << "New connection created";
}


void connection::start()
{
	identifier( std::string( socket().remote_endpoint().address().to_string())
		    + ":" + boost::lexical_cast<std::string>( socket().remote_endpoint().port() ));
	LOG_TRACE << "Starting connection to " << identifier();

	boost::asio::write( socket(), boost::asio::buffer( "Welcome to bla bla bla\n" ));
	LOG_TRACE << "Wrote welcome message to " << identifier();

	socket_.async_read_some( boost::asio::buffer( buffer_ ),
				 strand_.wrap( boost::bind( &connection::handleRead,
							    boost::static_pointer_cast<connection>( shared_from_this()),
							    boost::asio::placeholders::error,
							    boost::asio::placeholders::bytes_transferred )));
	setTimeout( connectionTimeout::TIMEOUT_IDLE );
}

} // namespace _SMERP
