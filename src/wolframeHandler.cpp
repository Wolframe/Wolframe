/************************************************************************

 Copyright (C) 2011 - 2014 Project Wolframe.
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
#include "prgbind/programLibrary.hpp"
#include "logger-v1.hpp"
#include "standardConfigs.hpp"
#include "handlerConfig.hpp"
#include <stdexcept>

#ifdef WITH_SSL
#include "system/SSLcertificateInfo.hpp"
#endif

#include <string>
#include <cstring>
#include <stdexcept>

#include "boost/date_time/posix_time/posix_time.hpp"		// to print time_t structures

namespace _Wolframe	{

wolframeConnection::wolframeConnection( const WolframeHandler& context,
					const net::LocalEndpointR& local )
	: m_globalCtx( context ),
	  m_input(0), m_inputsize(16536), m_inputpos(0), m_output(0), m_outputsize( 4096 ), m_execContext( &context.proc(), &context.aaaa())
{
	m_input = std::malloc( m_inputsize);
	m_output = std::malloc( m_outputsize);
	if (!m_input || !m_output)
	{
		if (m_input) std::free( m_input);
		if (m_output) std::free( m_output);
		throw std::bad_alloc();
	}
	net::ConnectionEndpoint::ConnectionType type = local->type();
	std::string protocol;
	if (local->config().protocol.empty())
	{
		protocol = "standard";
	}
	else
	{
		protocol = local->config().protocol;
	}
	m_protocolHandler.reset( m_execContext.provider()->protocolHandler( protocol));
	if (!m_protocolHandler.get())
	{
		LOG_FATAL << "protocol '" << protocol << "' is not known (protocol module not loaded)";
		abort();
	}
	switch ( type )	{
		case net::ConnectionEndpoint::UDP:
			LOG_FATAL << "UDP local connection type not implemented";
			abort();

		case net::ConnectionEndpoint::TCP:	{
			m_localEP = local;
			LOG_TRACE << "Created connection handler for " << m_localEP->toString();
			break;
		}
#ifdef WITH_SSL
		case net::ConnectionEndpoint::SSL:	{
			m_localEP = local;
			LOG_TRACE << "Created connection handler (SSL) for " << m_localEP->toString();
			break;
		}
#else
		case net::ConnectionEndpoint::SSL:
#endif // WITH_SSL
		default:
			LOG_FATAL << "Impossible local connection type !";
			abort();
	}

	// Initialize channel data to start-up values
	m_state = NEW_CONNECTION;
	m_dataStart = NULL;
	m_dataSize = 0;

	m_authentication = NULL;
	m_authorization = NULL;
	m_audit = NULL;
	m_protocolHandler->setInputBuffer( m_input, m_inputsize);
	m_protocolHandler->setOutputBuffer( m_output, m_outputsize, 0);
	m_protocolHandler->setExecContext( &m_execContext);
	m_protocolHandler->setLocalEndPoint( m_localEP);
}


wolframeConnection::~wolframeConnection()
{
	if ( m_authorization )	{
		m_authorization->close();
		m_authorization = NULL;
	}
	if ( m_authentication )	{
		m_authentication->dispose();
		m_authentication = NULL;
	}
	if ( m_audit )	{
		m_audit->close();
		m_audit = NULL;
	}
	if (m_input) std::free( m_input);
	if (m_output) std::free( m_output);

	LOG_TRACE << "Connection handler destroyed";
}


void wolframeConnection::setPeer( const net::RemoteEndpointR& remote )
{
	net::ConnectionEndpoint::ConnectionType type = remote->type();

	switch ( type )	{
		case net::ConnectionEndpoint::UDP:
			LOG_FATAL << "UDP local connection type not implemented";
			abort();

		case net::ConnectionEndpoint::TCP:	{
			m_remoteEP = remote;
			LOG_TRACE << "Peer set to " << m_remoteEP->toString() << ", connected at "
				  << boost::posix_time::from_time_t( m_remoteEP->connectionTime());
			break;
		}

		case net::ConnectionEndpoint::SSL:
#ifdef WITH_SSL
		{
			m_remoteEP = remote;
			const net::RemoteSSLendpoint* rmt = dynamic_cast<const net::RemoteSSLendpoint*>( remote.get() );

			LOG_TRACE << "Peer set to " << m_remoteEP->toString() << ", connected at " << boost::posix_time::from_time_t( m_remoteEP->connectionTime());
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
	// Propagate setPeer to the command handler
	m_protocolHandler->setPeer( m_remoteEP);

	// Check if the connection is allowed
	if (( m_authorization = m_globalCtx.aaaa().authorizer()))	{
		if ( m_authorization->allowed( AAAA::ConnectInfo( *m_localEP, *m_remoteEP )))	{
			LOG_DEBUG << "Connection from " << m_remoteEP->toString()
				  << " to " << m_localEP->toString() << " authorized";
			m_execContext.setAuthorizer( m_authorization);

			if (!m_execContext.checkAuthorization( proc::ExecContext::CONNECT))
			{
				LOG_DEBUG << "Connection from " << m_remoteEP->toString()
					  << " to " << m_localEP->toString() << " not authorized (CONNECT)";
				// close the connection
				m_state = FORBIDDEN;
			}
		}
		else	{
			LOG_DEBUG << "Connection from " << m_remoteEP->toString()
				  << " to " << m_localEP->toString() << " not authorized";
			// close the connection
			m_state = FORBIDDEN;
		}
	}
	else	{
		LOG_WARNING << "Authorization not available";
		//		abort();
	}
}

static void logNetwork( const char* title, const void* ptr, std::size_t size)
{
	static const char hex[17] = "0123456789abcdef";
	std::size_t ii,blkidx;

	if (_Wolframe::log::LogBackend::instance().minLogLevel() <= _Wolframe::log::LogLevel::LOGLEVEL_DATA)
	{
		std::size_t nn = size>60?60:size;
		std::string str;
		for (ii=0; ii<nn; ++ii)
		{
			unsigned char ch = ((const unsigned char*)ptr)[ii];
			if (ch >= ' ' && ch <= 127)
			{
				str.push_back( ch);
			}
			else
			{
				str.push_back( '.');
			}
		}
		LOG_DATA << "[" << title << "] " << str << ((size>nn)?"...":"");
	}
	if (_Wolframe::log::LogBackend::instance().minLogLevel() <= _Wolframe::log::LogLevel::LOGLEVEL_DATA2)
	{
		for (ii=0,blkidx=0; ii<size; ii+=16,++blkidx)
		{
			std::string hexblk;
			std::string chrblk;
			std::size_t ci,ce;
			std::size_t eidx;
			for (ci = 0, ce = ((size-ii) >= 16)?16:(size-ii), eidx = 1; ci < ce; ++ci,++eidx)
			{
				unsigned char ch = *((const unsigned char*)ptr + ii + ci);
				if (ch >= ' ' && ch <= 127)
				{
					chrblk.push_back( ch);
				}
				else
				{
					chrblk.push_back( '.');
				}
				hexblk.push_back( hex[ ch / 16]);
				hexblk.push_back( hex[ ch % 16]);
				if ((eidx & 1) == 0) hexblk.push_back( ' ');
			}
			for (eidx=1; ci < 16; ++ci,++eidx)
			{
				chrblk.push_back( ' ');
				hexblk.push_back( ' ');
				hexblk.push_back( ' ');
				if ((eidx & 1) == 0) hexblk.push_back( ' ');
			}
			std::string idxstr;
			idxstr.push_back( hex[ (blkidx % 256) / 16]);
			idxstr.push_back( hex[ (blkidx % 256) % 16]);

			LOG_DATA2 << "[" << title << " " << idxstr << "] " << chrblk << " - " << hexblk;
		}
	}
}

/// Handle a request and produce a reply.
const net::NetworkOperation wolframeConnection::nextOperation()
{
	for ( ;; )	{
		if (m_state != COMMAND_HANDLER)
		{
			LOG_TRACE << "STATE wolframeConnection handler " << stateName( m_state );
		}
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
				m_state = COMMAND_HANDLER;
				return net::NetworkOperation( net::ReadData( m_input, m_inputsize, 30 ));
			}

			case TIMEOUT_OCCURED:	{
				m_state = TERMINATING;
				return net::NetworkOperation( net::SendString( m_endDataSessionMarker + "BYE Timeout. :P\n" ));
			}

			case SIGNALLED:	{
				if ( m_state == TERMINATING )	{
					return net::NetworkOperation( net::NoOp() );
				}
				else	{
					m_state = TERMINATING;
					return net::NetworkOperation( net::SendString( m_endDataSessionMarker + "BYE Server is shutting down. :P\n" ));
				}
			}

			case FORBIDDEN:	{
				m_state = TERMINATING;
				return net::NetworkOperation( net::SendString( "BYE Access denied.\n" ));
			}

			case TERMINATING:	{
				m_state = FINISHED;
				return net::NetworkOperation( net::CloseConnection() );
			}

			case FINISHED:
				LOG_DEBUG << "Processor in FINISHED state";
				return net::NetworkOperation( net::NoOp() );

			case COMMAND_HANDLER:	{
				void* inpp;
				std::size_t inppsize;
				const void* outpp;
				std::size_t outppsize;

				switch( m_protocolHandler->nextOperation() )	{
					case cmdbind::ProtocolHandler::READ:
						m_protocolHandler->getInputBlock( inpp, inppsize);
						return net::ReadData( inpp, inppsize);

					case cmdbind::ProtocolHandler::WRITE:
						m_protocolHandler->getOutput( outpp, outppsize);
						if ( _Wolframe::log::LogBackend::instance().minLogLevel() <= _Wolframe::log::LogLevel::LOGLEVEL_DATA)
						{
							logNetwork( "write", outpp, outppsize);
						}
						return net::SendData( outpp, outppsize);

					case cmdbind::ProtocolHandler::CLOSE:
						m_state = FINISHED;
						return net::NetworkOperation( net::CloseConnection());
				}
			}
			default:
				LOG_ALERT << "Internal: Processor in illegal state << (" << (int)__LINE__ << ")";
				return net::NetworkOperation( net::CloseConnection() );
		} /* switch( m_state ) */
	} /* for (;;) */
}


