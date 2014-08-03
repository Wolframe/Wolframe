//
// connectionHandler.cpp
//

#include "echoHandler.hpp"
#include "logger-v1.hpp"
#include "system/SSLcertificateInfo.hpp"

#include <string>
#include <cstring>

#include "boost/date_time/posix_time/posix_time.hpp"		// to print time_t structures

namespace _Wolframe {

echoConnection::echoConnection( const net::LocalEndpointR& local, unsigned short timeout )
{
	net::ConnectionEndpoint::ConnectionType type = local->type();

	switch ( type )	{
		case net::ConnectionEndpoint::UDP:
			LOG_FATAL << "UDP local connection type not implemented";
			abort();

		case net::ConnectionEndpoint::TCP:	{
			const net::LocalTCPendpoint* lcl = static_cast<const net::LocalTCPendpoint*>( local.get() );
			LOG_TRACE << "Created connection handler for " << lcl->toString();
			break;
		}
#ifdef WITH_SSL
		case net::ConnectionEndpoint::SSL:	{
			const net::LocalSSLendpoint* lcl = static_cast<const net::LocalSSLendpoint*>( local.get() );
			LOG_TRACE << "Created connection handler (SSL) for " << lcl->toString();
			break;
		}
#else
		case net::ConnectionEndpoint::SSL:
#endif // WITH_SSL
		default:
			LOG_FATAL << "Impossible local connection type !";
			abort();
	}

	m_state = NEW;
	m_dataStart = NULL;
	m_dataSize = 0;
	m_idleTimeout = timeout;
}


echoConnection::~echoConnection()
{
	LOG_TRACE << "Connection handler destroyed";
}


void echoConnection::setPeer( const net::RemoteEndpointR& remote )
{
	net::ConnectionEndpoint::ConnectionType type = remote->type();

	switch ( type )	{
		case net::ConnectionEndpoint::UDP:
			LOG_FATAL << "UDP local connection type not implemented";
			abort();

		case net::ConnectionEndpoint::TCP:	{
			const net::RemoteTCPendpoint* rmt = static_cast<const net::RemoteTCPendpoint*>( remote.get() );
			LOG_TRACE << "Peer set to " << rmt->toString() << ", connected at " << rmt->connectionTime();
			break;
		}
#ifdef WITH_SSL
		case net::ConnectionEndpoint::SSL:	{
			const net::RemoteSSLendpoint* rmt = static_cast<const net::RemoteSSLendpoint*>( remote.get() );
			LOG_TRACE << "Peer set to " << rmt->toString() << ", connected at " << boost::posix_time::from_time_t( rmt->connectionTime());
			if ( rmt->SSLcertInfo() )	{
				LOG_TRACE << "Peer SSL certificate serial number " << rmt->SSLcertInfo()->serialNumber()
					  << ", issued by: " << rmt->SSLcertInfo()->issuer();
				LOG_TRACE << "Peer SSL certificate valid from " << boost::posix_time::from_time_t( rmt->SSLcertInfo()->notBefore())
					  << " to " <<  boost::posix_time::from_time_t( rmt->SSLcertInfo()->notAfter());
				LOG_TRACE << "Peer SSL certificate subject: " << rmt->SSLcertInfo()->subject();
				LOG_TRACE << "Peer SSL certificate Common Name: " << rmt->SSLcertInfo()->commonName();
			}
			break;
		}
#else
		case net::ConnectionEndpoint::SSL:
#endif // WITH_SSL
		default:
			LOG_FATAL << "Impossible remote connection type !";
			abort();
	}
}


/// Handle a request and produce a reply.
const net::NetworkOperation echoConnection::nextOperation()
{
	switch( m_state )	{
	case NEW:	{
		m_state = HELLO_SENT;
		return net::NetworkOperation( net::SendString( "Welcome to Wolframe.\n" ));
	}

	case HELLO_SENT:	{
		m_state = READ_INPUT;
		return net::NetworkOperation( net::ReadData( m_readBuf, ReadBufSize, m_idleTimeout ));
	}

	case READ_INPUT:
		m_dataStart = m_readBuf;
		// Yes, it continues with OUTPUT_MSG, sneaky, sneaky, sneaky :P

	case OUTPUT_MSG:
		if ( !strncmp( "quit", m_dataStart, 4 ))	{
			m_state = TERMINATING;
			return net::NetworkOperation( net::SendString( "Thank you for using Wolframe.\n" ));
		}
		else	{
			char *s = m_dataStart;
			for ( std::size_t i = 0; i < m_dataSize; i++ )	{
				if ( *s == '\n' )	{
					s++;
					m_outMsg = std::string( m_dataStart, s - m_dataStart );
					m_dataSize -= s - m_dataStart;
					m_dataStart = s;
					m_state = OUTPUT_MSG;
					return net::NetworkOperation( net::SendString( m_outMsg ));
				}
				s++;
			}
			// If we got here, no \n was found, we need to read more
			// or close the connection if the buffer is full
			if ( m_dataSize >= ReadBufSize )	{
				m_state = TERMINATING;
				return net::NetworkOperation( net::SendString( "Line too long. Bye.\n" ));
			}
			else {
				memmove( m_readBuf, m_dataStart, m_dataSize );
				m_state = READ_INPUT;
				return net::NetworkOperation( net::ReadData( m_readBuf + m_dataSize,
									     ReadBufSize - m_dataSize,
									     m_idleTimeout ));
			}
		}

	case TIMEOUT_OCCURED:	{
		m_state = TERMINATING;
		return net::NetworkOperation( net::SendString( "Timeout. :P\n" ));
	}

	case SIGNALLED:	{
		m_state = TERMINATING;
		return net::NetworkOperation( net::SendString( "Server is shutting down. :P\n" ));
	}

	case TERMINATING:	{
		m_state = FINISHED;
		return net::NetworkOperation( net::CloseConnection() );
	}

	case FINISHED:
		LOG_DEBUG << "Processor in FINISHED state";
		break;

	} /* switch( state_ ) */
	return net::NetworkOperation( net::CloseConnection() );
}


/// Parse incoming data. The return value indicates how much of the
/// input has been consumed.
void echoConnection::networkInput( const void*, std::size_t bytesTransferred )
{
	m_dataSize += bytesTransferred;
}


void echoConnection::signalOccured( NetworkSignal signal )
{
	switch( signal )	{
		case TIMEOUT:
			m_state = TIMEOUT_OCCURED;
			LOG_TRACE << "Processor received timeout signal";
			break;

		case TERMINATE:
			m_state = SIGNALLED;
			LOG_TRACE << "Processor received terminate signal";
			break;

		case END_OF_FILE:
			LOG_TRACE << "Processor received EOF (read on closed connection)";
			m_state = TERMINATING;
			break;

		case BROKEN_PIPE:
			LOG_TRACE << "Processor received BROKEN PIPE (write on closed connection)";
			m_state = TERMINATING;
			break;

		case CONNECTION_RESET:
			LOG_TRACE << "Processor received CONNECTION RESET (connection closed by peer)";
			m_state = TERMINATING;
			break;

		case OPERATION_CANCELLED:
			LOG_TRACE << "Processor received OPERATION_CANCELED (should have been requested by us)";
			m_state = TERMINATING;
			break;

		case UNKNOWN_ERROR:
			LOG_TRACE << "Processor received an UNKNOWN error from the framework";
			m_state = TERMINATING;
			break;
	}
}


/// ServerHandler PIMPL
net::ConnectionHandler* ServerHandler::ServerHandlerImpl::newConnection( const net::LocalEndpointR& local )
{
	return new echoConnection( local, timeout );
}

/// Outside face of the PIMPL
ServerHandler::ServerHandler( const HandlerConfiguration *conf,
			      const module::ModulesDirectory* /*modules*/ )
	: m_impl( new ServerHandlerImpl( conf ) )	{}

ServerHandler::~ServerHandler()	{ delete m_impl; }

net::ConnectionHandler* ServerHandler::newConnection( const net::LocalEndpointR& local )
{
	return m_impl->newConnection( local );
}

} // namespace _Wolframe
