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
// connectionBase.hpp
//

#ifndef _CONNECTION_BASE_HPP_INCLUDED
#define _CONNECTION_BASE_HPP_INCLUDED

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <string>
#include <cassert>

#ifdef WITH_SSL
#include <boost/asio/ssl/error.hpp>
#endif // WITH_SSL

#include "connectionHandler.hpp"
#include "logger-v1.hpp"

namespace _Wolframe {
namespace net {

/// Represents a single connection from a client.
template< typename socketType >
class ConnectionBase : public boost::enable_shared_from_this< ConnectionBase< socketType > >,
		private boost::noncopyable
{
public:
	/// Construct a connection with the given io_service.
	explicit ConnectionBase( boost::asio::io_service& IOservice,
				 ConnectionHandler* handler ) :
		m_strand( IOservice ),
		m_connHandler( handler ),
		m_timer( IOservice )
	{
		assert( handler != NULL );
		m_readBuffer = NULL;
	}

	virtual ~ConnectionBase()
	{
		delete m_connHandler;
	}

	/// Get the socket associated with the connection.
	virtual socketType& socket() = 0;

	/// Start the first asynchronous operation for the connection.
	virtual void start() = 0;

	/// Unregister the connection from the list of active connections
	virtual void unregister() = 0;

	/// Dispatch a signal for the processor
	void signal()
	{
		LOG_TRACE << "Signalling handler for connection to " << identifier();
		m_strand.dispatch( boost::bind( &ConnectionBase::handleSignal,
						this->shared_from_this() ));
		LOG_TRACE << "Handler for connection to " << identifier() << " signalled";
	}

	/// Set the connection identifier (i.e. remote endpoint).
	void identifier( const std::string& name )	{ m_ID = name; }

	/// Get the connection identifier (i.e. remote endpoint).
	const std::string& identifier()			{ return m_ID; }

protected:
	/// Connection identification string (i.e. remote endpoint)
	std::string			m_ID;

	/// Strand to ensure the connection's handlers are not called concurrently.
	boost::asio::io_service::strand	m_strand;

	/// Pointer to the read buffer
	void*				m_readBuffer;

	/// The handler used to process the incoming request.
	ConnectionHandler		*m_connHandler;

	/// The timer for timeouts.
	boost::asio::deadline_timer	m_timer;

public:
	/// Connection base state machine
	void nextOperation()
	{
		NetworkOperation netOp = m_connHandler->nextOperation();
		switch ( netOp.operation() )	{
			case NetworkOperation::READ:	{
				LOG_TRACE << "Next operation: READ " << netOp.size() << " bytes from " << identifier();
				if ( netOp.buffer() == NULL )	{
					LOG_FATAL << "Attempt to READ from " << identifier() << " to a NULL data block";
					abort();		// here should be a system exception
				}
				if ( netOp.size() == 0 )	{
					LOG_FATAL << "Attempt to READ 0 bytes data block from " << identifier();
					abort();		// here should be a system exception
				}
				if ( netOp.timeout() > 0 )
					setTimeout( netOp.timeout());
				m_readBuffer = netOp.buffer();
				socket().async_read_some( boost::asio::buffer( m_readBuffer, netOp.size() ),
							  m_strand.wrap( boost::bind( &ConnectionBase::handleRead,
										      this->shared_from_this(),
										      boost::asio::placeholders::error,
										      boost::asio::placeholders::bytes_transferred )));
			}
				break;

			case NetworkOperation::WRITE:	{
				LOG_TRACE << "Next operation: WRITE " << netOp.size() << " bytes to " << identifier();
				if ( netOp.data() == NULL )	{
					LOG_FATAL << "Attempt to WRITE a NULL data block to " << identifier();
					abort();		// here should be a system exception
				}
				if ( netOp.size() == 0 )	{
					LOG_FATAL << "Attempt to WRITE a 0 bytes data block to " << identifier();
					abort();		// here should be a system exception
				}
				if ( netOp.timeout() > 0 )
					setTimeout( netOp.timeout());
				boost::asio::async_write( socket(),
							  boost::asio::buffer( netOp.data(), netOp.size() ),
							  m_strand.wrap( boost::bind( &ConnectionBase::handleWrite,
										      this->shared_from_this(),
										      boost::asio::placeholders::error )));
			}
				break;

			case NetworkOperation::CLOSE:	{
				LOG_TRACE << "Next operation: CLOSE connection to " << identifier();
				// Initiate graceful connection closure.
				setTimeout( 0 );
				if ( socket().lowest_layer().is_open() )	{
					boost::system::error_code ignored_ec;
					socket().lowest_layer().shutdown( boost::asio::ip::tcp::socket::shutdown_both, ignored_ec );
					socket().lowest_layer().close();
				}
				unregister();
				LOG_DEBUG << "Connection to " << identifier() << " closed";
				break;
			}

			case NetworkOperation::NOOP:
				LOG_TRACE << "Next operation: NOOP on connection to " << identifier();
				break;
		}
	}

protected:
	/// Translate network error to processor error
	void signalError( const boost::system::error_code& e )
	{
		ConnectionHandler::NetworkSignal	ns;
		std::string				name;

		switch( e.value() )	{
		case boost::asio::error::eof :
			ns = ConnectionHandler::END_OF_FILE;
			name = "EOF";
			break;

		case boost::asio::error::operation_aborted :
			ns = ConnectionHandler::OPERATION_CANCELLED;
			name = "OPERATION CANCELLED";
			break;

		case boost::asio::error::broken_pipe :
			ns = ConnectionHandler::BROKEN_PIPE;
			name = "BROKEN PIPE";
			break;

		case boost::asio::error::connection_reset :
			ns = ConnectionHandler::CONNECTION_RESET;
			name = "CONNECTION RESET";
			break;

		default:	{
			std::string err = e.message();
#ifdef WITH_SSL
			if ( e.category() == boost::asio::error::get_ssl_category() )	{
				err = std::string( "(" )
						+ boost::lexical_cast< std::string >( ERR_GET_LIB( e.value() ) ) + ", "
						+ boost::lexical_cast< std::string >( ERR_GET_FUNC( e.value() ) )+ ", "
						+ boost::lexical_cast< std::string >( ERR_GET_REASON( e.value() ) ) + ")";
				//ERR_PACK /* crypto/err/err.h */
				char buf[ 128 ];
				::ERR_error_string_n( e.value(), buf, sizeof( buf ) );
				err += buf;
			}
#endif // WITH_SSL
			LOG_DEBUG << "Unknown error: " << e.value() << ", category: " << e.category().name()
				  << ", message: " << err;
			ns = ConnectionHandler::UNKNOWN_ERROR;
			name = "UNKNOWN ERROR";
			break;
			}
		}
		m_connHandler->signalOccured( ns );
		LOG_DATA << "Signalled " << name << " to processor for connection to " << identifier();
	}


	/// Handle completion of a read operation.
	void handleRead( const boost::system::error_code& e, std::size_t bytesTransferred )
	{
		setTimeout( 0 );
		if ( !e )	{
			LOG_TRACE << "Read " << bytesTransferred << " bytes from " << identifier();
			m_connHandler->networkInput( m_readBuffer, bytesTransferred );
		}
		else	{
			LOG_TRACE << "Read error: " << e.message();
			signalError( e );
		}
		nextOperation();
	}
	// handleRead function end


	/// Handle completion of a write operation.
	void handleWrite( const boost::system::error_code& e )
	{
		setTimeout( 0 );
		if ( !e )	{
			;
		}
		else	{
			LOG_DATA << "Write error: " << e.message();
			signalError( e );
		}
		nextOperation();
	}
	// handleWrite function end


	/// Handle the completion of a timer operation.
	void handleTimeout( const boost::system::error_code& e )
	{
		if ( !e )	{
			m_connHandler->signalOccured( ConnectionHandler::TIMEOUT );
			LOG_DEBUG << "Timeout on connection to " << identifier();

			nextOperation();
		}
		else	{
			assert( e == boost::asio::error::operation_aborted );
		}
	}
	// handleTimeout function end


	/// Set / reset the timeout timer
	void setTimeout( unsigned timeout )
	{
		if ( timeout == 0 )	{
			LOG_TRACE << "Timeout for connection to " << identifier() << " reset";
			m_timer.cancel();
		}
		else	{
			m_timer.cancel();
			m_timer.expires_from_now( boost::posix_time::seconds( timeout ));
			m_timer.async_wait( m_strand.wrap( boost::bind( &ConnectionBase::handleTimeout,
									this->shared_from_this(),
									boost::asio::placeholders::error )));
			LOG_TRACE << "Timeout for connection to " << identifier() << " set to " << timeout << "s";
		}
	}
	// setTimeout function end


	/// Handle a signal from above
	/// For now we handle only terminate
	void handleSignal()
	{
		LOG_TRACE << "Signal received for connection to " << identifier();
		m_connHandler->signalOccured( ConnectionHandler::TERMINATE );

		nextOperation();
	}
	// handleSignal function end
};

}} // namespace _Wolframe::net

#endif // _CONNECTION_BASE_HPP_INCLUDED