/// Parse incoming data..
void wolframeConnection::networkInput( const void* begin, std::size_t bytesTransferred )
{
	LOG_DATA << "network input got " << bytesTransferred << " bytes";
	if ( _Wolframe::log::LogBackend::instance().minLogLevel() <= _Wolframe::log::LogLevel::LOGLEVEL_DATA)
	{
		logNetwork( "read", begin, bytesTransferred);
	}
	if ( m_state == COMMAND_HANDLER )	{
		m_protocolHandler->putInput( begin, bytesTransferred );
	}
	else	{
		m_dataSize += bytesTransferred;
	}
}


void wolframeConnection::signalOccured( NetworkSignal signal )
{
	switch( signal )	{
		case TERMINATE:
			LOG_TRACE << "Processor received termination signal";
			if ( m_state == COMMAND_HANDLER )
			{
				m_endDataSessionMarker = m_protocolHandler->interruptDataSessionMarker();
			}
			if ( m_state != TERMINATING && m_state != FINISHED )
				m_state = SIGNALLED;
			break;

		case TIMEOUT:
			LOG_TRACE << "Processor received timeout signal";
			if ( m_state == COMMAND_HANDLER )
			{
				m_endDataSessionMarker = m_protocolHandler->interruptDataSessionMarker();
			}
			if ( m_state != TERMINATING && m_state != FINISHED )
				m_state = TIMEOUT_OCCURED;
			break;

		case END_OF_FILE:
			LOG_TRACE << "Processor received EOF (read on closed connection)";
			if ( m_state != TERMINATING && m_state != FINISHED )
				m_state = TERMINATING;
			break;

		case BROKEN_PIPE:
			LOG_TRACE << "Processor received BROKEN PIPE (write on closed connection)";
			if ( m_state != TERMINATING && m_state != FINISHED )
				m_state = TERMINATING;
			break;

		case CONNECTION_RESET:
			LOG_TRACE << "Processor received CONNECTION RESET (connection closed by peer)";
			if ( m_state != TERMINATING && m_state != FINISHED )
				m_state = TERMINATING;
			break;

		case OPERATION_CANCELLED:
			LOG_TRACE << "Processor received OPERATION CANCELED (should have been requested by us)";
			if ( m_state != TERMINATING && m_state != FINISHED )
				m_state = TERMINATING;
			break;

		case UNKNOWN_ERROR:
			if ( m_state == COMMAND_HANDLER )
			{
				m_endDataSessionMarker = m_protocolHandler->interruptDataSessionMarker();
			}
			LOG_TRACE << "Processor received an UNKNOWN error from the framework";
			if ( m_state != TERMINATING && m_state != FINISHED )
				m_state = TERMINATING;
			break;
	}
}


/// The server handler global context
WolframeHandler::WolframeHandler( const HandlerConfiguration* conf,
				  const module::ModulesDirectory* modules )
	: m_banner( conf->bannerCfg->toString() ),
	  m_db( conf->databaseCfg, modules ),
	  m_aaaa( conf->aaaaCfg, modules ),
	  m_proc( conf->procCfg, modules, &m_prglib)
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

	if (!m_proc.loadPrograms())
	{
		LOG_FATAL << "Not all programs could be loaded";
		throw std::runtime_error( "Not all programs could be loaded" );
	}
}

WolframeHandler::~WolframeHandler()
{
	LOG_TRACE << "Global Wolframe handler / context destroyed";
}


/// ServerHandler PIMPL
net::ConnectionHandler* ServerHandler::ServerHandlerImpl::newConnection( const net::LocalEndpointR& local )
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

net::ConnectionHandler* ServerHandler::newConnection( const net::LocalEndpointR& local )
{
	return m_impl->newConnection( local );
}

} // namespace _Wolframe
