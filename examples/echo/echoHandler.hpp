//
// echoHandler.hpp - simple echo handler example
//

#ifndef _ECHO_HANDLER_HPP_INCLUDED
#define _ECHO_HANDLER_HPP_INCLUDED

#include "connectionHandler.hpp"
#include "handlerConfig.hpp"

namespace _Wolframe {
	/// The connection handler
	class echoConnection : public net::connectionHandler
	{
	public:
		echoConnection( const net::LocalEndpoint& local, unsigned short timeout );

		~echoConnection();

		void setPeer( const net::RemoteEndpoint& remote );

		/// Parse incoming data. The return value indicates how much of the
		/// input has been consumed.
		void networkInput( const void *begin, std::size_t bytesTransferred );

		void timeoutOccured();
		void signalOccured();
		void errorOccured( NetworkSignal );

		/// Handle a request and produce a reply.
		const net::NetworkOperation nextOperation();

	private:
		enum State	{
			NEW,
			HELLO_SENT,
			READ_INPUT,
			OUTPUT_MSG,
			TIMEOUT,
			SIGNALLED,
			TERMINATE,
			FINISHED
		};

		static const std::size_t ReadBufSize = 8192;
		/// The state of the processor FSM
		State		state_;
		/// Read buffer
		char		readBuf_[ ReadBufSize ];
		char*		dataStart_;
		std::size_t	dataSize_;
		/// Output buffer
		std::string	outMsg_;
		/// Idle timeout value
		unsigned	idleTimeout_;
	};


	/// The server handler container
	class ServerHandler::ServerHandlerImpl
	{
	public:
		ServerHandlerImpl( const HandlerConfiguration *config )
		{
			timeout = config->echoConfig->timeout;
		}
		net::connectionHandler* newConnection( const net::LocalEndpoint& local );

	private:
		short unsigned timeout;
	};

} // namespace _Wolframe

#endif // _ECHO_HANDLER_HPP_INCLUDED
