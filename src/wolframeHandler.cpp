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
#include "logger.hpp"
#include "standardConfigs.hpp"
#include "handlerConfig.hpp"

#ifdef WITH_SSL
#include "SSLcertificateInfo.hpp"
#endif

#include <string>
#include <cstring>

#include "boost/date_time/posix_time/posix_time.hpp"		// to print time_t structures

namespace _Wolframe	{

wolframeConnection::wolframeConnection( const wolframeHandler& context,
					const net::LocalEndpoint& local )
	: globalCtx_( context )
{
	localEP_ = &local;
	remoteEP_ = NULL;
	_Wolframe::net::ConnectionEndpoint::ConnectionType type = localEP_->type();

	switch ( type )	{
	case _Wolframe::net::ConnectionEndpoint::TCP_CONNECTION:	{
		const _Wolframe::net::LocalTCPendpoint* lcl = static_cast<const _Wolframe::net::LocalTCPendpoint*>( localEP_ );
		LOG_TRACE << "Created connection handler for " << lcl->toString();
		break;
	}
#ifdef WITH_SSL
	case _Wolframe::net::ConnectionEndpoint::SSL_CONNECTION:	{
		const _Wolframe::net::LocalSSLendpoint* lcl = static_cast<const _Wolframe::net::LocalSSLendpoint*>( localEP_ );
		LOG_TRACE << "Created connection handler (SSL) for " << lcl->toString();
		break;
	}
#else
	case _Wolframe::net::ConnectionEndpoint::SSL_CONNECTION:
#endif // WITH_SSL
	default:
		LOG_FATAL << "Impossible local connection type !";
		abort();
	}

	state_ = NEW;
	dataStart_ = NULL;
	dataSize_ = 0;
	idleTimeout_ = 30;
}


wolframeConnection::~wolframeConnection()
{
	LOG_TRACE << "Connection handler destroyed";
}

void wolframeConnection::setPeer( const net::RemoteEndpoint& remote )
{
	remoteEP_ = &remote;
	_Wolframe::net::ConnectionEndpoint::ConnectionType type = remoteEP_->type();

	switch ( type )	{
	case _Wolframe::net::ConnectionEndpoint::TCP_CONNECTION:	{
		const _Wolframe::net::RemoteTCPendpoint* rmt = static_cast<const _Wolframe::net::RemoteTCPendpoint*>( remoteEP_ );
		LOG_TRACE << "Peer set to " << rmt->toString() << ", connected at " << rmt->connectionTime();
		break;
	}
#ifdef WITH_SSL
	case _Wolframe::net::ConnectionEndpoint::SSL_CONNECTION:	{
		const _Wolframe::net::RemoteSSLendpoint* rmt = static_cast<const _Wolframe::net::RemoteSSLendpoint*>( remoteEP_ );
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
	case _Wolframe::net::ConnectionEndpoint::SSL_CONNECTION:
#endif // WITH_SSL
	default:
		LOG_FATAL << "Impossible remote connection type !";
		abort();
	}
}

/// Handle a request and produce a reply.
const net::NetworkOperation wolframeConnection::nextOperation()
{
	switch( state_ )	{
	case NEW:	{
		state_ = HELLO_SENT;
		if ( ! globalCtx_.banner().empty() )
			outMsg_ = globalCtx_.banner() + "\nOK\n";
		else
			outMsg_ = "OK\n";
		return net::NetworkOperation( net::SendString( outMsg_ ));
	}

	case HELLO_SENT:	{
		state_ = READ_INPUT;
		return net::NetworkOperation( net::ReadData( readBuf_, ReadBufSize, idleTimeout_ ));
	}

	case READ_INPUT:
		dataStart_ = readBuf_;
		// Yes, it continues with OUTPUT_MSG, sneaky, sneaky, sneaky :P

	case OUTPUT_MSG:
		if ( !strncmp( "quit", dataStart_, 4 ))	{
			state_ = TERMINATE;
			return net::NetworkOperation( net::SendString( "Bye\n" ));
		}
		else	{
			char *s = dataStart_;
			for ( std::size_t i = 0; i < dataSize_; i++ )	{
				if ( *s == '\n' )	{
					s++;
					outMsg_ = std::string( dataStart_, s - dataStart_ );
					dataSize_ -= s - dataStart_;
					dataStart_ = s;
					state_ = OUTPUT_MSG;
					return net::NetworkOperation( net::SendString( outMsg_ ));
				}
				s++;
			}
			// If we got here, no \n was found, we need to read more
			// or close the connection if the buffer is full
			if ( dataSize_ >= ReadBufSize )	{
				state_ = TERMINATE;
				return net::NetworkOperation( net::SendString( "Line too long. Bye.\n" ));
			}
			else {
				memmove( readBuf_, dataStart_, dataSize_ );
				state_ = READ_INPUT;
				return net::NetworkOperation( net::ReadData( readBuf_ + dataSize_,
									     ReadBufSize - dataSize_,
									     idleTimeout_ ));
			}
		}

	case TIMEOUT:	{
		state_ = TERMINATE;
		return net::NetworkOperation( net::SendString( "Timeout. :P\n" ));
	}

	case SIGNALLED:	{
		state_ = TERMINATE;
		return net::NetworkOperation( net::SendString( "Server is shutting down. :P\n" ));
	}

	case TERMINATE:	{
		state_ = FINISHED;
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
void wolframeConnection::networkInput( const void*, std::size_t bytesTransferred )
{
	LOG_DATA << "network Input: Read " << bytesTransferred << " bytes";
	dataSize_ += bytesTransferred;
}

void wolframeConnection::timeoutOccured()
{
	state_ = TIMEOUT;
	LOG_TRACE << "Processor received timeout";
}

void wolframeConnection::signalOccured()
{
	state_ = SIGNALLED;
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
		LOG_TRACE << "Processor received OPERATION_CANCELED (should have been requested by us)";
		break;

	case UNKNOWN_ERROR:
		LOG_TRACE << "Processor received an UNKNOWN error from the framework";
		break;
	}
	state_ = TERMINATE;
}


/// The server handler global context
wolframeHandler::wolframeHandler( const HandlerConfiguration* config )
	: banner_( config->banner->toString() ),
	  db_( *config->database )
{
	//		banner_ = config->banner->toString();
	LOG_TRACE << "Global context: banner: <" << banner_ << ">";
}

wolframeHandler::~wolframeHandler()
{
	LOG_TRACE << "Global context destroyed";
}


/// ServerHandler PIMPL
net::connectionHandler* ServerHandler::ServerHandlerImpl::newConnection( const net::LocalEndpoint& local )
{
	return new wolframeConnection( globalContext_, local );
}

ServerHandler::ServerHandlerImpl::ServerHandlerImpl( const HandlerConfiguration* config )
	: globalContext_( config )	{}

ServerHandler::ServerHandlerImpl::~ServerHandlerImpl()	{}

/// Outside face of the PIMPL
ServerHandler::ServerHandler( const HandlerConfiguration* config )
	: impl_( new ServerHandlerImpl( config ) )	{}

ServerHandler::~ServerHandler()	{ delete impl_; }

net::connectionHandler* ServerHandler::newConnection( const net::LocalEndpoint& local )
{
	return impl_->newConnection( local );
}

} // namespace _Wolframe
