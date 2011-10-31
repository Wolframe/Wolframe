/************************************************************************

 Copyright (C) 2011 Project Wolframe.
 All rights reserved.

 This file is part of Project Wolframe.

 Commercial Usage
    Licensees holding valid Project Wolframe Commercial licenses may
    use this file in accordance with the Project Wolframe
    Commercial License Agreement provided with the Software or,
    alternatively, in accordance with the terms contained
    in a written agreement between the licensee and Project Wolframe.

 GNU General Public License Usage
    Alternatively, you can redistribute this file and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Wolframe is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wolframe.  If not, see <http://www.gnu.org/licenses/>.

 If you have questions regarding the use of this file, please contact
 Project Wolframe.

************************************************************************/
//
// connectionHandler.cpp
//

#include "wolframeHandler.hpp"
#include "logger-v1.hpp"
#include "standardConfigs.hpp"
#include "handlerConfig.hpp"

#ifdef WITH_SSL
#include "SSLcertificateInfo.hpp"
#endif

#include <string>
#include <cstring>
#include <stdexcept>

#include "boost/date_time/posix_time/posix_time.hpp"		// to print time_t structures

namespace _Wolframe	{

wolframeConnection::wolframeConnection( const WolframeHandler& context,
					const net::LocalEndpoint& local )
	: m_globalCtx( context ),
	  m_readBuf( 16536 )
{
	m_localEP = &local;
	m_remoteEP = NULL;
	net::ConnectionEndpoint::ConnectionType type = m_localEP->type();

	switch ( type )	{
	case net::ConnectionEndpoint::TCP_CONNECTION:	{
		const net::LocalTCPendpoint* lcl = static_cast< const net::LocalTCPendpoint* >( m_localEP );
		LOG_TRACE << "Created connection handler for " << lcl->toString();
		break;
	}
#ifdef WITH_SSL
	case net::ConnectionEndpoint::SSL_CONNECTION:	{
		const net::LocalSSLendpoint* lcl = static_cast< const net::LocalSSLendpoint* >( m_localEP );
		LOG_TRACE << "Created connection handler (SSL) for " << lcl->toString();
		break;
	}
#else
	case net::ConnectionEndpoint::SSL_CONNECTION:
#endif // WITH_SSL
	default:
		LOG_FATAL << "Impossible local connection type !";
		abort();
	}

	m_state = NEW_CONNECTION;
	m_dataStart = NULL;
	m_dataSize = 0;

	// Initialize various channel processors to start-up values
	m_authentication = NULL;
}


wolframeConnection::~wolframeConnection()
{
	if ( m_authentication )	{
		m_authentication->close();
		m_authentication = NULL;
	}

	LOG_TRACE << "Connection handler destroyed";
}


void wolframeConnection::setPeer( const net::RemoteEndpoint& remote )
{
	m_remoteEP = &remote;
	net::ConnectionEndpoint::ConnectionType type = m_remoteEP->type();

	switch ( type )	{
	case net::ConnectionEndpoint::TCP_CONNECTION:	{
		const net::RemoteTCPendpoint* rmt = static_cast< const net::RemoteTCPendpoint* >( m_remoteEP );
		LOG_TRACE << "Peer set to " << rmt->toString() << ", connected at " << rmt->connectionTime();
		break;
	}

	case net::ConnectionEndpoint::SSL_CONNECTION:
#ifdef WITH_SSL
	{
		const net::RemoteSSLendpoint* rmt = static_cast<const net::RemoteSSLendpoint*>( m_remoteEP );
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
#endif // WITH_SSL

	default:
		LOG_FATAL << "Impossible remote connection type !";
		abort();
	}

// Check here if the connection is allowed

// The AAAA stuf should also depend on peer properties
//	m_authentication = m_globalCtx.aaaa().authenticationChannel();
//	m_authorization = m_globalCtx.aaaa().authorizationChannel();
//	m_audit = m_globalCtx.aaaa().auditChannel();
//	m_accounting = m_globalCtx.aaaa().accountingChannel();
}

/// Handle a request and produce a reply.
const net::NetworkOperation wolframeConnection::nextOperation()
{
	switch( m_state )	{
	case NEW_CONNECTION:	{
		m_state = SEND_HELLO;
		if ( ! m_globalCtx.banner().empty() )
			m_outMsg = m_globalCtx.banner() + "\nOK\n";
		else
			m_outMsg = "OK\n";
		return net::NetworkOperation( net::SendString( m_outMsg ));
	}

	case SEND_HELLO:	{
		m_state = READ_INPUT;
		return net::NetworkOperation( net::ReadData( m_readBuf.ptr(), m_readBuf.size(), 30 ));
	}

	case READ_INPUT:
		m_dataStart = m_readBuf.charptr();
		// Yes, it continues with OUTPUT_MSG, sneaky, sneaky, sneaky :P

	case OUTPUT_MSG:
		if ( !strncmp( "quit", m_dataStart, 4 ))	{
			m_state = TERMINATE;
			return net::NetworkOperation( net::SendString( "Bye\n" ));
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
			if ( m_dataSize >= m_readBuf.size() )	{
				m_state = TERMINATE;
				return net::NetworkOperation( net::SendString( "Line too long. Bye.\n" ));
			}
			else {
				memmove( m_readBuf.ptr(), m_dataStart, m_dataSize );
				m_state = READ_INPUT;
				// Aba: Windows: wolframeHandler.cpp(194) : error C2036: 'void *' : unknown size,
				// fixed temporarily with an ugly cast. Please check.
				return net::NetworkOperation( net::ReadData( (char *)m_readBuf.ptr() + m_dataSize,
									     m_readBuf.size() - m_dataSize,
									     30 ));
			}
		}

	case TIMEOUT:	{
		m_state = TERMINATE;
		return net::NetworkOperation( net::SendString( "Timeout. :P\n" ));
	}

	case SIGNALLED:	{
		m_state = TERMINATE;
		return net::NetworkOperation( net::SendString( "Server is shutting down. :P\n" ));
	}

	case TERMINATE:	{
		m_state = FINISHED;
		return net::NetworkOperation( net::CloseConnection() );
	}

	case FINISHED:
		LOG_DEBUG << "Processor in FINISHED state";
		break;

	} /* switch( m_state ) */

	LOG_ALERT << "Connection FSM out of states";
	return net::NetworkOperation( net::CloseConnection() );
}


/// Parse incoming data..
void wolframeConnection::networkInput( const void*, std::size_t bytesTransferred )
{
	LOG_DATA << "network Input: Read " << bytesTransferred << " bytes";
	m_dataSize += bytesTransferred;
}

void wolframeConnection::timeoutOccured()
{
	m_state = TIMEOUT;
	LOG_TRACE << "Processor received timeout";
}

void wolframeConnection::signalOccured()
{
	m_state = SIGNALLED;
	LOG_TRACE << "Processor received signal";
}

void wolframeConnection::errorOccured( NetworkSignal signal )
{
	switch( signal )	{
	case END_OF_FILE:
		LOG_TRACE << "Processor received EOF (read on closed connection)";
		break;

	case BROKEN_PIPE:
		LOG_TRACE << "Processor received BROKEN PIPE (write on closed connection)";
		break;

	case OPERATION_CANCELLED:
		LOG_TRACE << "Processor received OPERATION CANCELED (should have been requested by us)";
		break;

	case UNKNOWN_ERROR:
		LOG_TRACE << "Processor received an UNKNOWN error from the framework";
		break;
	}
	m_state = TERMINATE;
}


/// The server handler global context
WolframeHandler::WolframeHandler( const HandlerConfiguration* conf,
				  const module::ModulesDirectory* modules )
	: m_banner( conf->bannerCfg->toString() ),
	  m_db( conf->databaseCfg, modules ),
	  m_aaaa( conf->aaaaCfg, modules ),
	  m_proc( conf->procCfg, modules )
{
	LOG_TRACE << "Global context: banner: <" << m_banner << ">";
	if ( ! m_aaaa.resolveDB( m_db ) )	{
		LOG_FATAL << "Cannot resolve database references for AAAA services";
		throw( std::invalid_argument( "WolframeHandler AAAA: unresolved database reference" ));
	}
	LOG_TRACE << "AAAA database references resolved";
	if ( ! m_proc.resolveDB( m_db ) )	{
		LOG_FATAL << "Cannot resolve database reference for processor group";
		throw( std::invalid_argument( "WolframeHandler processor: unresolved database reference" ));
	}
	LOG_TRACE << "Processor group database reference resolved";
}

WolframeHandler::~WolframeHandler()
{
	LOG_TRACE << "Global Wolframe handler / context destroyed";
}


/// ServerHandler PIMPL
net::ConnectionHandler* ServerHandler::ServerHandlerImpl::newConnection( const net::LocalEndpoint& local )
{
	return new wolframeConnection( m_globalContext, local );
}

ServerHandler::ServerHandlerImpl::ServerHandlerImpl( const HandlerConfiguration* conf,
						     const module::ModulesDirectory* modules )
	: m_globalContext( conf, modules )			{}

ServerHandler::ServerHandlerImpl::~ServerHandlerImpl()	{}

/// Outside face of the PIMPL
ServerHandler::ServerHandler( const HandlerConfiguration* conf,
			      const module::ModulesDirectory* modules )
	: m_impl( new ServerHandlerImpl( conf, modules ) )	{}

ServerHandler::~ServerHandler()	{ delete m_impl; }

net::ConnectionHandler* ServerHandler::newConnection( const net::LocalEndpoint& local )
{
	return m_impl->newConnection( local );
}

} // namespace _Wolframe
