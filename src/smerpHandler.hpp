//
// smerpHandler.hpp - smerp main handler
//

#ifndef _SMERP_HANDLER_HPP_INCLUDED
#define _SMERP_HANDLER_HPP_INCLUDED

#include "connectionHandler.hpp"

namespace _SMERP {
	/// The connection handler
	class echoConnection : public Network::connectionHandler
	{
	public:
		echoConnection( const Network::LocalTCPendpoint& local );
		echoConnection( const Network::LocalSSLendpoint& local );
		~echoConnection();

		void setPeer( const Network::RemoteTCPendpoint& remote );
		void setPeer( const Network::RemoteSSLendpoint& remote );

		/// Parse incoming data. The return value indicates how much of the
		/// input has been consumed.
		void networkInput( const void *begin, std::size_t bytesTransferred );

		void timeoutOccured();
		void signalOccured();
		void errorOccured( NetworkSignal );

		/// Handle a request and produce a reply.
		const Network::NetworkOperation nextOperation();

	private:
		enum State	{
			NEW,
			HELLO,
			READING,
			ANSWERING,
			FINISHING,
			TIMEOUT,
			SIGNALLED,
			CLOSING,
			TERMINATED
		};

		static const std::size_t ReadBufSize = 8192;
		State		state_;
		char		readBuf_[ ReadBufSize ];
		char*		bufStart_;
		std::size_t	bufSize_;
		std::string	outMsg_;
	};

	/// The server handler container
	class ServerHandler::ServerHandlerImpl
	{
	public:
		Network::connectionHandler* newConnection( const Network::LocalTCPendpoint& local );
		Network::connectionHandler* newSSLconnection( const Network::LocalSSLendpoint& local );
	};

} // namespace _SMERP

#endif // _SMERP_HANDLER_HPP_INCLUDED
