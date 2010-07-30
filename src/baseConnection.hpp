//
// baseConnection.hpp
//

#ifndef _BASE_CONNECTION_HPP_INCLUDED
#define _BASE_CONNECTION_HPP_INCLUDED

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <string>

#include "reply.hpp"
#include "request.hpp"
#include "requestHandler.hpp"

namespace _SMERP {

	/// Represents a single connection from a client.
	class baseConnection : public boost::enable_shared_from_this<baseConnection>,
			       private boost::noncopyable
	{
	public:
		/// Construct a connection with the given io_service.
		explicit baseConnection( boost::asio::io_service& io_service,
					 requestHandler& handler,
					 unsigned long idleTimeout, unsigned long requestTimeout,
					 unsigned long processTimeout, unsigned long answerTimeout );

		/// Get the socket associated with the connection.
		virtual boost::asio::ip::tcp::socket& socket() = 0;

		/// Start the first asynchronous operation for the connection.
		virtual void start() = 0;

		/// Set the connection identifier (i.e. remote endpoint).
		void identifier( const std::string& name )	{ identifier_ = name; }

		/// Get the connection identifier (i.e. remote endpoint).
		const std::string& identifier()	{ return identifier_; }

	protected:
		enum TimeOutType	{
			TIMEOUT_NONE,
			TIMEOUT_IDLE,
			TIMEOUT_REQUEST,
			TIMEOUT_PROCESSING,
			TIMEOUT_ANSWER
		};

		/// Set / reset timeout timer
		void setTimeout( const TimeOutType type );

		/// Handle completion of a read operation.
		void handleRead( const boost::system::error_code& e, std::size_t bytesTransferred );

		/// Handle completion of a write operation.
		void handleWrite( const boost::system::error_code& e );

		/// Handle completion of a timer operation.
		void handleTimeout( const boost::system::error_code& e );


		/// Connection identification string (i.e. remote endpoint)
		std::string	identifier_;

		/// Strand to ensure the connection's handlers are not called concurrently.
		boost::asio::io_service::strand	strand_;

		/// Buffer for incoming data.
		boost::array<char, 8192>	buffer_;

		/// The handler used to process the incoming request.
		requestHandler& requestHandler_;

		/// The incoming request.
		request	request_;

		/// The reply to be sent back to the client.
		reply	reply_;

		/// The timer for timeouts.
		boost::asio::deadline_timer	timer_;
		/// The timer type
		TimeOutType			timerType_;

		/// Idle connection timeout (milliseconds). 0 to disable.
		unsigned long idleTimeout_;

		/// Receiving a request timeout (milliseconds). 0 to disable.
		unsigned long requestTimeout_;

		/// Processing a request timeout (milliseconds). 0 to disable.
		unsigned long processTimeout_;

		/// Sending an answer timeout (milliseconds). 0 to disable.
		unsigned long answerTimeout_;
	};

	typedef boost::shared_ptr<baseConnection> connection_ptr;


	// map TimeoutType enum values to strings
	template< typename CharT, typename TraitsT >
	inline std::basic_ostream< CharT, TraitsT > &operator<< ( std::basic_ostream< CharT, TraitsT >& s,
								  baseConnection::TimeOutType t )
	{
		static const char *const str[] = {
			"NONE", "IDLE", "REQUEST", "PROCESSING", "ANSWER" };
		if( static_cast< size_t >( t ) < ( sizeof( str ) / sizeof( *str ) ) ) {
			s << str[t];
		} else {
			s << "Unknown enum used '" << static_cast< int >( t ) << "'";
		}
		return s;
	}

} // namespace _SMERP

#endif // _BASE_CONNECTION_HPP_INCLUDED
