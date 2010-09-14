//
// connection.cpp
//

#include "connectionBase.hpp"
#include "connection.hpp"
#include "logger.hpp"
#include "connectionHandler.hpp"

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

namespace _SMERP {

connection::connection( boost::asio::io_service& IOservice,
			connectionTimeout& timeouts,
			connectionHandler& handler ) :
	connectionBase< boost::asio::ip::tcp::socket >( IOservice, timeouts, handler ),
	socket_( IOservice )
{
	LOG_TRACE << "New connection created";
}

connection::~connection()
{
	LOG_TRACE << "Connection destroyed";
}


void connection::start()
{
	identifier( std::string( socket().remote_endpoint().address().to_string())
		    + ":" + boost::lexical_cast<std::string>( socket().remote_endpoint().port() ));
	LOG_TRACE << "Starting connection to " << identifier();

	networkOperation netOp = connectionHandler_.nextOperation();
	switch ( netOp.operation )	{
	case networkOperation::READ:
	case networkOperation::WRITE:
	case networkOperation::WRITE_TERMINATE:
	case networkOperation::TERMINATE:
		break;
	}

	boost::asio::write( socket(), boost::asio::buffer( "Welcome to bla bla bla\n" ));
	LOG_TRACE << "Wrote welcome message to " << identifier();

	socket_.async_read_some( boost::asio::buffer( buffer_ ),
				 strand_.wrap( boost::bind( &connection::handleRead,
							    boost::static_pointer_cast<connection>( shared_from_this()),
							    boost::asio::placeholders::error,
							    boost::asio::placeholders::bytes_transferred )));
	setTimeout( connectionTimeout::TIMEOUT_IDLE );
}


#ifdef WITH_SSL

SSLconnection::SSLconnection( boost::asio::io_service& IOservice,
			      boost::asio::ssl::context& SSLcontext,
			      connectionTimeout& timeouts,
			      connectionHandler& handler ) :
	connectionBase< ssl_socket >( IOservice, timeouts, handler ),
	SSLsocket_( IOservice, SSLcontext )
{
	LOG_TRACE << "New SSL connection created";
}

SSLconnection::~SSLconnection()
{
	LOG_TRACE << "SSL connection destroyed";
}

void SSLconnection::start()
{
	identifier( std::string( SSLsocket_.lowest_layer().remote_endpoint().address().to_string())
		    + ":" + boost::lexical_cast<std::string>( SSLsocket_.lowest_layer().remote_endpoint().port() )
		    + " (SSL)");
	LOG_TRACE << "Starting connection to " << identifier();
	SSLsocket_.async_handshake( boost::asio::ssl::stream_base::server,
				    strand_.wrap( boost::bind( &SSLconnection::handleHandshake,
							       boost::static_pointer_cast<SSLconnection>( shared_from_this() ),
							       boost::asio::placeholders::error )));
}


void SSLconnection::handleHandshake( const boost::system::error_code& e )
{
	LOG_DATA << "SSL handshake to " << identifier();
	if ( !e )	{
		boost::asio::write( socket(), boost::asio::buffer( "Welcome to SSL bla bla bla\n" ));
		LOG_TRACE << "Wrote welcome message to " << identifier();

		socket().async_read_some( boost::asio::buffer( buffer_ ),
					    strand_.wrap( boost::bind( &SSLconnection::handleRead,
								       boost::static_pointer_cast<SSLconnection>( shared_from_this() ),
								       boost::asio::placeholders::error,
								       boost::asio::placeholders::bytes_transferred )));
		setTimeout( connectionTimeout::TIMEOUT_IDLE );
	}
	else	{
		LOG_DEBUG << "ERROR handling SSL handshake from " << identifier();
//		delete this;
		boost::system::error_code ignored_ec;
		socket().lowest_layer().shutdown( boost::asio::ip::tcp::socket::shutdown_both, ignored_ec );
	}
}

#endif // WITH_SSL

} // namespace _SMERP
