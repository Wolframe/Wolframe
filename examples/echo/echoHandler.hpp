//
// echoHandler.hpp - simple echo handler example
//

#ifndef _ECHO_HANDLER_HPP_INCLUDED
#define _ECHO_HANDLER_HPP_INCLUDED

#include "system/connectionHandler.hpp"
#include "handlerConfig.hpp"

namespace _Wolframe {
	/// The connection handler
	class echoConnection : public net::ConnectionHandler
	{
	public:
		echoConnection( const net::LocalEndpointR& local, unsigned short timeout );

		~echoConnection();

		void setPeer( const net::RemoteEndpointR& remote );

		/// Parse incoming data. The return value indicates how much of the
		/// input has been consumed.
		void networkInput( const void *begin, std::size_t bytesTransferred );

		void signalOccured( NetworkSignal );

		/// Handle a request and produce a reply.
		const net::NetworkOperation nextOperation();

	private:
		enum State	{
			NEW,
			HELLO_SENT,
			READ_INPUT,
			OUTPUT_MSG,
			TIMEOUT_OCCURED,
			SIGNALLED,
			TERMINATING,
			FINISHED
		};

		static const std::size_t ReadBufSize = 8192;
		/// The state of the processor FSM
		State		m_state;
		/// Read buffer
		char		m_readBuf[ ReadBufSize ];
		char*		m_dataStart;
		std::size_t	m_dataSize;
		/// Output buffer
		std::string	m_outMsg;
		/// Idle timeout value
		unsigned	m_idleTimeout;
	};


	/// The server handler container
	class ServerHandler::ServerHandlerImpl
	{
	public:
		ServerHandlerImpl( const HandlerConfiguration *conf )
		{
			timeout = conf->echoConfig->timeout;
		}
		net::ConnectionHandler* newConnection( const net::LocalEndpointR& local );

	private:
		short unsigned timeout;
	};

} // namespace _Wolframe

#endif // _ECHO_HANDLER_HPP_INCLUDED
