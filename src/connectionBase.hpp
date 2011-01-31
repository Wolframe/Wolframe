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
#include <string>
#include <cassert>

#include "connectionHandler.hpp"
#include "logger.hpp"

namespace _SMERP {
	namespace Network {

	/// Represents a single connection from a client.
	template< typename socketType >
	class connectionBase : public boost::enable_shared_from_this< connectionBase< socketType > >,
			       private boost::noncopyable
	{
	public:
		/// Construct a connection with the given io_service.
		explicit connectionBase( boost::asio::io_service& IOservice,
						connectionHandler* handler ) :
			strand_( IOservice ),
			connectionHandler_( handler ),
			timer_( IOservice )
		{
			assert( handler != NULL );
			connectionHandler_ = handler;
			readBuffer_ = NULL;
			LOG_TRACE << "New connection base created";
		}

		~connectionBase()
		{
			delete connectionHandler_;
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
			strand_.dispatch( boost::bind( &connectionBase::handleSignal,
						       this->shared_from_this() ));
			LOG_TRACE << "Handler for connection to " << identifier() << " signalled";
		}

		/// Set the connection identifier (i.e. remote endpoint).
		void identifier( const std::string& name )	{ identifier_ = name; }

		/// Get the connection identifier (i.e. remote endpoint).
		const std::string& identifier()			{ return identifier_; }

	protected:
		/// Connection identification string (i.e. remote endpoint)
		std::string			identifier_;

		/// Strand to ensure the connection's handlers are not called concurrently.
		boost::asio::io_service::strand	strand_;

		/// Pointer to the read buffer
		void*				readBuffer_;

		/// The handler used to process the incoming request.
		connectionHandler		*connectionHandler_;

		/// The timer for timeouts.
		boost::asio::deadline_timer	timer_;


		/// Connection base state machine
		void nextOperation()
		{
			NetworkOperation netOp = connectionHandler_->nextOperation();
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
				readBuffer_ = netOp.buffer();
				socket().async_read_some( boost::asio::buffer( readBuffer_, netOp.size() ),
							  strand_.wrap( boost::bind( &connectionBase::handleRead,
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
							  strand_.wrap( boost::bind( &connectionBase::handleWrite,
										     this->shared_from_this(),
										     boost::asio::placeholders::error )));
				}
				break;

			case NetworkOperation::CLOSE:	{
					LOG_TRACE << "Next operation: CLOSE connection to " << identifier();
					// Initiate graceful connection closure.
					setTimeout( 0 );
					boost::system::error_code ignored_ec;
					socket().lowest_layer().shutdown( boost::asio::ip::tcp::socket::shutdown_both, ignored_ec );
					socket().lowest_layer().close();
					unregister();
				}
				break;

			case NetworkOperation::TERMINATE:	{
					LOG_TRACE << "Next operation: TERMINATE on connection to " << identifier();
					// Initiate graceful connection closure.
					setTimeout( 0 );
					boost::system::error_code ignored_ec;
					if ( socket().lowest_layer().is_open() )	{
						socket().lowest_layer().shutdown( boost::asio::ip::tcp::socket::shutdown_both, ignored_ec );
						socket().lowest_layer().close();
					}
					unregister();
				}
				break;
			}
		}


		/// Translate network error to processor error
		void signalError( const boost::system::error_code& e )
		{
			connectionHandler::NetworkSignal	ns;
			std::string				name;

			switch( e.value() )	{
			case boost::asio::error::eof:
				ns = connectionHandler::END_OF_FILE;
				name = "EOF";
				break;

			case boost::asio::error::operation_aborted:
				ns = connectionHandler::OPERATION_CANCELLED;
				name = "OPERATION CANCELLED";
				break;

			case boost::asio::error::broken_pipe:
				ns = connectionHandler::BROKEN_PIPE;
				name = "BROKEN PIPE";
				break;

			default:
				LOG_DEBUG << "Unknown error: " << e.value() << ", message: " << e.message();
				ns = connectionHandler::UNKNOWN_ERROR;
				name = "UNKNOWN ERROR";
				break;
			}
			connectionHandler_->errorOccured( ns );
			LOG_DATA << "Signalled " << name << " to processor for connection to " << identifier();
		}


		/// Handle completion of a read operation.
		void handleRead( const boost::system::error_code& e, std::size_t bytesTransferred )
		{
			setTimeout( 0 );
			if ( !e )	{
				LOG_TRACE << "Read " << bytesTransferred << " bytes from " << identifier();
				connectionHandler_->networkInput( readBuffer_, bytesTransferred );
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
				connectionHandler_->timeoutOccured();
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
				timer_.cancel();
			}
			else	{
				timer_.cancel();
				timer_.expires_from_now( boost::posix_time::seconds( timeout ));
				timer_.async_wait( strand_.wrap( boost::bind( &connectionBase::handleTimeout,
									      this->shared_from_this(),
									      boost::asio::placeholders::error )));
				LOG_TRACE << "Timeout for connection to " << identifier() << " set to " << timeout << "s";
			}
		}
		// setTimeout function end


		/// Handle a signal from above
		void handleSignal()
		{
			LOG_TRACE << "Signal received for connection to " << identifier();
			connectionHandler_->signalOccured();

			nextOperation();
		}
		// handleSignal function end

	};

} // namespace Network
} // namespace _SMERP

#endif // _CONNECTION_BASE_HPP_INCLUDED
