//
// wolframeHandler.hpp - wolframe main handler
//

#ifndef _Wolframe_HANDLER_HPP_INCLUDED
#define _Wolframe_HANDLER_HPP_INCLUDED

#include "connectionHandler.hpp"

namespace _Wolframe {
	/// The connection handler
	class wolframeConnection : public Network::connectionHandler
	{
	public:
		wolframeConnection( const Network::LocalTCPendpoint& local );
#ifdef WITH_SSL
		wolframeConnection( const Network::LocalSSLendpoint& local );
#endif // WITH_SSL
		~wolframeConnection();

		void setPeer( const Network::RemoteTCPendpoint& remote );
#ifdef WITH_SSL
		void setPeer( const Network::RemoteSSLendpoint& remote );
#endif // WITH_SSL
		/// Parse / get the incoming data.
		void networkInput( const void *begin, std::size_t bytesTransferred );

		void timeoutOccured();
		void signalOccured();
		void errorOccured( NetworkSignal );

		/// Handle a request and produce a reply.
		const Network::NetworkOperation nextOperation();

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
		Network::connectionHandler* newConnection( const Network::LocalTCPendpoint& local );
#ifdef WITH_SSL
		Network::connectionHandler* newSSLconnection( const Network::LocalSSLendpoint& local );
#endif // WITH_SSL
	};

} // namespace _Wolframe

#endif // _Wolframe_HANDLER_HPP_INCLUDED
