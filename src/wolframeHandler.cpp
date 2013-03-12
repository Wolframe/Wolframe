/************************************************************************

 Copyright (C) 2011 - 2013 Project Wolframe.
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
	  m_readBuf( 16536 ), m_outputBuf( 4096 )
{
	m_remoteEP = NULL;
	net::ConnectionEndpoint::ConnectionType type = local.type();

	switch ( type )	{
		case net::ConnectionEndpoint::UDP:
			LOG_FATAL << "UDP local connection type not implemented";
			abort();

		case net::ConnectionEndpoint::TCP:	{
			const net::LocalTCPendpoint& lcl = dynamic_cast< const net::LocalTCPendpoint& >( local );
			m_localEP = new net::LocalTCPendpoint( lcl );
			LOG_TRACE << "Created connection handler for " << m_localEP->toString();
			break;
		}
#ifdef WITH_SSL
		case net::ConnectionEndpoint::SSL:	{
			const net::LocalSSLendpoint& lcl = dynamic_cast< const net::LocalSSLendpoint& >( local );
			m_localEP = new net::LocalSSLendpoint( lcl );
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
//	m_proc = NULL;
	m_cmdHandler.setInputBuffer( m_readBuf.charptr(), m_readBuf.size() );
	m_cmdHandler.setOutputBuffer( m_outputBuf.charptr(), m_outputBuf.size() );
	m_cmdHandler.setProcProvider( &context.proc());
}


wolframeConnection::~wolframeConnection()
{
	if ( m_localEP )
		delete m_localEP;
	if ( m_remoteEP )
		delete m_remoteEP;
	if ( m_authorization )	{
		m_authorization->close();
		m_authorization = NULL;
	}
	if ( m_authentication )	{
		m_authentication->close();
		m_authentication = NULL;
	}
	if ( m_audit )	{
		m_audit->close();
		m_audit = NULL;
	}

	LOG_TRACE << "Connection handler destroyed";
}


void wolframeConnection::setPeer( const net::RemoteEndpoint& remote )
{
	net::ConnectionEndpoint::ConnectionType type = remote.type();

	switch ( type )	{
		case net::ConnectionEndpoint::UDP:
			LOG_FATAL << "UDP local connection type not implemented";
			abort();

		case net::ConnectionEndpoint::TCP:	{
			const net::RemoteTCPendpoint& rmt = dynamic_cast< const net::RemoteTCPendpoint& >( remote );
			m_remoteEP = new net::RemoteTCPendpoint( rmt );
			LOG_TRACE << "Peer set to " << m_remoteEP->toString() << ", connected at "
				  << boost::posix_time::from_time_t( m_remoteEP->connectionTime());
			break;
		}

		case net::ConnectionEndpoint::SSL:
#ifdef WITH_SSL
		{
			const net::RemoteSSLendpoint& rmt = dynamic_cast<const net::RemoteSSLendpoint&>( remote );
			m_remoteEP = new net::RemoteSSLendpoint( rmt );
			LOG_TRACE << "Peer set to " << m_remoteEP->toString() << ", connected at " << boost::posix_time::from_time_t( m_remoteEP->connectionTime());
			if ( rmt.SSLcertInfo() )	{
				LOG_TRACE << "Peer SSL certificate serial number " << rmt.SSLcertInfo()->serialNumber()
					  << ", issued by: " << rmt.SSLcertInfo()->issuer();
				LOG_TRACE << "Peer SSL certificate valid from " << boost::posix_time::from_time_t( rmt.SSLcertInfo()->notBefore())
					  << " to " <<  boost::posix_time::from_time_t( rmt.SSLcertInfo()->notAfter());
				LOG_TRACE << "Peer SSL certificate subject: " << rmt.SSLcertInfo()->subject();
				LOG_TRACE << "Peer SSL certificate Common Name: " << rmt.SSLcertInfo()->commonName();
			}
			break;
		}
#endif // WITH_SSL
		default:
			LOG_FATAL << "Impossible remote connection type !";
			abort();
	}

	// Check if the connection is allowed
	if (( m_authorization = m_globalCtx.aaaa().authorizer()))	{
		if ( m_authorization->allowed( AAAA::ConnectInfo( *m_localEP, *m_remoteEP )))	{
			LOG_DEBUG << "Connection from " << m_remoteEP->toString()
				  << " to " << m_localEP->toString() << " authorized";
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

	for (ii=0,blkidx=0; ii<size; ii+=16,++blkidx)
	{
		std::string hexblk;
		std::string chrblk;
		std::size_t ci,ce;
		for (ci = 0, ce = ((size-ii) >= 16)?16:(size-ii); ci < ce; ++ci)
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
			hexblk.push_back( ' ');
		}
		for(; ci < 16; ++ci)
		{
			chrblk.push_back( ' ');
			hexblk.push_back( ' ');
			hexblk.push_back( ' ');
			hexblk.push_back( ' ');
		}
		std::string idxstr;
		idxstr.push_back( hex[ (blkidx % 256) / 16]);
		idxstr.push_back( hex[ (blkidx % 256) % 16]);

		LOG_DATA << "[" << title << " " << idxstr << "] " << chrblk << " - " << hexblk;
	}
}

/// Handle a request and produce a reply.
const net::NetworkOperation wolframeConnection::nextOperation()
{
	for ( ;; )	{
		LOG_TRACE << "STATE wolframeConnection handler " << stateName( m_state );
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
				m_cmdHandler.putInput( m_readBuf.charptr(), m_readBuf.pos());
				return net::NetworkOperation( net::ReadData( m_readBuf.ptr(), m_readBuf.size(), 30 ));
			}

			case TIMEOUT_OCCURED:	{
				m_state = TERMINATING;
				return net::NetworkOperation( net::SendString( std::string(m_cmdHandler.interruptDataSessionMarker()) + "BYE Timeout. :P\n" ));
			}

			case SIGNALLED:	{
				if ( m_state == TERMINATING )	{
					return net::NetworkOperation( net::NoOp() );
				}
				else	{
					m_state = TERMINATING;
					return net::NetworkOperation( net::SendString( std::string(m_cmdHandler.interruptDataSessionMarker()) + "BYE Server is shutting down. :P\n" ));
				}
			}

			case FORBIDDEN:	{
				m_state = TERMINATING;
				return net::NetworkOperation( net::SendString( std::string(m_cmdHandler.interruptDataSessionMarker()) + "BYE Access denied.\n" ));
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
				switch( m_cmdHandler.nextOperation() )	{
					case cmdbind::CommandHandler::READ:
						m_cmdHandler.getInputBlock( inpp, inppsize);
						return net::ReadData( inpp, inppsize);

					case cmdbind::CommandHandler::WRITE:
						m_cmdHandler.getOutput( outpp, outppsize);
						if ( _Wolframe::log::LogBackend::instance().minLogLevel() <= _Wolframe::log::LogLevel::LOGLEVEL_DATA)
						{
							logNetwork( "conn write", outpp, outppsize);
						}
						return net::SendData( outpp, outppsize);

					case cmdbind::CommandHandler::CLOSE:
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
	LOG_DATA << "network Input: Read " << bytesTransferred << " bytes";
	if ( _Wolframe::log::LogBackend::instance().minLogLevel() <= _Wolframe::log::LogLevel::LOGLEVEL_DATA)
	{
		logNetwork( "conn read", begin, bytesTransferred);
	}
	if ( m_state == COMMAND_HANDLER )	{
		m_cmdHandler.putInput( begin, bytesTransferred );
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
			if ( m_state != TERMINATING && m_state != FINISHED )
				m_state = SIGNALLED;
			break;

		case TIMEOUT:
			LOG_TRACE << "Processor received timeout signal";
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
