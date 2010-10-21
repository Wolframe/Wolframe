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
#ifdef _WIN32
// avoid C4003: not enough actual parameters for macro 'max'
#undef max
#undef min
#endif
#include <limits>

#include "connectionHandler.hpp"
#include "logger.hpp"

namespace _SMERP {

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
			timeoutID_ = std::numeric_limits<unsigned>::max();
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

		/// Set the connection identifier (i.e. remote endpoint).
		void identifier( const std::string& name )	{ identifier_ = name; }

		/// Get the connection identifier (i.e. remote endpoint).
		const std::string& identifier()	{ return identifier_; }

	protected:
		/// Connection identification string (i.e. remote endpoint)
		std::string	identifier_;

		/// Strand to ensure the connection's handlers are not called concurrently.
		boost::asio::io_service::strand	strand_;

		/// Buffer for incoming data.
		boost::array<char, 8192>	buffer_;

		/// The handler used to process the incoming request.
		connectionHandler		*connectionHandler_;

		/// The timer for timeouts.
		boost::asio::deadline_timer	timer_;
		/// The timeout ID
		unsigned			timeoutID_;


		/// Connection base state machine
		void nextOperation()
		{
			NetworkOperation netOp = connectionHandler_->nextOperation();
			switch ( netOp.operation() )	{
			case NetworkOperation::SET_TIMEOUT:
				LOG_TRACE << "Next operation: SET TIMEOUT id: " << netOp.timeoutID() << " to " << netOp.timeout() << "s";
				setTimeout( netOp.timeout(), netOp.timeoutID());
				break;
			case NetworkOperation::READ:
				LOG_TRACE << "Next operation: READ from " << identifier();
				socket().async_read_some( boost::asio::buffer( buffer_ ),
							 strand_.wrap( boost::bind( &connectionBase::handleRead,
										    this->shared_from_this(),
										    boost::asio::placeholders::error,
										    boost::asio::placeholders::bytes_transferred )));
				break;
			case NetworkOperation::WRITE:
				LOG_TRACE << "Next operation: WRITE to " << identifier();
				boost::asio::async_write( socket(),
							  boost::asio::buffer( netOp.data(), netOp.size() ),
							  strand_.wrap( boost::bind( &connectionBase::handleWrite,
										     this->shared_from_this(),
										     boost::asio::placeholders::error )));
				break;
			case NetworkOperation::TERMINATE:
				LOG_TRACE << "Next operation: TERMINATE connection to " << identifier();
				// Initiate graceful connection closure.
				boost::system::error_code ignored_ec;
				socket().lowest_layer().shutdown( boost::asio::ip::tcp::socket::shutdown_both, ignored_ec );
				break;
			}
		}


		/// Handle completion of a read operation.
		void handleRead( const boost::system::error_code& e, std::size_t bytesTransferred )
		{
			if ( !e )	{
				LOG_TRACE << "Read " << bytesTransferred << " bytes from " << identifier();

				connectionHandler_->parseInput( buffer_.data(), bytesTransferred );
				nextOperation();
			}

			// If an error occurs then no new asynchronous operations are started. This
			// means that all shared_ptr references to the connection object will
			// disappear and the object will be destroyed automatically after this
			// handler returns. The connection class's destructor closes the socket.
		}
		// handleRead function end


		/// Handle completion of a write operation.
		void handleWrite( const boost::system::error_code& e )
		{
			if ( !e )	{
				nextOperation();
			}

			// No new asynchronous operations are started. This means that all shared_ptr
			// references to the connection object will disappear and the object will be
			// destroyed automatically after this handler returns. The connection class's
			// destructor closes the socket.
		}
		// handleWrite function end



		/// Handle completion of a timer operation.
		void handleTimeout( const boost::system::error_code& e )
		{
			if ( !e )	{
				connectionHandler_->timeoutOccured( timeoutID_ );
				LOG_DEBUG << "Timeout, id: " << timeoutID_;
				nextOperation();
			}
		}
		// handleTimeout function end


		/// Set / reset timeout timer
		void setTimeout( unsigned long timeout, unsigned ID )
		{
			if ( timeout == 0 )	{
				timeoutID_ = std::numeric_limits<unsigned>::max();
			}
			else	{
				if ( timeoutID_ != ID )	{
					timer_.cancel();
					timeoutID_ = ID;
					timer_.expires_from_now( boost::posix_time::seconds( timeout ));
					timer_.async_wait( strand_.wrap( boost::bind( &connectionBase::handleTimeout,
										      this->shared_from_this(),
										      boost::asio::placeholders::error )));
					LOG_TRACE << "Timeout for connection to " << identifier() << " set to " << timeout << "s";
				}
			}
		}
		// setTimeout function end
	};

} // namespace _SMERP

#endif // _CONNECTION_BASE_HPP_INCLUDED
